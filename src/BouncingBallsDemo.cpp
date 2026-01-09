#include "BouncingBallsDemo.hpp"
#include <cmath>
#include <cstdlib> // for rand()

BouncingBallsDemo::BouncingBallsDemo(Hub75Display& display) 
    : Display(display)
{
}

void BouncingBallsDemo::Setup() 
{
    SnowFlakes.clear();
    BouncingBalls.clear();
    
    // Create snowflakes
    for(int i = 0; i < DemoConfig::SNOW_COUNT; i++) 
    {
        SnowFlakes.push_back({
            static_cast<float>(rand() % DemoConfig::SCREEN_SIZE),
            static_cast<float>(rand() % DemoConfig::SCREEN_SIZE),
            0.1f + ((rand() % 10) / 30.0f)
        });
    }
    
    // Create 3 balls
    BouncingBalls.push_back({15.0f, 20.0f, 0.6f, 0.4f, DemoConfig::BALL_RADIUS}); 
    BouncingBalls.push_back({32.0f, 50.0f, -0.4f, -0.6f, DemoConfig::BALL_RADIUS}); 
    BouncingBalls.push_back({48.0f, 30.0f, 0.3f, -0.2f, DemoConfig::BALL_RADIUS}); 
}

void BouncingBallsDemo::GetStableColour(int positionY, uint8_t brightness, uint8_t& redValue, uint8_t& greenValue, uint8_t& blueValue) 
{
    if (positionY < DemoConfig::HALF_SCREEN) 
    {
        // Top Half: Green dead -> Magenta (Red + Blue)
        redValue = brightness;
        greenValue = 0;
        blueValue = brightness;
    } 
    else 
    {
        // Bottom Half: Blue shorts to Green -> Red Only (Safe)
        redValue = brightness;
        greenValue = 0;
        blueValue = 0;
    }
}

void BouncingBallsDemo::Update()
{
    Display.Clear(); 
    
    UpdateSnow();
    UpdateBalls();
    DrawBorder();
    
    Display.Flip(); 
    sleep_ms(DemoConfig::FRAME_DELAY_MS);
}

void BouncingBallsDemo::UpdateSnow()
{
    for(auto& flake : SnowFlakes) 
    {
        flake.PositionY += flake.FallSpeed;
        
        if(flake.PositionY > DemoConfig::SCREEN_MAX_INDEX) 
        { 
            flake.PositionY = 0; 
            flake.PositionX = static_cast<float>(rand() % DemoConfig::SCREEN_SIZE); 
        }
        
        uint8_t r, g, b;
        GetStableColour(static_cast<int>(flake.PositionY), 100, r, g, b); 
        Display.SetPixel(static_cast<int>(flake.PositionX), static_cast<int>(flake.PositionY), r, g, b); 
    }
}

// ------------------------------------------------------------------------
// PHYSICS ENGINE EQUATIONS
// ------------------------------------------------------------------------
// 1. Euler Integration (Movement)
//    NewPosition = CurrentPosition + Velocity
//
// 2. Collision Detection (Circle-Circle)
//    Distance² = (x2-x1)² + (y2-y1)²
//    Collision if: Distance < (Radius1 + Radius2)
//
// 3. Elastic Collision Response (Equal Mass)
//    We project 2D velocity onto the collision "Normal" vector.
//    Conservation of Momentum for Equal Mass (m1=m2):
//      v1_normal_final = v2_normal_initial
//      v2_normal_final = v1_normal_initial
//    (Velocities simply swap along the axis of impact)
// ------------------------------------------------------------------------
void BouncingBallsDemo::UpdateBalls()
{
    // 1. Move & Wall Bounce
    for (auto& ball : BouncingBalls) 
    {
        ball.PositionX += ball.VelocityX; 
        ball.PositionY += ball.VelocityY;
        
        float minB = ball.Radius + 2;
        float maxB = (DemoConfig::SCREEN_SIZE - 3) - ball.Radius;

        if (ball.PositionX < minB) { ball.PositionX = minB; ball.VelocityX = -ball.VelocityX; }
        if (ball.PositionX > maxB) { ball.PositionX = maxB; ball.VelocityX = -ball.VelocityX; }
        if (ball.PositionY < minB) { ball.PositionY = minB; ball.VelocityY = -ball.VelocityY; }
        if (ball.PositionY > maxB) { ball.PositionY = maxB; ball.VelocityY = -ball.VelocityY; }
        
        // Draw Ball
        int dX = static_cast<int>(ball.PositionX);
        int dY = static_cast<int>(ball.PositionY);
        int dR = static_cast<int>(ball.Radius);
         
        for(int y = dY - dR; y <= dY + dR; y++) 
        {
            for(int x = dX - dR; x <= dX + dR; x++) 
            {
                if ((x - dX) * (x - dX) + (y - dY) * (y - dY) <= dR * dR) 
                {
                    uint8_t r, g, b;
                    GetStableColour(y, 255, r, g, b);
                    Display.SetPixel(x, y, r, g, b);
                }
            }
        }
    }
    
    // 2. Ball vs Ball Collision
    for (size_t i = 0; i < BouncingBalls.size(); ++i) 
    {
        for (size_t j = i + 1; j < BouncingBalls.size(); ++j) 
        {
            float dx = BouncingBalls[i].PositionX - BouncingBalls[j].PositionX;
            float dy = BouncingBalls[i].PositionY - BouncingBalls[j].PositionY;
            float distSq = dx*dx + dy*dy;
            float minDist = BouncingBalls[i].Radius + BouncingBalls[j].Radius;
            
            if (distSq < minDist * minDist) 
            {
                float dist = sqrt(distSq);
                if (dist < 0.1f) dist = 0.1f;
                
                float normalX = dx / dist; 
                float normalY = dy / dist;
                float overlap = 0.5f * (minDist - dist);
                
                // Separate the Balls
                BouncingBalls[i].PositionX += overlap * normalX; 
                BouncingBalls[i].PositionY += overlap * normalY;
                BouncingBalls[j].PositionX -= overlap * normalX; 
                BouncingBalls[j].PositionY -= overlap * normalY;
                
                // Calculate Vector Components
                // Tangent vector is perpendicular to Normal (-y, x)
                float tangentX = -normalY;
                float tangentY = normalX;
                
                // Project velocities onto the Normal and Tangent vectors
                // (Dot Product: v . n)
                float velocity1Normal = BouncingBalls[i].VelocityX * normalX + BouncingBalls[i].VelocityY * normalY;
                float velocity1Tangent = BouncingBalls[i].VelocityX * tangentX + BouncingBalls[i].VelocityY * tangentY;
                
                float velocity2Normal = BouncingBalls[j].VelocityX * normalX + BouncingBalls[j].VelocityY * normalY;
                float velocity2Tangent = BouncingBalls[j].VelocityX * tangentX + BouncingBalls[j].VelocityY * tangentY;
                
                // Elastic Collision Reaction
                // Since mass is equal, the normal components simply swap
                float velocity1NormalAfter = velocity2Normal;
                float velocity2NormalAfter = velocity1Normal;
                
                // Reconstruct the new velocity vectors
                // NewVelocity = (NewNormalComponent * NormalVector) + (OriginalTangentComponent * TangentVector)
                BouncingBalls[i].VelocityX = velocity1NormalAfter * normalX + velocity1Tangent * tangentX; 
                BouncingBalls[i].VelocityY = velocity1NormalAfter * normalY + velocity1Tangent * tangentY;
                
                BouncingBalls[j].VelocityX = velocity2NormalAfter * normalX + velocity2Tangent * tangentX; 
                BouncingBalls[j].VelocityY = velocity2NormalAfter * normalY + velocity2Tangent * tangentY;
            }
        }
    }
}

void BouncingBallsDemo::DrawBorder()
{
    for(int i = 0; i < DemoConfig::SCREEN_SIZE; i++) 
    {
        uint8_t r, g, b;
        GetStableColour(0, 150, r, g, b); 
        Display.SetPixel(i, 0, r, g, b); Display.SetPixel(i, 1, r, g, b);
        
        GetStableColour(DemoConfig::SCREEN_MAX_INDEX, 150, r, g, b); 
        Display.SetPixel(i, 62, r, g, b); Display.SetPixel(i, DemoConfig::SCREEN_MAX_INDEX, r, g, b);
        
        GetStableColour(i, 150, r, g, b); 
        Display.SetPixel(0, i, r, g, b); Display.SetPixel(1, i, r, g, b);
        Display.SetPixel(62, i, r, g, b); Display.SetPixel(DemoConfig::SCREEN_MAX_INDEX, i, r, g, b);
    }
}
