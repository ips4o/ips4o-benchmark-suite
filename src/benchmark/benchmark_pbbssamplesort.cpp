/*******************************************************************************
 * Project Ips4o Benchmark Suite
 *
 * src/benchmark/benchmark_pbbssamplesort.cpp
 *
 * Parallel sample sort form the PBBS benchmark suite benchmark.
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

#include "../algorithm/pbbssamplesort.hpp"
#include "../benchmark.hpp"
#include "../name_extractor.hpp"

using Algorithm = Sequence<true, cilk_algos::PbbsSamplesort>;

int main(int argc, char *argv[]) {
    Config config = readParameters(argc, argv, NameExtractor<Algorithm>());

    __cilkrts_end_cilk();
    const std::string num_workers = std::to_string(config.num_threads);
    __cilkrts_set_param("nworkers", num_workers.c_str());

    benchmark<Algorithm>(config);
    return 0;
}
