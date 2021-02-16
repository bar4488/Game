#include "timer.h"
#include <unordered_map>

namespace timer {
	using clock = std::chrono::high_resolution_clock;

	struct timer
	{
		unsigned int lap_count{};
		std::chrono::steady_clock::time_point last_lap;
		std::chrono::steady_clock::time_point start_time;

		static timer now()
		{
			const auto now = clock::now();
			return { 0, now, now };
		}
	};

	std::unordered_map<unsigned int, timer> timers;  // NOLINT(clang-diagnostic-exit-time-destructors)
	unsigned int highest = 0;

	unsigned int start()
	{
		timers[highest]  = timer::now();
		return highest++;
	}

	std::chrono::nanoseconds lap(const unsigned int handle)
	{
		timers[handle].lap_count++;
		const auto now = clock::now();
		const auto dt = now - timers[handle].last_lap;
		timers[handle].last_lap = now;
		return dt;
	}
	
	std::chrono::nanoseconds total(const unsigned int handle)
	{
		return clock::now() - timers[handle].start_time;
	}

	std::chrono::nanoseconds reset(const unsigned int handle)
	{
		const auto t = timers[handle].start_time;
		const auto now = clock::now();
		timers[handle] = { 0, now, now};
		return timers[handle].start_time - t;
	}

	std::chrono::nanoseconds average(const unsigned int handle)
	{
		return (clock::now() - timers[handle].start_time) / timers[handle].lap_count;
	}
}
