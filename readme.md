## STM32 Tetris

Project name: STM32 Tetris (STM32F429)

Description: This repository contains the source code for a Tetris game running on the STM32F429I-DISCO board. The project uses the ST HAL libraries, optional FreeRTOS for certain configurations, and TouchGFX/ILI9341 for the display. It supports the STMPE811 touch controller and audio playback from the `sounds/wav` folder.

Repository contents:
- Firmware source for STM32 (folders: `Core/Src`, `Drivers`, `STM32CubeIDE`, `EWARM`, `gcc`/Makefile).
- TouchGFX UI and assets (folders: `TouchGFX`, `App`, `assets`).
- Project configuration files for multiple IDEs (STM32CubeIDE, MDK-ARM, EWARM) and linker scripts for GCC.
- Audio resources in `sounds/wav` and an audio helper script at `sounds/main.py`.

---

> [!IMPORTANT]
> Before doing `git add .`, generate the code using TouchGFX again to overwrite the formatting changes of STM32CubeIDE. It is also *recommended* to use VSCode C/C++ formatter afterwards.

## Tool versions
Use these exact versions:
- STM32CubeIDE 1.19.0
- TouchGFX Designer 4.25.0

## Wiring details

## STM32F429I_DISCO_REV_D01 TBS

The default IDE is set to STM32CubeIDE, to change IDE open the STM32F429I_DISCO_REV_D01.ioc with STM32CubeMX and select from the supported IDEs (EWARM from version 8.50.9, MDK-ARM, and STM32CubeIDE). Supports flashing of the STM32F429I_DISCO_DEV_D01 board directly from TouchGFX Designer using GCC and STM32CubeProgrammer. Flashing the board requires STM32CubeProgrammer which can be downloaded from the ST webpage. 

This TBS is configured for 320 x 240 pixels 16bpp screen resolution.  

Performance testing can be done using the GPIO pins designated with the following signals: VSYNC_FREQ  - Pin PE2, RENDER_TIME - Pin PE3, FRAME_RATE  - Pin PE4, MCU_ACTIVE  - Pin PE5
