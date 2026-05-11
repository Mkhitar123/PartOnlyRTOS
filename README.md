
# Part Only RTOS

A lightweight, modular operating environment for 8-bit microcontrollers (ATmega2560). It enables dynamic loading and execution of application modules directly from an SD card without reprogramming the MCU.

## Key Features
* **Dynamic Loading:** Load and run `.bin` modules at runtime via SD card.
* **Tiny Footprint:** Uses only ~2.5 KB of Flash and ~430 Bytes of RAM.
* **Memory Safety:** Built-in module validation (Pattern Matching) and Stack Canary protection.
* **Architecture:** 3-tier system (Launcher, custom SPM Bootloader, and Kernel).

## Hardware Requirements
* ATmega2560 (or compatible 8-bit AVR)
* SD/MicroSD Card Module (SPI)
* UART (for serial interface)

## Scientific Publication
For detailed technical analysis and architecture, please refer to the published paper:
**[Operational Environment for Dynamic Loading and Management of Modular Applications in Embedded Systems](https://journals.ysu.am/proceedings-phys-math/article/view/vol60_no1_2026_pp053-062)** *Proceedings of the YSU A: Physical and Mathematical Sciences, 2026.*
