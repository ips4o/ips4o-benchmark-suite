/*******************************************************************************
 * Project Ips4o Benchmark Suite
 *
 * src/parallel/parallel_checker.hpp
 *
 * Parallel checker which tests for sorted output.
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

#include <algorithm>
#include <functional>
#include <numeric>
#include <thread>
#include <vector>

#include <sort_checker.hpp>

template <class T>
class ParallelChecker {
    using Checker = checker::SortChecker<T>;

 public:
    ParallelChecker() : num_threads_(std::thread::hardware_concurrency()) {
        scs_.resize(num_threads_);
    }

    void add_pre(T* begin, T* end) {
        const size_t size = end - begin;
        const size_t thread_size = (size + num_threads_ - 1) / num_threads_;

        std::vector<std::thread> threads(num_threads_ - 1);

        auto exec = [](T* my_begin, T* my_end, Checker* sc) {
            for (auto it = my_begin; it != my_end; ++it) { sc->add_pre(*it); }
        };

        // Create threads and execute.
        for (size_t i = 0; i < num_threads_ - 1; ++i) {
            const size_t start = std::min(i * thread_size, size);
            const size_t stop = std::min(start + thread_size, size);
            threads[i] = std::thread(exec, begin + start, begin + stop, scs_.data() + i);
        }

        // Execute the remainder
        const size_t start = std::min((num_threads_ - 1) * thread_size, size);
        const size_t stop = std::min(start + thread_size, size);
        exec(begin + start, begin + stop, scs_.data() + (num_threads_ - 1));

        // Wait for the other thread to finish their task
        std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
    }

    template <class Comp>
    void add_post(T* begin, T* end, Comp comp) {
        const size_t size = end - begin;
        const size_t thread_size = (size + num_threads_ - 1) / num_threads_;

        std::vector<std::thread> threads(num_threads_ - 1);

        auto exec = [&comp](T* my_begin, T* my_end, Checker* sc) {
            for (auto it = my_begin; it != my_end; ++it) { sc->add_post(*it, comp); }
        };

        // Create threads and execute.
        for (size_t i = 0; i < num_threads_ - 1; ++i) {
            const size_t start = std::min(i * thread_size, size);
            const size_t stop = std::min(start + thread_size, size);
            threads[i] = std::thread(exec, begin + start, begin + stop, scs_.data() + i);
        }

        // Execute the remainder
        const size_t start = std::min((num_threads_ - 1) * thread_size, size);
        const size_t stop = std::min(start + thread_size, size);
        exec(begin + start, begin + stop, scs_.data() + (num_threads_ - 1));

        // Wait for the other threads to finish their task
        std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
    }

    bool is_likely_permutated() {
        return Checker::is_likely_permuted(scs_.begin(), scs_.end());
    }

    template <class Comp>
    bool is_likely_sorted(Comp comp) {
        return Checker::is_likely_sorted(scs_.begin(), scs_.end(), comp);
    }

 protected:
    const int num_threads_;
    std::vector<Checker> scs_;
};
