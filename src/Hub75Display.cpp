#include "Hub75Display.hpp"
#include "hardware/gpio.h"
#include "pico/multicore.h"
#include "hardware/irq.h"
#include <algorithm> // For std::fill, std::swap

// ----------------------------------------------------------------------------
// HARDWARE CONFIGURATION
// ----------------------------------------------------------------------------
namespace Hub75Config 
{
    // Color Data Pins (Top Half)
    constexpr uint P_R1 = 2;
    constexpr uint P_G1 = 1; 
    constexpr uint P_B1 = 4;

    // Color Data Pins (Bottom Half)
    constexpr uint P_R2 = 5;
    constexpr uint P_G2 = 8;
    constexpr uint P_B2 = 9;

    // Control Signals
    constexpr uint P_CLK = 11; // Clock (Shift)
    constexpr uint P_LAT = 12; // Latch (Show)
    constexpr uint P_OE  = 13; // Output Enable (Light Switch)

    // Row Address Pins (Selecting Y Coordinate)
    constexpr uint P_ADDR_A = 10; // Bit 0 (1)
    constexpr uint P_ADDR_B = 16; // Bit 1 (2)
    constexpr uint P_ADDR_C = 18; // Bit 2 (4)
    constexpr uint P_ADDR_D = 20; // Bit 3 (8)
    constexpr uint P_ADDR_E = 22; // Bit 4 (16)
}

static Hub75Display* GlobalHub75Ptr = nullptr;

Hub75Display::Hub75Display(uint displayWidth, uint displayHeight) 
    : DisplayWidth(displayWidth), DisplayHeight(displayHeight) 
{
    Buffer1.resize(displayWidth * displayHeight, 0);
    Buffer2.resize(displayWidth * displayHeight, 0);
    FrontBuffer = Buffer1.data();
    BackBuffer = Buffer2.data();
    SwapRequested = false;
    mutex_init(&SwapMutex);
}

void Hub75Display::SetPixel(uint positionX, uint positionY, uint8_t redValue, uint8_t greenValue, uint8_t blueValue) 
{
    if (positionX >= DisplayWidth || positionY >= DisplayHeight) return;
    
    // Pack 3 bytes into one 32-bit integer for easier storage
    // Format: 00000000 BBBBBBBB GGGGGGGG RRRRRRRR
    uint32_t pixel = (static_cast<uint32_t>(blueValue) << 16) | 
                     (static_cast<uint32_t>(greenValue) << 8) | 
                      static_cast<uint32_t>(redValue);
    BackBuffer[positionY * DisplayWidth + positionX] = pixel;
}

void Hub75Display::Clear() 
{
    std::fill(BackBuffer, BackBuffer + (DisplayWidth * DisplayHeight), 0);
}

void Hub75Display::Flip() 
{
    mutex_enter_blocking(&SwapMutex);
    SwapRequested = true;
    mutex_exit(&SwapMutex);
}

void Hub75Display::Core1Trampoline() 
{
    if (GlobalHub75Ptr) GlobalHub75Ptr->Update();
}

void Hub75Display::Start() 
{
    GlobalHub75Ptr = this;
    sleep_ms(10);
    multicore_reset_core1();
    multicore_launch_core1(Core1Trampoline);
}

void Hub75Display::DebugPinSweep(int pinNumber) { }

// Helper to check if a specific color channel is active (> 0)
// Using templates to allow the compiler to inline this efficiently
inline bool IsChannelActive(uint32_t pixelData, uint32_t mask)
{
    return (pixelData & mask) != 0;
}

void Hub75Display::Update() 
{
    using namespace Hub75Config;

    // 1. Initialize all GPIO pins
    uint pins[] = {
        P_R1, P_G1, P_B1, 
        P_R2, P_G2, P_B2, 
        P_ADDR_A, P_ADDR_B, P_ADDR_C, P_ADDR_D, P_ADDR_E, 
        P_CLK, P_LAT, P_OE
    };

    for(uint p : pins) 
    {
        gpio_init(p); 
        gpio_set_dir(p, GPIO_OUT); 
        gpio_put(p, 0);
    }
    
    // Start with screen OFF (OE=1) so we don't flash garbage during startup
    gpio_put(P_OE, 1); 

    while (true) 
    {
        // Check if the Game Loop (Core 0) has finished a new frame
        if (SwapRequested) 
        {
            mutex_enter_blocking(&SwapMutex);
            if (SwapRequested) 
            {
                 // Atomic-like swap of the buffer pointers using modern C++
                 std::swap(FrontBuffer, BackBuffer);
                 SwapRequested = false;
            }
            mutex_exit(&SwapMutex);
        }

        // Render Loop
        // 1/32 Scan: Updates two rows at once (Row N and Row N+32)
        for (int row = 0; row < 32; ++row) 
        {
             // Shift out pixel data
             for (int x = 0; x < DisplayWidth; ++x) 
             {
                uint32_t topPixel = FrontBuffer[row * DisplayWidth + x];
                uint32_t botPixel = FrontBuffer[(row + 32) * DisplayWidth + x];
                
                // Color Data
                gpio_put(P_R1, IsChannelActive(topPixel, 0xFF));
                gpio_put(P_G1, IsChannelActive(topPixel, 0xFF00));
                gpio_put(P_B1, IsChannelActive(topPixel, 0xFF0000));
                
                gpio_put(P_R2, IsChannelActive(botPixel, 0xFF));
                gpio_put(P_G2, IsChannelActive(botPixel, 0xFF00));
                gpio_put(P_B2, IsChannelActive(botPixel, 0xFF0000));
                
                // Clock Pulse
                asm volatile("nop"); 
                gpio_put(P_CLK, 1); 
                asm volatile("nop"); 
                gpio_put(P_CLK, 0);
            }
            
            // Latch Row
            gpio_put(P_OE, 1); // Disable Output

            // Set Address
            gpio_put(P_ADDR_A, (row & 1));
            gpio_put(P_ADDR_B, (row >> 1) & 1);
            gpio_put(P_ADDR_C, (row >> 2) & 1);
            gpio_put(P_ADDR_D, (row >> 3) & 1);
            gpio_put(P_ADDR_E, (row >> 4) & 1);
            
            sleep_us(4); 
            
            // Latch Pulse
            gpio_put(P_LAT, 1);
            sleep_us(2); 
            gpio_put(P_LAT, 0);
            sleep_us(2); 
            
            gpio_put(P_OE, 0); // Enable Output
            sleep_us(100); 
        }
    }
}
