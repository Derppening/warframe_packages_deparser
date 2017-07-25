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
