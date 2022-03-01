#pragma once

#include <functional>
#include <thread>
#include <mutex>


namespace kl {
	namespace thread {
        // Multithreaded for loop
		void ParallelFor(int startInclusive, int endExclusive, std::function<void(int, int)> loopBody, int threadCount = 4);
	}
}
