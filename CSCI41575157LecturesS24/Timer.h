#pragma once
#include <ctime>
#include <ratio>
#include <chrono>

class Timer
{
protected:
	std::chrono::high_resolution_clock::time_point startTime, endTime;

public:
	Timer();
	void StartTiming();
	double GetElapsedTimeInSeconds();
};

