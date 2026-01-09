#pragma once

#include <vector>
#include <cstdint>
#include "Hub75Display.hpp"

// ----------------------------------------------------------------------------
// CONSTANTS
// ----------------------------------------------------------------------------
namespace DemoConfig
{
    constexpr int SCREEN_SIZE = 64;
    constexpr int SCREEN_MAX_INDEX = SCREEN_SIZE - 1;
    constexpr int HALF_SCREEN = 32;
    
    constexpr int SNOW_COUNT = 80;
    constexpr float BALL_RADIUS = 5.0f;
    constexpr int FRAME_DELAY_MS = 30; // ~33 FPS
}

// ----------------------------------------------------------------------------
// DATA STRUCTURES
// ----------------------------------------------------------------------------
struct SnowFlake 
{
    float PositionX, PositionY; 
    float FallSpeed;            
};

struct BouncingBall 
{
    float PositionX, PositionY; 
    float VelocityX, VelocityY; 
    float Radius;               
};

// ----------------------------------------------------------------------------
// DEMO CLASS
// ----------------------------------------------------------------------------
class BouncingBallsDemo
{
public:
    BouncingBallsDemo(Hub75Display& display);
    
    void Setup();
    void Update();

private:
    Hub75Display& Display; // Reference to the display driver
    
    std::vector<SnowFlake> SnowFlakes;
    std::vector<BouncingBall> BouncingBalls;

    void GetStableColour(int positionY, uint8_t brightness, uint8_t& redValue, uint8_t& greenValue, uint8_t& blueValue);
    void UpdateSnow();
    void UpdateBalls();
    void DrawBorder();
};
