#include "Timer.h"

Timer::Timer()
{
	StartTiming();
}

void Timer::StartTiming()
{
	startTime = std::chrono::high_resolution_clock::now();
}

double Timer::GetElapsedTimeInSeconds()
{
	endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> timeSpan =
		duration_cast<std::chrono::duration<double>>(endTime - startTime);
	startTime = std::chrono::high_resolution_clock::now();
	return timeSpan.count();
}
