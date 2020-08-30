/*******************************************************************************
 * Project Ips4o Benchmark Suite
 *
 * src/algorithm/inplace_msd_radixsort.hpp
 *
 * Parallel inplace radixsort.
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

#include <string>
#include <numa.h>

extern "C" {
#include "msb_64.h"
}

#include "../datatypes.hpp"
#include "../sequence.hpp"

namespace pthread {

class Inplacemsdradixsort {
  
public:
  Inplacemsdradixsort() {
  }

  template<class T>
  static constexpr bool accepts() {
    return std::is_same_v<T, pair_t>;
  }

  static bool isParallel() {
    return true;
  }

  static std::string name() {
    return "inplacemsdradixsort";
  }

  template<class T>
  static void init(typename T::int_type **keys, typename T::int_type **rids, uint64_t *size, uint64_t *cap,
		   T *begin, T *end, int numa) {

    std::thread t[numa];

    std::vector<std::thread> thread;
    for (int i = 0; i != numa; ++i) {
      auto copy_op = [keys, rids, size, cap, begin, end, numa, i] () {
		       numa_available();
		       numa_run_on_node(i);
		       numa_set_preferred(i);
		       keys[i] = (typename T::int_type*)mamalloc(cap[i] * sizeof(typename T::int_type));
		       rids[i] = (typename T::int_type*)mamalloc(cap[i] * sizeof(typename T::int_type));

		       uint64_t offset = 0;
		       for (int j = 0; j < i; ++j) {
			 offset += size[j];
		       }
		       for (uint64_t j = 0; j != size[i]; ++j) {
			 keys[i][j] = begin[offset + j].k;
			 rids[i][j] = begin[offset + j].v;
		       }
		     };
      thread.emplace_back(copy_op);
    }
    for (int i = 0; i != numa; ++i) {
      thread[i].join();
    }
  }

  template<class T>
  static void deinit(typename T::int_type **keys, typename T::int_type **rids, uint64_t *size, uint64_t *cap,
		   T *begin, T *end, int numa) {

    std::thread t[numa];

    std::vector<std::thread> thread;
    for (int i = 0; i != numa; ++i) {
      auto copy_op = [keys, rids, size, cap, begin, end, numa, i] () {
		       numa_available();
		       numa_run_on_node(i);
		       numa_set_preferred(i);

		       uint64_t offset = 0;
		       for (int j = 0; j < i; ++j) {
			 offset += size[j];
		       }
		       for (uint64_t j = 0; j != size[i]; ++j) {
			 begin[offset + j].k = keys[i][j];
			 begin[offset + j].v = rids[i][j];
		       }

		       free(keys[i]);
		       free(rids[i]);

		     };
      thread.emplace_back(copy_op);
    }
    for (int i = 0; i != numa; ++i) {
      thread[i].join();
    }
  }

  template<class T, template<class T1> class Vector>
  static std::pair<double, double> sort(T* begin, T* end, size_t num_threads) {

    if (num_threads != 64 || end - begin < (1ul << 26)) {
      ++begin[0].k;
      return {0., 0.};
    }
    
    uint64_t times[12];
    char *desc[12];
    double fudge = 1.2;
    numa_available();
    int numa = numa_max_node() + 1;
    int threads = 64;
    typename T::int_type	*keys[numa], *rids[numa];
    uint64_t	 size[numa], cap[numa];
    uint64_t tuples = end - begin;

    uint64_t tuples_per_numa = tuples / numa;
    uint64_t capacity_per_numa = tuples_per_numa * fudge;
    
    for (int i = 0 ; i != numa ; ++i) {
      size[i] = tuples_per_numa;
      cap[i] = size[i] * fudge;
    }

    init<T>(keys, rids, size, cap, begin, end, numa);    
    
    auto start = std::chrono::high_resolution_clock::now();
    ::sort(keys, rids, size, threads, numa, fudge, desc, times);
    
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = finish - start;

    deinit<T>(keys, rids, size, cap, begin, end, numa);

    return {0., elapsed.count()};
  }

protected:
};

} // namespace pthread
