#include "gTimer.h"

gTimer::gTimer()
{
    ticks = 0;
    started = false;
}

void gTimer::start()
{
    started = true;
    ticks = SDL_GetTicks();
}

void gTimer::stop()
{
    started = false;
    ticks = 0;
}

void gTimer::reset()
{
	ticks = SDL_GetTicks();
}

Uint32 gTimer::getTicks()
{
    Uint32 time = 0;

    if(started == true)
        time = SDL_GetTicks() - ticks;

    return time;
}
