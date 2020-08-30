/*******************************************************************************
 * Project Ips4o Benchmark Suite
 *
 * src/algorithm/ips2ra.hpp
 *
 * Sequential radix sort integrated into the ips4o framework.
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

#include <ips2ra.hpp>

#include "../datatypes.hpp"
#include "../sequence.hpp"

namespace sequential {

class Ips3ro {
 public:
    Ips3ro() {}

    template <class T>
    static constexpr bool accepts() {
        return Datatype<T>::hasUnsignedKey();
    }

    static bool isParallel() { return false; }

    static std::string name() { return "ips2ra"; }

    template <class T, template <class T1> class Vector>
    static std::pair<double, double> sort(T* begin, T* end, size_t num_threads) {
        static_assert(Datatype<T>::hasUnsignedKey());

        auto start = std::chrono::high_resolution_clock::now();
        ips2ra::sort(begin, end, Datatype<T>::getKeyExtractor());
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = finish - start;
        return {0, elapsed.count()};
    }
};

}  // namespace sequential
