// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// Utilities for timekeeping.
//

#ifndef WARFRAME_PACKAGES_DEPARSER_TIMER_H_
#define WARFRAME_PACKAGES_DEPARSER_TIMER_H_

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
   * Gets the time since the last Start has been called.
   *
   * @return Time in nanoseconds
   */
  auto GetElapsedTime() -> double;
  /**
   * Gets the time since the last Start has been called.
   *
   * @return Time in nanoseconds
   */
  auto GetRawElapsedTime() -> std::chrono::duration<double, std::nano>;

  /**
   * Gets the time recorded from last full Start->Stop cycle.
   *
   * @return Time in nanoseconds
   */
  auto GetTime() -> double { return GetRawTime().count(); }
  /**
   * Gets the time recorded from last full Start->Stop cycle.
   *
   * @return Time as a @c std::chrono::duration object
   */
  auto GetRawTime() -> std::chrono::duration<double, std::nano> { return last_time_; }

 private:
  std::chrono::time_point<std::chrono::steady_clock> start_time_ = std::chrono::time_point<std::chrono::steady_clock>();
  std::chrono::time_point<std::chrono::steady_clock> end_time_ = std::chrono::time_point<std::chrono::steady_clock>();

  std::chrono::duration<double, std::nano> last_time_ = std::chrono::duration<double, std::nano>::zero();
};

#endif  // WARFRAME_PACKAGES_DEPARSER_TIMER_H_
