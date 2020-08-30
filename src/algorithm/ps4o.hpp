/*******************************************************************************
 * Project Ips4o Benchmark Suite
 *
 * src/algorithm/ps4o.hpp
 *
 * Super-Scalar-Sample-Sort.
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
#include <string>

#include <ps4o.hpp>

#include "../datatypes.hpp"
#include "../sequence.hpp"

namespace sequential {

class Ps4o {
 public:
    Ps4o() {}

    template <class T>
    static constexpr bool accepts() {
        return true;
    }

    static bool isParallel() { return false; }

    static std::string name() { return "ps4o"; }

    template <class T, template <class T1> class Vector>
    static std::pair<double, double> sort(T* begin, T* end, size_t num_threads) {
        auto start = std::chrono::high_resolution_clock::now();
        ps4o::sort(begin, end, Datatype<T>::getComparator());
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = finish - start;
        return {0, elapsed.count()};
    }
};

}  // namespace sequential
