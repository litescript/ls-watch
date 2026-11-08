.DEFAULT_GOAL := all

.PHONY: all deps deps-dry-run check run clean size disasm tar compdb

CROSS   := arm-none-eabi-
CC      := $(CROSS)gcc
OBJCOPY := $(CROSS)objcopy
OBJDUMP := $(CROSS)objdump
SIZE    := $(CROSS)size

TARGET    := ls-watch
BUILD_DIR := build

SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

CPUFLAGS := -mcpu=cortex-m4 -mthumb \
            -mfpu=fpv4-sp-d16 -mfloat-abi=hard

CFLAGS := $(CPUFLAGS) \
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

all: $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) linker.ld
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

run: $(BUILD_DIR)/$(TARGET).bin
	@test -x "$(EMU)" || { echo "Emulator not found: $(EMU)"; exit 1; }
	"$(EMU)" $<

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
	rm -rf $(BUILD_DIR)

# Generate compile_commands.json for clangd.
# -B ensures Make executes the compilation commands even if
# the object files are already up to date.
compdb:
	bear -- $(MAKE) -B all

-include $(OBJECTS:.o=.d)
