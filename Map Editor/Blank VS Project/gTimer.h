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

#endif // GTIMER_H
