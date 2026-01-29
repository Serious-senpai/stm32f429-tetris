# STM32 Tetris

A classic Tetris game implementation running on the STM32F429I-DISCO development board, featuring a touchscreen interface and external audio feedback.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Hardware Architecture](#hardware-architecture)
- [Software Architecture](#software-architecture)
- [Project Structure](#project-structure)
- [Audio System Setup](#audio-system-setup)
- [Usage](#usage)
- [Contributing](#contributing)

## Overview

This project implements the classic Tetris game on an STM32F429I-DISCO board. The game utilizes the onboard 2.4" QVGA TFT LCD with resistive touchscreen for display and input. Sound effects are transmitted via UART to an external host running a Python audio player.

## Features

- Classic Tetris gameplay with all 7 standard tetrominoes (I, O, T, S, Z, J, L)
- Touch-based controls for piece movement and rotation
- Real-time score tracking with persistent high score
- Smooth graphics rendering using DMA2D hardware acceleration
- External sound effects via UART communication
- Responsive UI built with TouchGFX framework

---

> [!IMPORTANT]
> Before doing `git add .`, generate the code using TouchGFX again to overwrite the formatting changes of STM32CubeIDE. It is also *recommended* to use VSCode C/C++ formatter afterwards.

## Hardware Architecture

### Target Platform

| Component | Specification |
|-----------|---------------|
| MCU | STM32F429ZIT6 (Cortex-M4, 180 MHz, FPU) |
| Flash | 2 MB internal |
| RAM | 256 KB (192 KB SRAM + 64 KB CCM) |
| External SDRAM | 8 MB (64 Mbit IS42S16400J) |

### Display Subsystem

The display subsystem consists of the ILI9341 LCD controller driving a 2.4" TFT panel. The LTDC (LCD-TFT Display Controller) peripheral manages frame buffer output with double buffering stored in external SDRAM. DMA2D (Chrom-ART Accelerator) handles hardware-accelerated 2D graphics operations including blitting, filling, and pixel format conversion.

| Parameter | Value |
|-----------|-------|
| Resolution | 240 x 320 pixels (QVGA, Portrait) |
| Color Depth | 16-bit RGB565 |
| Interface | Parallel RGB via LTDC |
| Frame Buffer | Double-buffered in SDRAM |

### Touch Input Subsystem

The STMPE811 touch controller provides resistive touchscreen input via I2C3 interface. Raw touch coordinates undergo calibration and filtering before being passed to the application layer. The controller supports both polling and interrupt-driven modes.

| Parameter | Value |
|-----------|-------|
| Controller | STMPE811 |
| Interface | I2C3 (address 0x82) |
| Type | 4-wire resistive |

### Audio Output Subsystem

Audio is implemented as an external system where the MCU transmits sound effect commands over UART1 to a connected host computer. The host runs a Python script that receives these commands and plays corresponding WAV files.

| Parameter | Value |
|-----------|-------|
| Interface | UART1 |
| Baud Rate | 115200 |
| Protocol | Null-terminated ASCII strings |

## Software Architecture

### Layer Overview

The software follows a layered architecture with clear separation of concerns:
```
+--------------------------------------------------+
|                Application Layer                 |
|        (Game Logic, UI Screens, Presenters)      |
+--------------------------------------------------+
|                 TouchGFX Framework               |
|        (Graphics Engine, Widget Library)         |
+--------------------------------------------------+
|                    FreeRTOS                      |
|         (Task Scheduling, Synchronization)       |
+--------------------------------------------------+
|               Hardware Abstraction               |
|          (HAL Drivers, BSP Components)           |
+--------------------------------------------------+
|                    Hardware                      |
|       (STM32F429, Peripherals, External ICs)     |
+--------------------------------------------------+
```

### TouchGFX MVP Pattern

The user interface implements the Model-View-Presenter (MVP) pattern as prescribed by TouchGFX:

**Model** contains the core game state and logic including the game board array, active tetromino tracking, score management, and collision detection algorithms. The Model operates independently of the display and processes game ticks to advance the game state.

**View** handles all visual representation through TouchGFX widgets. Each screen (Welcome, Game) has a corresponding View class that manages widget creation, positioning, and updates. The View receives display commands from the Presenter and translates them into widget manipulations.

**Presenter** acts as the intermediary between Model and View. It implements the ModelListener interface to receive game state change notifications and forwards relevant updates to the View. The Presenter also handles screen transitions and user input routing.

### Task Structure

FreeRTOS manages concurrent execution with the following task configuration:

| Task | Priority | Stack Size | Responsibility |
|------|----------|------------|----------------|
| TouchGFX | Normal | 4096 bytes | Graphics rendering, UI event handling |
| Default | Normal | 512 bytes | System initialization, idle processing |

Inter-task communication uses FreeRTOS message queues. The `channel` queue transmits button events from the input handler to the game model for processing.

### Game Logic

The Tetris game engine implements standard mechanics:

**Board Representation** uses a 10x18 cell array where each cell stores occupancy state. The visible play area spans rows 0-14, with rows 15-17 serving as the spawn zone. Game over triggers when any block occupies row 15 after piece placement.

**Tetromino Encoding** represents each piece as a 16-bit bitmap arranged in a 4x4 grid. Four rotation states are precomputed for each of the seven piece types, enabling O(1) rotation lookups.

**Collision Detection** validates moves by checking the proposed position against board boundaries and existing blocks. The algorithm accounts for the current piece position to avoid self-collision.

**Line Clearing** scans completed rows after each piece placement, removes full rows, and shifts all rows above downward. Each cleared line increments the score.

**Timing** controls piece descent at 1000ms intervals using FreeRTOS tick counting. Player inputs are processed immediately upon receipt.

### Graphics Pipeline

The rendering pipeline leverages hardware acceleration:

1. TouchGFX framework manages the scene graph and determines dirty regions
2. Widget draw calls generate primitive operations (fills, blits, copies)
3. DMA2D executes operations in hardware, freeing the CPU
4. LTDC continuously scans the front buffer to the display
5. Buffer swap occurs during vertical blanking to prevent tearing

Frame buffers reside in external SDRAM with double buffering. The animation storage buffer provides additional space for transition effects.

### Peripheral Drivers

**LTDC Driver** configures display timing, layer properties, and handles VSYNC interrupts for frame synchronization.

**DMA2D Driver** provides hardware-accelerated graphics operations with support for multiple pixel formats and alpha blending modes.

**I2C Driver** manages communication with the touch controller using HAL blocking transfers.

**UART Driver** transmits sound effect commands using HAL blocking mode for simplicity.

**FMC Driver** initializes and manages the external SDRAM interface with appropriate timing parameters.

## Usage

### Controls

Touch the screen in different regions to control the game:

| Touch Region | Action |
|--------------|--------|
| Top | Rotate piece clockwise |
| Left | Move piece left |
| Right | Move piece right |
| Bottom | Hard drop (instant fall) |

### Gameplay

1. Power on the board to display the Welcome screen
2. Touch "START" to begin the game
3. Guide falling tetrominoes to complete horizontal lines
4. Completed lines are cleared and award points
5. Game ends when blocks stack above the play area