/*******************************************************************************
 * Project Ips4o Benchmark Suite
 *
 * src/generator/generator.hpp
 *
 * Template sequence of data types.
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

#include <string>
#include <vector>

#include <tclap/CmdLine.h>

#include "generator/generator.hpp"
#include "name_extractor.hpp"
#include "vector_types.hpp"

constexpr uint32_t ALIGNMENT = 0x100;

struct Config {
    long begin_logn{0};
    long end_logn{0};
    std::vector<std::string> generators;
    std::vector<std::string> datatypes;
};

using NumericalDatatypes =
        Sequence<false, Datatype<double>,
        Sequence<false, Datatype<uint32_t>,
        Sequence<true,  Datatype<uint64_t>
    >>>;

inline Config readParameters(int argc, char* argv[]) {
    Config config;

    try {
        TCLAP::CmdLine cmd("Benchmark of different Algorithms", ' ', "0.1");

        std::vector<std::string> generator_allowed = NameExtractor<Generators>();
        std::vector<std::string> datatype_allowed = NameExtractor<NumericalDatatypes>();

        TCLAP::ValuesConstraint<std::string> generator_allowedVals(generator_allowed);

        TCLAP::ValuesConstraint<std::string> datatype_allowedVals(datatype_allowed);

        TCLAP::MultiArg<std::string> generator_arg(
                "g", "generator",
                "Name of the generator. If no generator is specified, all generators are "
                "executed.",
                false, &generator_allowedVals);
        TCLAP::MultiArg<std::string> datatype_arg(
                "d", "datatype",
                "Name of the datatype. If no datatype is specified, all datatypes are "
                "executed.",
                false, &datatype_allowedVals);

        TCLAP::ValueArg<long> begin_logsize_arg(
                "b", "beginlogsize", "The logarithm of the minimum input size in bytes.",
                true, 0, "long");

        TCLAP::ValueArg<long> end_logsize_arg(
                "e", "endlogsize",
                "The logarithm of the maximum input size in bytes (incl)", true, 0,
                "long");

        cmd.add(generator_arg);
        cmd.add(datatype_arg);
        cmd.add(begin_logsize_arg);
        cmd.add(end_logsize_arg);

        cmd.parse(argc, argv);

        config.generators = generator_arg.getValue();
        config.datatypes = datatype_arg.getValue();

        if (config.generators.empty()) { config.generators = generator_allowed; }
        if (config.datatypes.empty()) { config.datatypes = datatype_allowed; }

        config.begin_logn = begin_logsize_arg.getValue();
        config.end_logn = end_logsize_arg.getValue();

    } catch (TCLAP::ArgException& e)  // catch exceptions
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        return Config();
    }

    return config;
}

template <class T>
std::pair<size_t, size_t> logSizes(const Config& config) {
    const size_t type_log_size = tlx::integer_log2_ceil(sizeof(T));
    const size_t min =
            config.begin_logn >= type_log_size ? config.begin_logn - type_log_size : 1;
    const size_t max =
            config.end_logn >= type_log_size ? config.end_logn - type_log_size : 1;
    return {min, max};
}

template <class T, class Generator>
void print(const Config& config) {
    const auto [min_log_size, max_log_size] = logSizes<T>(config);

    Generator gen;
    for (size_t size = (1ul << min_log_size); size <= (1ul << max_log_size); size *= 2) {
        AlignedUniquePtr<T> v(size, std::max<size_t>(16, ALIGNMENT));
        assert(reinterpret_cast<uintptr_t>(v.get()) % ALIGNMENT == 0);

        gen(v.get(), v.get() + size);

        for (size_t i = 0; i != size; ++i) {
            std::cout << "RESULT"
                      << "\tgen=" << Generator::name()
                      << "\tdatatype=" << Datatype<T>::name() << "\tsize=" << size
                      << "\tidx=" << i << "\tval=" << v[i] << std::endl;
        }
    }
}

template <class T, class Generators>
void selectAndExecGenerators(const Config& config) {
    using Generator = typename Generators::SequenceClass;
    for (const auto generator : config.generators) {
        if (!Generator::name().compare(generator)) {
            if constexpr (Generator::template accepts<T>()) {
                print<T, Generator>(config);
            } else {
                std::cout << "RESULT"
                          << "\tgen=" << Generator::name() << "\tconfigwarning=1"
                          << "\tdatatype=" << Datatype<T>::name() << std::endl;
            }
        }
    }

    if constexpr (!Generators::isLast()) {
        selectAndExecGenerators<T, typename Generators::SubSequence>(config);
    }
}

template <class Datatypes>
void selectAndExecDatatype(const Config& config) {
    using TypeDescription = typename Datatypes::SequenceClass;
    using T = typename TypeDescription::value_type;

    const std::string type_name = TypeDescription::name();
    for (const auto datatype : config.datatypes) {
        if (!type_name.compare(datatype)) {
            selectAndExecGenerators<T, Generators>(config);
        }
    }

    if constexpr (!Datatypes::isLast()) {
        selectAndExecDatatype<typename Datatypes::SubSequence>(config);
    }
}

int main(int argc, char* argv[]) {
    Config config = readParameters(argc, argv);
    selectAndExecDatatype<NumericalDatatypes>(config);
    return 0;
}
