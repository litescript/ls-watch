.DEFAULT_GOAL := all

.PHONY: all hardware emulator deps deps-dry-run check run flash clean size disasm tar compdb

CROSS   := arm-none-eabi-
CC      := $(CROSS)gcc
OBJCOPY := $(CROSS)objcopy
OBJDUMP := $(CROSS)objdump
SIZE    := $(CROSS)size

TARGET    := ls-watch
# Hardware is the default; each environment has independent build artifacts.
MODE ?= hardware
ifeq ($(MODE),hardware)
    MODE_FLAGS :=
else ifeq ($(MODE),emulator)
    MODE_FLAGS := -DEMULATOR
else
    $(error Invalid MODE '$(MODE)': choose hardware or emulator)
endif
BUILD_DIR := build/$(MODE)

SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

CPUFLAGS := -mcpu=cortex-m4 -mthumb \
            -mfpu=fpv4-sp-d16 -mfloat-abi=hard

CFLAGS := $(CPUFLAGS) $(MODE_FLAGS) \
          -std=c11 -Os -g3 \
          -ffreestanding -fno-builtin \
          -ffunction-sections -fdata-sections \
          -Wall -Wextra -Werror \
          -Iinclude

LDFLAGS := $(CPUFLAGS) \
           -nostdlib -nostartfiles \
           -T linker.ld \
           -Wl,--gc-sections \
           -Wl,-Map=$(BUILD_DIR)/$(TARGET).map \
           -Wl,--print-memory-usage

EMU ?= $(HOME)/dev/pinetime-emu/build/pinetime-emu

OPENOCD ?= openocd
SWD_SPEED ?= 100

all: $(BUILD_DIR)/$(TARGET).bin

hardware:
	$(MAKE) MODE=hardware all

emulator:
	$(MAKE) MODE=emulator all

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: src/%.c Makefile | $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) linker.ld Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@ -lgcc

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@
	$(SIZE) $<

deps:
	bash scripts/install-deps.sh

deps-dry-run:
	bash scripts/install-deps.sh --dry-run

check:
	@command -v $(CC)
	@command -v $(OBJCOPY)
	@command -v $(OBJDUMP)
	@command -v $(SIZE)
	@command -v cmake
	@command -v ninja
	@command -v git
	@command -v bear
	@echo "Tool checks passed."

run: emulator
	@test -x "$(EMU)" || { echo "Emulator not found: $(EMU)"; exit 1; }
	"$(EMU)" build/emulator/$(TARGET).bin

# Explicit hardware programming only; never performs automatic recovery.
flash: hardware
	$(OPENOCD) -f interface/cmsis-dap.cfg \
	    -c "transport select swd" -f target/nrf52.cfg \
	    -c "adapter speed $(SWD_SPEED)" \
	    -c "program build/hardware/$(TARGET).elf verify reset exit"

size: $(BUILD_DIR)/$(TARGET).elf
	$(SIZE) -A $<

disasm: $(BUILD_DIR)/$(TARGET).elf
	$(OBJDUMP) -d -S $<

tar:
	tar --exclude=.git \
	    --exclude=build \
	    --exclude=compile_commands.json \
	    --exclude=.cache \
	    -czf ../$(TARGET)-source.tar.gz .

clean:
	rm -rf build

# Generate compile_commands.json for clangd.
# -B ensures Make executes the compilation commands even if
# the object files are already up to date.
compdb:
	bear -- $(MAKE) MODE=$(MODE) -B all

-include $(OBJECTS:.o=.d)
