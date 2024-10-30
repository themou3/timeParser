#pragma once

#include <chrono>

class Timer 
{
public:
	void Start()
	{
		if (isRunning) return;

		isRunning = true;
		startTime = std::chrono::system_clock::now();
	}

	double Stop()
	{
		if (!isRunning)
			return 0.0;

		isRunning = false;
		std::chrono::system_clock::time_point finish = std::chrono::system_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(finish - startTime).count();
	}
private:
	bool isRunning = false;
	std::chrono::system_clock::time_point startTime;
};
