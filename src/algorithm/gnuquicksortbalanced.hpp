/*******************************************************************************
 * Project Ips4o Benchmark Suite
 *
 * src/algorithm/gnuquicksortbalanced.hpp
 *
 * Parallel balanced quicksort from the GNU Compiler Collection.
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

#include "gnuquicksortbalanced.hpp"

#include <string>
#include "omp.h"

#include <parallel/algorithm>

#include "../datatypes.hpp"
#include "../sequence.hpp"

namespace omp {

class GnuQuicksortBalanced {
public:
  GnuQuicksortBalanced() {
  }

  template<class T>
  static constexpr bool accepts() {
    return true;
  }

  static bool isParallel() {
    return true;
  }

  static std::string name() {
    return "gnuquicksortbalanced";
  }

  template<class T, template<class T1> class Vector>
  static std::pair<double, double> sort(T* begin, T* end, size_t num_threads) {
    const auto nested_default = omp_get_nested();
    omp_set_nested(true);
    auto start = std::chrono::high_resolution_clock::now();
    __gnu_parallel::sort(begin, end, Datatype<T>::getComparator(),
                         __gnu_parallel::balanced_quicksort_tag());
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = finish - start;
    return {0, elapsed.count()};
    omp_set_nested(nested_default);
  }

protected:
};
} // namespace omp

