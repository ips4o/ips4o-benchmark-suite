/*******************************************************************************
 * Project Ips4o Benchmark Suite
 *
 * src/timer.hpp
 *
 * Core of the benchmark.
 *
 * Copyright (C) 2020 Michael Axtmann <michael.axtmann@gmail.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see
 * <https://www.gnu.org/licenses/>.
 ******************************************************************************/

#pragma once

#include <chrono>

#ifdef IPS4O_TIMER

thread_local long g_active_counters;

class Timer {
public:
  Timer()
    : num_activations_(0)
  {}

  void start() {
    // std::cout << "start: " << g_active_counters << " " << num_activations_ << std::endl;
    if (g_active_counters <= 0 && num_activations_ == 0) start_ = Clock::now();
    ++g_active_counters;
    ++num_activations_;
  }

  void stop() {
    --g_active_counters;
    --num_activations_;
    // std::cout << "stop: " << g_active_counters << " " << num_activations_ << std::endl;
    
    if (g_active_counters <= 0 && num_activations_ == 0) {
      TimePoint end = Clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start_);
      time_ += elapsed.count();
    }
  }

  double getTime() const {
    return time_;
  }

  void reset() {
    time_ = 0.;
    num_activations_ = 0;
  }
  
private:
  using Clock = std::chrono::steady_clock;
  using TimePoint = std::chrono::time_point<Clock>;

  TimePoint start_;
  double time_;
  long num_activations_;
};

thread_local Timer g_base_case, g_sampling, g_classification, g_permutation,
  g_cleanup, g_overhead, g_empty_block, g_total;

#endif
