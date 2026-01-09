#pragma once

#include <vector>
#include <cstdint>
#include "pico/stdlib.h"
#include "pico/sync.h" // For thread safety

class Hub75Display
{
public:
    Hub75Display(uint displayWidth, uint displayHeight);

    void SetPixel(uint positionX, uint positionY, uint8_t redValue, uint8_t greenValue, uint8_t blueValue);
    void Clear();
    void Start();
    void Flip(); // Swap buffers
    void DebugPinSweep(int pinNumber);

    static void Core1Trampoline();
    void Update(); 

private:
    uint DisplayWidth;
    uint DisplayHeight;
    
    // Double Buffering
    std::vector<uint32_t> Buffer1;
    std::vector<uint32_t> Buffer2;
    
    // Pointers to active buffers
    uint32_t* FrontBuffer; // Read by Core 1
    uint32_t* BackBuffer;  // Written by Core 0
    
    bool SwapRequested;
    mutex_t SwapMutex;

    void Core1Entry(); 
};
