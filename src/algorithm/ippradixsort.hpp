/*******************************************************************************
 * Project Ips4o Benchmark Suite
 *
 * src/algorithm/ippradixsort.hpp
 *
 * Parallel radix sort from Intel's IPP.
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
#include <limits>
#include <chrono>

#include <ipps.h>
#include <ippcore.h>

#include "../datatypes.hpp"
#include "../sequence.hpp"

namespace sequential {

class Ippradixsort {
public:
  Ippradixsort() {
  }

  template<class T>
  static constexpr bool accepts() {
    return std::is_same_v<T, uint32_t>
      ||std::is_same_v<T, uint64_t>
      ||std::is_same_v<T, double>;
  }

  static bool isParallel() {
    return false;
  }

  static std::string name() {
    return "ippradixsort";
  }

  template<class T, template<class T1> class Vector>
  static std::pair<double, double> sort(T* begin, T* end, size_t num_threads) {
    
    auto start = std::chrono::high_resolution_clock::now();

    if constexpr (std::is_same_v<T, uint32_t>) {
      if (end - begin <= std::numeric_limits<int>::max()) {
      IppDataType type = ipp32u;
      int bufsize;
      ippsSortRadixGetBufferSize(end - begin, type, &bufsize);
      auto buffer = ippsMalloc_8u(bufsize);
  
      // ippsSortAscend_32u_I(ptr, end - begin);
      ippsSortRadixAscend_32u_I(begin, end - begin, buffer);
  
      ippsFree(buffer);
      } else {
	if (end - begin > 1) {
	  // Mark array as unsorted (including zero distribution).
	  *begin = 2;
	}
      }
      }

    else if constexpr (std::is_same_v<T, uint64_t> && sizeof(uint64_t) == sizeof(long long unsigned int)) {
      IppDataType type = ipp64u;
      IppSizeL bufsize;
      ippsSortRadixGetBufferSize_L(end - begin, type, &bufsize);
      auto buffer = ippsMalloc_8u_L(bufsize);
  
      // ippsSortAscend_64u_I(ptr, end - begin);
      ippsSortRadixAscend_64u_I_L(static_cast<long long unsigned int*>(static_cast<void*>(begin)), end - begin, buffer);
  
      ippsFree(buffer);
      }

    else if constexpr (std::is_same_v<T, double>) {
      IppDataType type = ipp64f;
      IppSizeL bufsize;
      ippsSortRadixGetBufferSize_L(end - begin, type, &bufsize);
      auto buffer = ippsMalloc_8u_L(bufsize);
  
      // ippsSortAscend_64f_I(ptr, end - begin);
      ippsSortRadixAscend_64f_I_L(begin, end - begin, buffer);
  
      ippsFree(buffer);
      }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = finish - start;
    return {0, elapsed.count()};
  }

protected:
};

} // namespace sequential

