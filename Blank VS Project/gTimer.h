#ifndef GTIMER_H
#define GTIMER_H

#include "Includes.h"

class gTimer
{
    public:
		gTimer();

		void start();
		void stop();
		void reset();

		Uint32 getTicks();

    private:
		Uint32 ticks;
		bool started;
};

class TimeClock
{
public:
	static Uint8 maxAlpha;
	static int maxDay;
	static int maxNight;
	static double alpha;
	static int hour;
	static double minute;
	static double speed;
	static void tick();
	static string display();
};

#endif // GTIMER_H
