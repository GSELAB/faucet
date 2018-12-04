#pragma once

#include <string>
#include <thread>
#include <sstream>
#include <iosfwd>
#include <sys/time.h>

namespace utils {

#define MAX_PATH_LENGTH (256)

int MKDIR(std::string const& path);

std::string pathcat(std::string const& path, std::string const& file);

int64_t currentTimestamp();

int64_t currentMillisecond();

int64_t currentMicrosecond();

inline void sleepMilliseconds(int64_t milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
} // end namespace
