/*******************************************************************************
 * Project Ips4o Benchmark Suite
 *
 * src/algorithm/raduls.hpp
 *
 * RADULS.
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

#include "../../extern/RADULS/Raduls/raduls.h"

#include "../datatypes.hpp"
#include "../parallel/parallel_for.hpp"
#include "../sequence.hpp"

namespace stdthread {

class Raduls {
 public:
    Raduls() {}

    template <class T>
    static constexpr bool accepts() {
        return Datatype<T>::hasRadulsFormat();
    }

    static bool isParallel() { return true; }

    static std::string name() { return "raduls"; }

    template <class T, template <class T1> class Vector>
    static std::pair<double, double> sort(T* begin, T* end, size_t num_threads) {
        static_assert(sizeof(T) % 8 == 0);
        assert(reinterpret_cast<uintptr_t>(begin) % raduls::ALIGNMENT == 0);

        auto start = std::chrono::high_resolution_clock::now();

        const auto size = std::distance(begin, end);
        const int key_size = sizeof(T);
        Vector<T> tmp(size, std::max<size_t>(16, raduls::ALIGNMENT));
        assert(reinterpret_cast<uintptr_t>(tmp.get()) % raduls::ALIGNMENT == 0);

        raduls::RadixSortMSD(reinterpret_cast<uint8_t*>(begin),
                             reinterpret_cast<uint8_t*>(tmp.get()), size, sizeof(T),
                             key_size, num_threads);

        if (key_size % 2 == 1) {
            std_parallel_for(
                    size, [begin, &tmp](size_t start, size_t stop, size_t /* thread*/) {
                        std::copy(tmp.get() + start, tmp.get() + stop, begin + start);
                    });
            // std::copy(reinterpret_cast<T*>(tmp.get()), reinterpret_cast<T*>(tmp.get())
            // + size,
            //           begin);
        }

        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = finish - start;

        return {0., elapsed.count()};
    }
};

}  // namespace stdthread
