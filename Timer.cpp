#include "Timer.h"

Timer::Timer()
{
    lastFrame = steady_clock::now();
}

float Timer::GetDeltaTime()
{
    duration<float> frameTime = steady_clock::now() - lastFrame;
    return frameTime.count();
}

void Timer::Tick()
{
    lastFrame = steady_clock::now();
}
