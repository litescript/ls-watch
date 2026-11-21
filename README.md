# PineTime Custom Firmware in C

### Work in progress!

Playing around with cheap, wearable embedded development.

Written in C, with a custom linker script and a sprinkling of inline ARM assembly. No OS, SDK, HAL, or external graphics library.

Yes it still needs tests. Among other things.

### Current status:

Boots on real PineTime hardware! Clock counts, wraps, and has dirty-region character drawing.

### Major Milestones:

* First successful execution on physical hardware: Nov 21, 2026
  * Initial working firmware footprint: 2,024 bytes!

### Featuring (cast in no particular order):
* graphics primitives
  * fill/void (rect, triangle, circle)
  * lines
  * single-pixel placement
  * display fill/clear
  * bitmap font with optional scaling
  * dirty region rendering
* timekeeping
  * hardware RTC configuration
  * rollover-safe elapsed tick calculation
  * fractional tick accumulation
  * 24-hour timekeeping
  * fixed-width HH:MM:SS rendering
* hardware drivers
  * [GPIO](src/gpio.c)
  * [SPI with EasyDMA](src/spi.c)
  * [ST7789 display](src/display.c)
  * [low-frequency clock and RTC](src/rtc.c)
  * [SysTick-based delays](src/timer.c)
* bare-metal runtime
  * [custom linker script](linker.ld) for memory layout (what a feature!)
  * [interrupt vector table](src/startup.c) (needs...fleshed out)
  * reset and default exception handlers (technically)
  * .data initialization and .bss zeroing
* development infrastructure
  * independent hardware and emulator build configurations
  * automated firmware flashing and verification via SWD
    * (required: human manually holding probe in place, gently)
  * compilation database generation for clangd
* eccentric [README](README.md)

## Acknowledgments

[Pine64](https://pine64.org/) for the [PineTime](https://pine64.org/devices/pinetime/) hardware this whole project is based around! *(not a sponsor!)*

Special thanks to [Weichwolf](https://github.com/Weichwolf) for developing [pinetime-emu](https://github.com/Weichwolf/pinetime-emu), an ARM Cortex-M4F and nRF52832 emulator for the PineTime. This project has been invaluable for developing and testing ls-watch firmware without requiring physical hardware for every iteration.

## License

ls-watch is licensed under the [Mozilla Public License 2.0](https://www.mozilla.org/en-US/MPL/2.0/).

You are free to use, modify, and distribute this software, including for commercial purposes, subject to the terms of the license. Modifications to MPL-covered source files must remain available under the MPL 2.0 when distributed.

See [LICENSE](LICENSE) for the full license text.
