// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// Utilities for timekeeping.
//

#ifndef TESTING_GROUNDS_TIMER_H_
#define TESTING_GROUNDS_TIMER_H_

#include <chrono>

class Timer {
 public:
  using nanoseconds = std::chrono::duration<double, std::nano>;
  using microseconds = std::chrono::duration<double, std::micro>;
  using milliseconds = std::chrono::duration<double, std::milli>;
  using seconds = std::chrono::duration<double>;
  using minutes = std::chrono::duration<double, std::ratio<60>>;
  using hours = std::chrono::duration<double, std::ratio<3600>>;

  /**
   * Starts the timer.
   */
  void Start();
  /**
   * Stops the timer.
   */
  void Stop();
  /**
   * Resets the timer.
   */
  void Reset();

  /**
   * @return Time in nanoseconds
   */
  double GetTime() { return GetTimeRaw().count(); }
  /**
   * @return Time as a @c std::chrono::duration object
   */
  std::chrono::duration<double, std::nano> GetTimeRaw() { return last_time_; }

 private:
  std::chrono::time_point<std::chrono::steady_clock> start_time_ = std::chrono::steady_clock::now();
  std::chrono::time_point<std::chrono::steady_clock> end_time_ = std::chrono::steady_clock::now();

  std::chrono::duration<double, std::nano> last_time_ = std::chrono::duration<double, std::nano>::zero();
};

#endif  // TESTING_GROUNDS_TIMER_H_
