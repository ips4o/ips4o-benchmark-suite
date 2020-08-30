/*******************************************************************************
 * Project Ips4o Benchmark Suite
 *
 * src/generator/simple_alias.hpp
 *
 * Alias method implementations. Simplified implementation of an alias
 * table extracted from
 * https://github.com/lorenzhs/wrs/blob/master/wrs/alias.hpp
 *
 * Copyright (C) 2018-2019 Lorenz Hübschle-Schneider <lorenz@4z2.de>
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
#ifndef WRS_SIMPLE_ALIAS_HEADER
#define WRS_SIMPLE_ALIAS_HEADER

#include <algorithm>
#include <cassert>
#include <cstring>
#include <limits>
#include <memory>
#include <numeric>
#include <utility>
#include <vector>

namespace wrs {

template <typename size_type = uint32_t>
struct simple_alias {
    static constexpr const char *name = "simple_alias";
    static constexpr bool yields_single_sample = true;
    static constexpr bool init_with_seed = false;
    static constexpr int pass_rand = 1;
    using result_type = size_type;
    using pair = std::pair<size_type, double>;

    static_assert((sizeof(double) / sizeof(size_type)) * sizeof(size_type)
                          == sizeof(double),
                  "size_type has weird size");
    static constexpr size_t ti_ptroffset = sizeof(double) / sizeof(size_type);

    // Simple_Alias table member item, used to be a std::tuple<double, size_type,
    // size_type>
    struct tableitem {
        double p;     // probability
        size_type i;  // item
        size_type a;  // alia

        tableitem() : p(0), i(-1), a(-1) {}
        tableitem(double p_, size_type i_, size_type a_) : p(p_), i(i_), a(a_) {}
    };

    friend std::ostream &operator<<(std::ostream &os, const tableitem &item) {
        return os << '(' << item.p << ',' << item.i << ',' << item.a << ')';
    }

    static constexpr bool debug = false;

    simple_alias() : table_(nullptr), work_(nullptr), size_(-1), W_(0) {}

    template <typename Iterator>
    simple_alias(Iterator w_begin, Iterator w_end) {
        init(w_end - w_begin);
        construct(w_begin, w_end);
    }

    simple_alias &operator=(const simple_alias &other) {
        size_ = other.size_;
        W_ = other.W_;
        W_n_ = other.W_n_;
        if (size_ != static_cast<size_t>(-1)) {
            table_.resize(size_);
            memcpy(table_.data(), other.table_.data(), size_ * sizeof(tableitem));
        } else {
            table_ = nullptr;
        }
        return *this;
    }
    simple_alias(const simple_alias &other) { *this = other; }
    //! delete move-constructor
    simple_alias(simple_alias &&) = delete;
    //! delete move-assignment
    simple_alias &operator=(simple_alias &&) = delete;

    void init(size_t size) {
        table_.resize(size);
        work_.resize(size);
        size_ = size;
    }

    template <typename Iterator>
    void construct(Iterator begin, Iterator end) {
        if (end - begin != static_cast<ssize_t>(size_)) { return; }
        if (size_ > static_cast<size_t>(std::numeric_limits<size_type>::max())) {
            return;
        }

        W_ = std::accumulate(begin, end, 0.0);
        W_n_ = W_ / size_;

        // Classify into small and large items
        ssize_t i_small = 0, i_large = size_ - 1;
        for (Iterator it = begin; it != end; ++it) {
            if (is_small(*it)) {
                work_[i_small++] = {it - begin, *it};
            } else {
                work_[i_large--] = {it - begin, *it};
            }
        }
        assert(i_small > i_large);  // must meet

        // Assign items
        auto s_begin = work_.data(), s_end = work_.data() + i_small, l_begin = s_end,
             l_end = work_.data() + size_, small = s_begin, large = l_begin;
        while (small != s_end && large != l_end) {
            table_[small->first] = tableitem(small->second, small->first, large->first);
            large->second = (large->second + small->second) - W_n_;

            if (is_small(large)) {
                if (large > s_end) {
                    std::swap(large, s_end);
                } else {
                }
                s_end++;
                // l_begin++; // not really needed, just for bookkeping
                large++;
            } else {
            }
            small++;
        }
        while (large != l_end) {
            size_t i_large = large->first;

            table_[i_large] = tableitem(W_n_, i_large, i_large);
            large++;
        }
        while (small != s_end) {
            size_t i_small = small->first;

            table_[i_small] = tableitem(W_n_, i_small, i_small);
            small++;
        }
    }

    // Query given a uniformly distributed [0,1) random value
    size_type operator()(double uniform) const {
        assert(size_ != static_cast<size_t>(-1));
        double rand = uniform * size_;
        size_t index = rand;
        tableitem item = table_[index];

        // sanity check: if simple_alias == -1, then prob == 1
        assert(item.a != static_cast<size_type>(-1) || std::abs(item.p - 1) < 1e-10);

        rand = (rand - index) * W_n_;
        size_t offset = rand >= item.p;
        assert(item.a != static_cast<size_type>(-1) || offset == 0);
        // ugly hack
        return *(reinterpret_cast<size_type *>(&item) + ti_ptroffset + offset);
    }

    double total_weight() const { return W_; }

    size_t size() const { return size_; }

    template <typename Callback>
    void find(size_type item, Callback &&callback) const {
        for (size_t i = 0; i < size_; i++) {
            if (table_[i].i == item) {
                callback(0, i, table_[i].p, table_[i]);
            } else if (table_[i].a == item) {
                callback(0, i, W_n_ - table_[i].p, table_[i]);
            }
        }
    }

 protected:
    inline bool is_small(const double &d) const { return d <= W_n_; }

    template <typename Iterator>
    inline bool is_small(Iterator it) const {
        return it->second <= W_n_;
    }

    std::vector<tableitem> table_;
    std::vector<pair> work_;
    size_t size_;
    double W_, W_n_;
};

}  // namespace wrs

#endif  // WRS_SIMPLE_ALIAS_HEADER
