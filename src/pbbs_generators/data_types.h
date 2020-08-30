// src/pbbs_generators/data_types.h
//
// Data types of this benchmark.
//
// Code borrowed and modified from the Problem Based Benchmark Suite
// http://www.cs.cmu.edu/~pbbs/benchmarks.html
//
// This code is part of the Problem Based Benchmark Suite (PBBS)
// Copyright (c) 2010 Guy Blelloch and the PBBS team
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights (to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// Part of Project Ips4o Benchmark Suite - https://github.com/MichaelAxtmann/ips4o-benchmark-suite
//
// Copyright (c) 2020 Michael Axtmann <michael.axtmann@gmail.com>

#pragma once

#include <cstdint>
#include <cstring>

#include <SimdMt.hpp>

struct pair_t {
    using int_type = uint64_t;

    pair_t(int_type k) : k(k) {}
    pair_t(int_type k, int_type v) : k(k), v(v){};
    pair_t(const pair_t& p) : k(p.k), v(p.v) {}
    pair_t() {}

    bool operator<(const pair_t& p) const { return k < p.k; }

    bool operator>(const pair_t& p) const { return k > p.k; }

    bool operator==(const pair_t& p) const { return k == p.k; }

    bool operator!=(const pair_t& p) const { return !(*this == p); }

    int_type k, v;
};

struct qtuple_t {
    using int_type = uint64_t;

    qtuple_t(int_type k1, int_type k2, int_type k3, int_type v)
        : k1(k1), k2(k2), k3(k3), v(v){};

    qtuple_t(const qtuple_t& q) : k1(q.k1), k2(q.k2), k3(q.k3), v(q.v) {}

    qtuple_t() {}

    bool operator<(const qtuple_t& q) const {
        if (k1 != q.k1)
            return k1 < q.k1;
        else if (k2 != q.k2)
            return k2 < q.k2;
        else
            return k3 < q.k3;
    }

    bool operator>(const qtuple_t& q) const {
        if (k1 != q.k1)
            return k1 > q.k1;
        else if (k2 != q.k2)
            return k2 > q.k2;
        else
            return k3 > q.k3;
    }

    /* bool operator <(const qtuple_t& q) const { */
    /*   return std::make_tuple(k1, k2, k3) < std::make_tuple(q.k1, q.k2, q.k3); */
    /* } */

    /* bool operator >(const qtuple_t& q) const { */
    /*   return std::make_tuple(k1, k2, k3) > std::make_tuple(q.k1, q.k2, q.k3); */
    /* } */

    /* bool operator <(const qtuple_t& q) const { */
    /*   return (k1 < q.k1); */
    /* } */

    /* bool operator >(const qtuple_t& q) const { */
    /*   return (k1 > q.k1); */
    /* } */

    /* bool operator <(const qtuple_t& q) const { */
    /*   return (k1 < q.k1) || (k1 == q.k1 && k2 < q.k2) */
    /*     || (k1 == q.k1 && k2 == q.k2 && k3 < q.k3);  */
    /* } */

    /* bool operator >(const qtuple_t& q) const { */
    /*   return (k1 > q.k1) || (k1 == q.k1 && k2 > q.k2) */
    /*     || (k1 == q.k1 && k2 == q.k2 && k3 > q.k3); */
    /* } */

    bool operator==(const qtuple_t& q) const {
        return k1 == q.k1 && k2 == q.k2 && k3 == q.k3;
    }

    bool operator!=(const qtuple_t& q) const { return !(*this == q); }

    int_type k1, k2, k3, v;
};

struct byte_t {
    static constexpr const size_t keySize = 10;
    static constexpr const size_t valSize = 90;

    using int_type = uint8_t;

    byte_t(const byte_t& b) {
        memcpy(k, b.k, keySize);
        memcpy(v, b.v, valSize);
    }

    byte_t(){};

    byte_t(SimdMtGeneratorUint64& gen) {
        uint64_t* ptr = reinterpret_cast<uint64_t*>(k);
        *ptr = gen();
        *(ptr + 1) = gen();
    }

    bool operator<(const byte_t& b) const {
        for (int i = 0; i < 10; ++i) {
            if (k[i] != b.k[i]) return k[i] < b.k[i];
        }
        return false;
        // return memcmp(k, b.k, keySize) < 0;
    }

    bool operator>(const byte_t& b) const {
        for (int i = 0; i < 10; ++i) {
            if (k[i] != b.k[i]) return k[i] > b.k[i];
        }
        return false;
        // return memcmp(k, b.k, keySize) > 0;
    }

    bool operator==(const byte_t& b) const {
        for (int i = 0; i < 10; ++i) {
            if (k[i] != b.k[i]) return false;
        }
        return true;
        // return memcmp(k, b.k, keySize) == 0;
    }

    bool operator!=(const byte_t& b) const {
        for (int i = 0; i < 10; ++i) {
            if (k[i] == b.k[i]) return false;
        }
        return true;
        // return !(*this == b);
    }

    int_type k[keySize];
    int_type v[valSize];
};
