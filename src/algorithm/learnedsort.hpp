/*******************************************************************************
 * Project Ips4o Benchmark Suite
 *
 * src/algorithm/learnedsort.hpp
 *
 * Learned sort.
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
#include <limits>
#include <string>

#include "../../extern/LearnedSort/include/learned_sort.h"

#include "../datatypes.hpp"
#include "../sequence.hpp"

namespace sequential {

class Learnedsort {
 public:
    Learnedsort() {}

    template <class T>
    static constexpr bool accepts() {
        return std::is_same_v<T, uint32_t>
                || std::is_same_v<T, uint64_t>
                || std::is_same_v<T, double>;
    }

    static bool isParallel() { return false; }

    static std::string name() { return "learnedsort"; }

    template <class T, template <class T1> class Vector>
    static std::pair<double, double> sort(T* begin, T* end, size_t num_threads) {
        auto start = std::chrono::high_resolution_clock::now();
        learned_sort::sort(begin, end);
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = finish - start;
        return {0, elapsed.count()};
    }
};

}  // namespace sequential
