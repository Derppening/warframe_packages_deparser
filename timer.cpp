// Copyright (c) 2017 David Mak. All rights reserved.
// Licensed under MIT.
//
// Implementations for Timer class.
//

#include "timer.h"

#include <chrono>

void Timer::Start() {
  start_time_ = std::chrono::steady_clock::now();
}

void Timer::Stop() {
  end_time_ = std::chrono::steady_clock::now();
  last_time_ = std::chrono::duration<double, std::nano>(end_time_ - start_time_);
  Reset();
}

void Timer::Reset() {
  start_time_ = std::chrono::time_point<std::chrono::steady_clock>();
  end_time_ = std::chrono::time_point<std::chrono::steady_clock>();
}

auto Timer::GetElapsedTime() -> double {
  return GetRawElapsedTime().count();
}

auto Timer::GetRawElapsedTime() -> std::chrono::duration<double, std::nano> {
  auto current_time = std::chrono::steady_clock::now();
  return std::chrono::duration<double, std::nano>(current_time - start_time_);
}
