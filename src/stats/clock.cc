#include "stats\clock.h"

using namespace Stats;

Clock::Clock()
{
  restart();
}

Clock::~Clock() = default;

Clock::Clock(const Clock& o) = default;


void Clock::restart()
{
  start_time_ = std::chrono::high_resolution_clock::now();
  stop_time_ = std::chrono::high_resolution_clock::now();
}

void Clock::stop()
{
  stop_time_ = std::chrono::high_resolution_clock::now();
}

s64 Clock::elapsed_time_us() const
{
  return std::chrono::duration_cast<std::chrono::microseconds>(stop_time_ - start_time_).count();
}

s64 Clock::actual_time_us() const
{
  return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_time_).count();
}

s64 Clock::elapsed_time_ms() const
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(stop_time_ - start_time_).count();
}

s64 Clock::actual_time_ms() const
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count();
}