/*******************************************************************************
 * Project Ips4o Benchmark Suite
 *
 * src/config.hpp
 *
 * Configuration of a benchmark job.
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
#include <vector>

struct Config {
    std::string machine{""};
    int runs{0};
    int num_threads{1};
    long begin_logn{0};
    long end_logn{0};
    std::vector<std::string> algos;
    std::vector<std::string> generators;
    std::vector<std::string> datatypes;
    std::vector<std::string> vectors;
    std::string info;
    bool copyback;
};
