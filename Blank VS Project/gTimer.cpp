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

void TimeClock::tick()
{
	minute += speed;
	if (minute >= 60)
	{
		hour++;
		minute -= 60;
		if (hour >= 24)
		{
			hour -= 24;
		}
	}
	if (hour >= 4 && hour < 8)
	{
		alpha = maxNight - (((int)(minute)+(hour - 4) * 60) * ((double)maxNight / 240.0));
	}
	else if (hour >= 16 && hour < 20)
	{
		alpha = (((int)(minute)+(hour - 16) * 60) * ((double)maxNight / 240.0));
	}
	else if (hour >= 8 && hour < 16)
	{
		alpha = maxDay;
	}
	else if (hour >= 20 || hour < 4)
	{
		alpha = maxNight;
	}
	if (alpha >= maxDay && alpha <= maxNight)
	{
		maxAlpha = alpha;
	}
}
	
string TimeClock::display()
{
	string x = "0";
	string y = "0";
	string xm = "am";
	int hourx = hour;
	if (hourx > 12)
	{
		hourx -= 12;
		xm = "pm";
	}
	if (hourx == 12)
		xm = "pm";
	if (hourx == 0)
		hourx = 12;
	if (hourx >= 10)
		x = "";
	if (minute >= 10)
		y = "";
	return x + to_string(hourx) + ":" + y + to_string((int)minute) + xm;
}
