/*******************************************************************************
 * Project Ips4o Benchmark Suite
 *
 * src/parallel/parallel_for.hpp
 *
 * Invoke a function with an index sequence in parallel.
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
#include <thread>
#include <vector>

template<class Fct>
static void std_parallel_for(size_t size, Fct&& fct) {
    size_t num_concurrency = std::thread::hardware_concurrency();
    size_t num_threads = num_concurrency == 0 ? 1 : (num_concurrency);

    const size_t thread_size = (size + num_threads - 1) / num_threads;

    std::vector<std::thread> threads(num_threads - 1);

    // Create threads and execute.
    for(size_t i = 0; i < num_threads - 1; ++i)
    {
      const size_t start = std::min(i * thread_size, size);
      const size_t stop = std::min(start + thread_size, size);
      
      threads[i] = std::thread(std::forward<Fct>(fct), start, stop, i);
    }

    // Execute the remainder
    // size_t start = (num_threads - 1) * thread_size;
    // fct(start, size, num_threads);

    const size_t start = std::min((num_threads - 1) * thread_size, size);
    const size_t stop = std::min(start + thread_size, size);
    fct(start, stop, num_threads - 1);
    
    // Wait for the other thread to finish their task
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
}
