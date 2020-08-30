/*******************************************************************************
 * Project Ips4o Benchmark Suite
 *
 * src/name_extractor.hpp
 *
 * Extract names from a sequence of data types.
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

#include "sequence.hpp"

template <class Sequence>
std::vector<std::string> NameExtractor() {
    constexpr bool last = Sequence::isLast();
    using SequenceClass = typename Sequence::SequenceClass;
    using SubSequence = typename Sequence::SubSequence;

    std::vector<std::string> algos = {SequenceClass::name()};
    if constexpr (!last) {
        const auto subalgos = NameExtractor<SubSequence>();
        algos.insert(algos.end(), subalgos.begin(), subalgos.end());
    }
    return algos;
}
