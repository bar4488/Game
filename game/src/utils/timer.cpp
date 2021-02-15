#include "timer.h"
#include <unordered_map>

namespace timer {

	std::unordered_map<unsigned int, std::chrono::steady_clock::time_point> timers;
	unsigned int highest = 0;

	unsigned int start()
	{
		std::chrono::steady_clock::time_point t = std::chrono::high_resolution_clock::now();
		timers[highest] = t;
		return highest++;
	}

	std::chrono::nanoseconds lap(unsigned int handle)
	{
		return std::chrono::high_resolution_clock::now() - timers[handle];
	}

	std::chrono::nanoseconds reset(unsigned int handle)
	{
		auto t = timers[handle];
		timers[handle] = std::chrono::high_resolution_clock::now();
		return timers[handle] - t;
	}
}
