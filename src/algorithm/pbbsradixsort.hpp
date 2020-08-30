/*******************************************************************************
 * Project Ips4o Benchmark Suite
 *
 * src/algorithm/pbbsradixsort.hpp
 *
 * Parallel radix sort form the PBBS benchmark suite.
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

#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

// Do not swap the order of these includes.
// PBBS defines macros for parallelism in two different places,
// and only one of them works correctly.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "../../extern/pbbs-sorting/integer_sort.h"
#pragma GCC diagnostic pop

#include "../datatypes.hpp"
#include "../sequence.hpp"

namespace cilk_algos {

class PbbsRadixsort {
  
public:
  PbbsRadixsort() {
  }

  template<class T>
  static constexpr bool accepts() {
    return Datatype<T>::hasUnsignedKey();
  }

  static bool isParallel() {
    return true;
  }

  static std::string name() {
    return "pbbsradixsort";
  }

  template<class T, template<class T1> class Vector>
  static std::pair<double, double> sort(T* begin, T* end, size_t num_threads) {
    sequence<T> s(begin, end);
    auto identity = Datatype<T>::getKeyExtractor();
    auto start = std::chrono::high_resolution_clock::now();
    pbbs::integer_sort<T>(s, s, identity, Datatype<T>::sizeofKey() * 8);
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = finish - start;
    return {0, elapsed.count()};
  }

protected:
};

} // namespace cilk_algos
