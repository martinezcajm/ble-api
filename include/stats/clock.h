#ifndef  __CLOCK_H__
#define __CLOCK_H__

#include <chrono>
#include "types.h"

namespace Stats {
  class Clock {
    /** @brief Engine_clock constructor
    *
    * Initializes a clock and sets the start time to the moment it was created
    *
    * @return *Engine_clock
    */
    Clock();

    /** @brief Engine_clock destructor
    *
    * Initializes a clock and sets the start time to the moment it was created
    *
    */
    ~Clock();

    /** @brief copy constructor of the clock
    *
    * Basic copy constructor operation
    *
    */
    Clock(const Clock& o);

    /** @brief Resets the clock
    *
    * Restarts the clock setting it's start time to the actual moment. Also resets
    * the time it was stoped
    *
    * @return *Engine_clock
    */
    void restart();

    /** @brief Stops the clock
    *
    * Sets the stop time of the clock to the actual moment. Elapsed time will return it's
    * duration in base to the stop time.
    *
    */
    void stop();
    /** @brief elapsed time from start to stop in us
    *
    * Returns the time passed from the last restart of the clock to the moment it was stoped in
    * micro seconds
    *
    * @return s64 time in micro seconds
    */
    s64 elapsed_time_us() const;
    /** @brief elapsed time from start to actual moment in us
    *
    * Returns the time passed from the last restart of the clock to the actual moment in
    * micro seconds
    *
    * @return s64 time in micro seconds
    */
    s64 actual_time_us() const;
    /** @brief elapsed time from start to stop in ms
    *
    * Returns the time passed from the last restart of the clock to the moment it was stoped in
    * milli seconds
    *
    * @return s64 time in milli seconds
    */
    s64 elapsed_time_ms() const;
    /** @brief elapsed time from start to actual moment in ms
    *
    * Returns the time passed from the last restart of the clock to the actual moment in
    * milli seconds
    *
    * @return s64 time in milli seconds
    */
    s64 actual_time_ms() const;
  private:
    std::chrono::high_resolution_clock::time_point start_time_;
    std::chrono::high_resolution_clock::time_point stop_time_;
  };
}
#endif // ! __CLOCK_H__
