/**
 * main.cpp
 * Entry point for the Pico Bouncing Balls Project.
 * Responsible for System Logic, Driver Init, and handing off to Demo Logic.
 */

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "Hub75Display.hpp"
#include "BouncingBallsDemo.hpp"

// Initialize the Hub75 Driver with 64x64 resolution
// We keep this global/static so it fits easily with C-style ISRs/Core1 launch if needed later
Hub75Display Display(64, 64);

int main() 
{
    // 1. Initialize Pico System
    stdio_init_all();
    if (cyw43_arch_init()) return -1; 

    // 2. Start the Display Driver (Core 1)
    Display.Start();
    
    // 3. Initialize the Demo Game Logic
    BouncingBallsDemo Demo(Display);
    Demo.Setup();

    // 4. Run the Game Loop (Core 0)
    while (true) 
    {
        Demo.Update();
    }
}
