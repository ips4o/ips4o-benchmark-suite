/*******************************************************************************
 * Project Ips4o Benchmark Suite
 *
 * src/datatypes.hpp
 *
 * Provider of key extractor functions, compare functions.
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
#include <tuple>
#include <utility>

#include "pbbs_generators/data_types.h"
#include "sequence.hpp"

template <class T>
struct is_simple_key_type;

template <>
struct is_simple_key_type<double> {
    static bool constexpr value = true;
    using Type = double;
};

template <>
struct is_simple_key_type<pair_t> {
    static bool constexpr value = true;
    using Type = pair_t::int_type;
};

template <>
struct is_simple_key_type<uint32_t> {
    static bool constexpr value = true;
    using Type = uint32_t;
};

template <>
struct is_simple_key_type<uint64_t> {
    static bool constexpr value = true;
    using Type = uint64_t;
};

template <>
struct is_simple_key_type<qtuple_t> {
    static bool constexpr value = false;
};

template <>
struct is_simple_key_type<byte_t> {
    static bool constexpr value = false;
};

template <class T>
struct Datatype {
    using value_type = T;

    static std::string name();
    static constexpr auto getComparator();
    static constexpr auto getKeyExtractor();
    static constexpr auto getSkaKeyExtractor();
    static constexpr bool hasKeyExtractor();
    static constexpr bool hasUnsignedKey();
    static constexpr size_t sizeofKey();
    static constexpr bool hasRadulsFormat();
};

template <>
std::string Datatype<pair_t>::name() {
    return "pair";
}
template <>
std::string Datatype<qtuple_t>::name() {
    return "qtuple";
}
template <>
std::string Datatype<byte_t>::name() {
    return "byte";
}
template <>
std::string Datatype<double>::name() {
    return "double";
}
template <>
std::string Datatype<uint32_t>::name() {
    return "uint32";
}
template <>
std::string Datatype<uint64_t>::name() {
    return "uint64";
}

template <>
constexpr auto Datatype<pair_t>::getComparator() {
    return std::less<pair_t>{};
}
template <>
constexpr auto Datatype<byte_t>::getComparator() {
    return std::less<byte_t>{};
}
template <>
constexpr auto Datatype<qtuple_t>::getComparator() {
    return std::less<qtuple_t>{};
}
template <>
constexpr auto Datatype<double>::getComparator() {
    return std::less<double>{};
}
template <>
constexpr auto Datatype<uint32_t>::getComparator() {
    return std::less<uint32_t>{};
}
template <>
constexpr auto Datatype<uint64_t>::getComparator() {
    return std::less<uint64_t>{};
}

template <>
constexpr auto Datatype<pair_t>::getSkaKeyExtractor() {
    return [](const pair_t& a) { return a.k; };
}
template <>
constexpr auto Datatype<byte_t>::getSkaKeyExtractor() {
    return [](const byte_t& a) {
        return std::make_tuple(a.k[0], a.k[1], a.k[2], a.k[3], a.k[4], a.k[5], a.k[6],
                               a.k[7], a.k[8], a.k[9]);
    };
}
template <>
constexpr auto Datatype<qtuple_t>::getSkaKeyExtractor() {
    return [](const qtuple_t& a) { return std::make_tuple(a.k1, a.k2, a.k3); };
}
// byte_t does not have a key extractor function
// qtuple_t does not have a key extractor function
template <>
constexpr auto Datatype<uint32_t>::getSkaKeyExtractor() {
    return [](const uint32_t& a) { return a; };
}
template <>
constexpr auto Datatype<uint64_t>::getSkaKeyExtractor() {
    return [](const uint64_t& a) { return a; };
}
template <>
constexpr auto Datatype<double>::getSkaKeyExtractor() {
    return [](const double& a) { return a; };
}

template <>
constexpr auto Datatype<pair_t>::getKeyExtractor() {
    return [](const pair_t& a) { return a.k; };
}
// byte_t does not have a key extractor function
// qtuple_t does not have a key extractor function
template <>
constexpr auto Datatype<uint32_t>::getKeyExtractor() {
    return [](const uint32_t& a) { return a; };
}
template <>
constexpr auto Datatype<uint64_t>::getKeyExtractor() {
    return [](const uint64_t& a) { return a; };
}
template <>
constexpr auto Datatype<double>::getKeyExtractor() {
    return [](const double& a) { return a; };
}

template <>
constexpr bool Datatype<pair_t>::hasKeyExtractor() {
    return true;
}
template <>
constexpr bool Datatype<byte_t>::hasKeyExtractor() {
    return false;
}
template <>
constexpr bool Datatype<qtuple_t>::hasKeyExtractor() {
    return false;
}
template <>
constexpr bool Datatype<double>::hasKeyExtractor() {
    return true;
}
template <>
constexpr bool Datatype<uint32_t>::hasKeyExtractor() {
    return true;
}
template <>
constexpr bool Datatype<uint64_t>::hasKeyExtractor() {
    return true;
}

template <>
constexpr bool Datatype<pair_t>::hasUnsignedKey() {
    return std::is_unsigned_v<pair_t::int_type>;
}
template <>
constexpr bool Datatype<byte_t>::hasUnsignedKey() {
    return false;
}
template <>
constexpr bool Datatype<qtuple_t>::hasUnsignedKey() {
    return false;
}
template <>
constexpr bool Datatype<double>::hasUnsignedKey() {
    return false;
}
template <>
constexpr bool Datatype<uint32_t>::hasUnsignedKey() {
    return true;
}
template <>
constexpr bool Datatype<uint64_t>::hasUnsignedKey() {
    return true;
}

template <>
constexpr size_t Datatype<pair_t>::sizeofKey() {
    return sizeof(pair_t::int_type);
}
template <>
constexpr size_t Datatype<byte_t>::sizeofKey() {
    return byte_t::keySize;
}
template <>
constexpr size_t Datatype<qtuple_t>::sizeofKey() {
    return sizeof(qtuple_t);
}
template <>
constexpr size_t Datatype<double>::sizeofKey() {
    return sizeof(double);
}
template <>
constexpr size_t Datatype<uint32_t>::sizeofKey() {
    return sizeof(uint32_t);
}
template <>
constexpr size_t Datatype<uint64_t>::sizeofKey() {
    return sizeof(uint64_t);
}

template <>
constexpr bool Datatype<pair_t>::hasRadulsFormat() {
    return sizeof(pair_t::int_type) % 8 == 0 && sizeof(pair_t) % 8 == 0;
}
template <>
constexpr bool Datatype<byte_t>::hasRadulsFormat() {
    return false;
}
template <>
constexpr bool Datatype<qtuple_t>::hasRadulsFormat() {
    return false;
}
template <>
constexpr bool Datatype<double>::hasRadulsFormat() {
    return false;
}
template <>
constexpr bool Datatype<uint32_t>::hasRadulsFormat() {
    return false;
}
template <>
constexpr bool Datatype<uint64_t>::hasRadulsFormat() {
    return true;
}

using Datatypes =
        Sequence<false, Datatype<pair_t>,
        Sequence<false, Datatype<qtuple_t>,
        Sequence<false, Datatype<byte_t>,
        Sequence<false, Datatype<double>,
        Sequence<false, Datatype<uint32_t>,
        Sequence<true,  Datatype<uint64_t>
    >>>>>>;
