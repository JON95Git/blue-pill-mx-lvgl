# LVGL port for STM32 Bluepill + ili9163 TFT display
This project implements a basic port of LVGL for STM32 Bluepill + ili9163 TFT display

# Introduction
The idea behind this project is allow user to undestand the basics of LVGL

# Graphical interface
There's just a simple widget (ARC) that moves based on a potentiometer value.

# Dependencies

## Software
To build this project you need:
- A PC `host` runing `GNU/Linux`
- Toolchain [arm-none-gnueabi-gcc](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads) for `arm cortex M` microcontrollers
- `Make` tool to use `Makefile`
- Library [LVGL](https://github.com/lvgl/lvgl) (up to `7.0`)
- [OpenOCD](http://openocd.org/) tool for build/debug firmware
- [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) to change peripheral

## Hardware
To run this project you need
- Devboard `STM32f103c8t6 - BluePill`
- Display TFT 128x160 ILI9163 
- Potentiometer - 10kohms