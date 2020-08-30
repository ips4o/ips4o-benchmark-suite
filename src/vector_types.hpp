/*******************************************************************************
 * Project Ips4o Benchmark Suite
 *
 * src/generator/generator.hpp
 *
 * Names of possible input vectors.
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

#include <numa_array.hpp>
#include <AlignedUniquePtr.hpp>

inline
std::vector<std::string> get_vector_types() {
  return std::vector<std::string>({Numa::AlignedArray<int>::name(), AlignedUniquePtr<int>::name()});
}
