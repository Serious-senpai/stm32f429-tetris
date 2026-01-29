# STM32 Tetris

A classic Tetris game implementation running on the STM32F429I-DISCO development board, featuring a touchscreen interface and external audio feedback.

## Team Members & Responsibilities

| Member | Responsibilities |
|--------|------------------|
| **Bui Quang Phong** | Hardware setup, BSP drivers (ILI9341, STMPE811), touch calibration |
| **Nguyen Thai Hoa** | Game logic (Model.cpp), collision detection, line clearing, scoring system |
| **Nguyen Viet Tuan Kiet** | TouchGFX UI (Views, Presenters), graphics assets, audio integration |

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
| SRAM | 192 KB |
| CCM RAM | 64 KB |
| Onboard SDRAM | 8 MB (IS42S16400J, mapped at 0xD0000000) |

### Display Subsystem

The ILI9341 LCD controller drives the onboard 2.4" TFT panel. The LTDC peripheral outputs frame data while DMA2D provides hardware-accelerated 2D graphics operations (blitting, filling, pixel format conversion).

| Parameter | Value |
|-----------|-------|
| Resolution | 240 × 320 pixels (Portrait) |
| Color Depth | 16-bit RGB565 |
| Frame Buffer | Double-buffered in onboard SDRAM |
| Animation Storage | Additional buffer for transitions |

From `TouchGFXGeneratedHAL.cpp`:
```cpp
uint32_t frameBuf[(240 * 320 * 2 + 3) / 4 * 2];  // Double buffer
uint32_t animationStorage[(240 * 320 * 2 + 3) / 4];  // Animation buffer
```

### Touch Input Subsystem

The STMPE811 touch controller provides resistive touchscreen input via I2C3. Raw coordinates undergo calibration with different correction factors depending on board revision (`isRevD` flag).

| Parameter | Value |
|-----------|-------|
| Controller | STMPE811 |
| I2C Address | 0x82 |
| Calibration | Board revision-dependent (RevD vs earlier) |

From `STM32TouchController.cpp`:
```cpp
#define TS_I2C_ADDRESS  0x82
BSP_TS_Init(240, 320);
```

### Audio Output Subsystem

Sound effects are transmitted as null-terminated ASCII strings over UART1 to an external host computer running a Python audio player.

| Parameter | Value |
|-----------|-------|
| Peripheral | UART1 (`huart1`) |
| Baud Rate | 115200 |
| Protocol | Null-terminated strings |
| Commands | `CONTROL`, `SCORE`, `GAMEOVER` |

From `sounds.c`:
```c
HAL_UART_Transmit(&huart1, (uint8_t *)payload, strlen(payload) + 1, HAL_MAX_DELAY);
```

### Memory Map

| Region | Address | Size | Usage |
|--------|---------|------|-------|
| Flash | 0x08000000 | 2 MB | Program code, constants |
| SRAM | 0x20000000 | 192 KB | Variables, heap, stack |
| CCM RAM | 0x10000000 | 64 KB | Fast access data |
| SDRAM | 0xD0000000 | 8 MB | TouchGFX frame buffers |

## Software Architecture

### Layer Overview

```
┌──────────────────────────────────────────────────┐
│              Application Layer                   │
│      (Model, Views, Presenters, Game Logic)      │
├──────────────────────────────────────────────────┤
│              TouchGFX Framework                  │
│       (Graphics Engine, Widget Library)          │
├──────────────────────────────────────────────────┤
│                  FreeRTOS                        │
│       (Task Scheduling, Message Queues)          │
├──────────────────────────────────────────────────┤
│            Hardware Abstraction                  │
│         (STM32 HAL, BSP Components)              │
├──────────────────────────────────────────────────┤
│                  Hardware                        │
│    (STM32F429, ILI9341, STMPE811, SDRAM)         │
└──────────────────────────────────────────────────┘
```

### TouchGFX MVP Pattern

**Model** (`Model.cpp`) contains game state and logic:
- Game board: `uint8_t _board[TETRIS_BOARD_WIDTH][TETRIS_BOARD_HEIGHT]` (10×18)
- Active piece tracking with `ActivePiece` class
- Score and high score management
- Collision detection and line clearing
- Tick-based game advancement

**View** (`GameView.cpp`, `WelcomeView.cpp`) handles visual representation:
- Widget management for score display, game board tiles
- Dynamic tile visibility based on board state
- Game over overlay display

**Presenter** (`GamePresenter.cpp`) bridges Model and View:
- Implements `ModelListener` interface
- Forwards `on_score_change`, `on_highscore_change`, `display_board`, `gameover` events
- Controls `ingame` state on screen transitions

From `FrontendApplication.hpp`:
```cpp
virtual void handleTickEvent()
{
    model.tick();
    FrontendApplicationBase::handleTickEvent();
}
```

### Task Structure

FreeRTOS manages concurrent execution:

| Task | Stack Size | Responsibility |
|------|------------|----------------|
| TouchGFX | 4096 bytes | Graphics rendering, UI events, game tick |
| Default | 512 bytes | System initialization |

Inter-task communication uses a FreeRTOS message queue (`channel`) to transmit button events to the game model:

From `Model.cpp`:
```cpp
while (osMessageQueueGet(channel, &message, NULL, 0) == osOK)
{
    if (message == MESSAGE_BUTTON_UP)
        _active.rotate(_board);
    else if (message == MESSAGE_BUTTON_RIGHT)
        _active.move(1, 0, _board);
    // ...
}
```

### Game Logic

**Board Representation**: 10×18 cell array (`TETRIS_BOARD_WIDTH` × `TETRIS_BOARD_HEIGHT`). Non-zero values indicate occupied cells. Game over triggers when any block occupies row 15 (`TETRIS_GAMEOVER_ROW`).

**Tetromino Encoding**: Each piece stored as 16-bit bitmap in 4×4 grid. Four rotation states precomputed for all 7 pieces:

From `Model.hpp`:
```cpp
inline static constexpr uint16_t _PIECES[7][4] = {
    {0xF000, 0x1111, 0xF000, 0x1111},  // I
    {0x3300, 0x3300, 0x3300, 0x3300},  // O
    {0x7200, 0x2620, 0x2700, 0x2320},  // T
    // ... S, Z, J, L
};
```

**Collision Detection**: `_is_valid_move()` checks proposed position against board boundaries and existing blocks, excluding current piece position to prevent self-collision.

**Line Clearing**: After piece placement, `_check_final_state()` scans for complete rows, removes them, shifts rows above downward, and increments score.

**Timing**: Pieces fall every 1000ms using tick counting:

From `Model.cpp`:
```cpp
static const uint32_t ticks_to_wait = osKernelGetTickFreq() * MILLISECONDS_BETWEEN_FALLS / 1000;
// MILLISECONDS_BETWEEN_FALLS = 1000
```

### Graphics Pipeline

1. `Model::tick()` updates game state
2. `ModelListener::display_board()` notifies presenter of changes
3. `GameView::display_board()` updates tile widget visibility
4. TouchGFX determines dirty regions and generates draw operations
5. DMA2D executes hardware-accelerated rendering
6. LTDC scans front buffer to display
7. Buffer swap occurs at VSYNC (`HAL_LTDC_LineEventCallback`)

### Display Layout

From `Tetris.hpp`:
```cpp
#define TETRIS_TILE_SIZE    18      // Pixels per cell
#define TETRIS_FIELD_LEFT   60      // Left margin (score panel width)
#define TETRIS_FIELD_BOTTOM 320     // Bottom of play field
```

Game field: 180×324 pixels (10×18 cells × 18px), offset 60px from left for score panel.

## Project Structure

```
├── Core/                      # STM32 application code
│   ├── Inc/                   # Headers (main.h, messages.h, sounds.h)
│   └── Src/                   # Sources (main.c, freertos.c, stm32f4xx_it.c)
├── Drivers/
│   ├── BSP/Components/        # ILI9341, STMPE811 drivers
│   ├── CMSIS/                 # ARM CMSIS headers
│   └── STM32F4xx_HAL_Driver/  # STM32 HAL library
├── Middlewares/
│   ├── ST/touchgfx/           # TouchGFX framework
│   └── Third_Party/FreeRTOS/  # FreeRTOS kernel
├── TouchGFX/
│   ├── App/                   # TouchGFX application entry
│   ├── assets/                # Images, fonts, texts
│   ├── generated/             # Auto-generated code
│   ├── gui/
│   │   ├── include/gui/
│   │   │   ├── common/        # FrontendApplication, FrontendHeap, Tetris.hpp
│   │   │   ├── model/         # Model.hpp, ModelListener.hpp
│   │   │   ├── game_screen/   # GameView.hpp, GamePresenter.hpp
│   │   │   └── welcome_screen/
│   │   └── src/               # Implementation files
│   └── target/                # HAL integration, touch controller
├── STM32CubeIDE/              # IDE project files, linker scripts
├── sounds/
│   ├── main.py                # Python audio player
│   ├── requirements.txt       # Python dependencies
│   └── wav/                   # Sound effect files
└── readme.md
```

## Development Environment

| Tool | Version |
|------|---------|
| STM32CubeIDE | 1.19.0 |
| TouchGFX Designer | 4.25.0 |

## Building and Flashing

1. Open `STM32CubeIDE/` as workspace
2. Import the project
3. Build Debug or Release configuration
4. Connect ST-LINK and flash to board

Or use TouchGFX Designer for direct flashing with GCC and STM32CubeProgrammer.

## Audio System Setup

### Hardware Connection

Connect USB-to-UART adapter:
- Board TX (PA9/USART1_TX) → Adapter RX
- Board GND → Adapter GND

### Software Setup

```bash
cd sounds
pip install -r requirements.txt
python main.py COM3  # Replace with your port
```

The script listens for commands and plays corresponding WAV files from `sounds/wav/`:
- `CONTROL` → `SFX_PieceMoveLR.wav` (piece movement)
- `SCORE` → `SFX_SpecialLineClearTriple.wav` (line cleared)
- `GAMEOVER` → `SFX_GameOver.wav` (game over)

## Usage

### Controls

Touch regions map to game actions (determined by touch Y-coordinate relative to active piece):

| Touch Region | Action |
|--------------|--------|
| Top | Rotate piece clockwise |
| Left | Move piece left |
| Right | Move piece right |
| Bottom | Hard drop (instant fall) |

### Gameplay

1. Power on → Welcome screen appears
2. Touch "START" → Game begins
3. Guide falling tetrominoes to complete rows
4. Completed rows clear and award points
5. Game ends when blocks reach row 15

## Contributing

1. Fork the repository
2. Create a feature branch
3. Regenerate TouchGFX code before committing
4. Format with VSCode C/C++ formatter
5. Submit pull request