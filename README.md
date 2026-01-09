# Raspberry Pi Pico HUB75 Display Driver

## Pico Bouncing Balls & Snow Demo

## Project Context
This is a personal project created as a Christmas gift. It drives a 64x64 RGB LED Matrix to display a physics simulation of bouncing balls and fallen snow, designed to look like a "Cosmic Sunset".

## Hardware Details
**Panel**: Xicoolee RGB Full-Color LED Matrix Panel P3.0-64x64 (Amazon)
*   **Driver**: HUB75
*   **Scan Rate**: 1/32 Scan
*   **Power**: 5V/4A

### Manufacturing Quirks (The "Cosmic Sunset" Fix)
The specific panel used for this project has two hardware defects:
1.  **Top Half**: Dead Green Channel.
2.  **Bottom Half**: Blue Channel shorts to Green.

**The Fix**: The code implements a "Split Personality" rendering engine to hide these defects:
*   **Top**: Uses Red + Blue (Magenta).
*   **Bottom**: Uses Red + Green (Yellow/Orange), disabling Blue.
This creates a consistent "Sunset" aesthetic.

*   `src/`: Main application code.
*   `include/`: Header files.

## Getting Started

## Build Instructions

1.  **Configure SDK**: `export PICO_SDK_PATH=/path/to/pico-sdk`
2.  **Generate**: `mkdir build && cd build && cmake ..`
3.  **Compile**: `make -j4`
4.  **Flash**: Copy `pico_bouncing_balls.uf2` to the Pico.

## Pin Mapping (Reversed Engineered)
| GPIO | Signal | Function |
|---|---|---|
| 2, 1, 4 | R1, G1, B1 | Top Color Data |
| 5, 8, 9 | R2, G2, B2 | Bot Color Data |
| 11 | CLK | Clock |
| 12 | LAT | Latch |
| 13 | OE | Output Enable |
| 10, 16, 18, 20, 22 | A-E | Address Lines |

*Note: G1 is dead, B2 is disabled in software.*
