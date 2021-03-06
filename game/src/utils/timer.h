#pragma once

#include <chrono>

namespace timer {
	unsigned int start();
	std::chrono::nanoseconds lap(unsigned int handle);
	std::chrono::nanoseconds reset(unsigned int handle);
	std::chrono::nanoseconds average(unsigned int handle);
	std::chrono::nanoseconds total(unsigned int handle);
}
