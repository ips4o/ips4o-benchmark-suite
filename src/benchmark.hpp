/*******************************************************************************
 * Project Ips4o Benchmark Suite
 *
 * src/benchmark.hpp
 *
 * Core of the benchmark.
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
#include <algorithm>
#include <random>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

#include <tclap/CmdLine.h>
#include <numa_array.hpp>
#include <tlx/math.hpp>

// #include "defs.h"
#include "timer.hpp"
#include "typename.hpp"
#include "name_extractor.hpp"
#include "generator/generator.hpp"
#include "datatypes.hpp"
#include "config.hpp"
#include "vector_types.hpp"
#include "parallel/parallel_checker.hpp"

constexpr uint32_t ALIGNMENT = 0x100;

template<class T>
std::pair<size_t, size_t> logSizes(const Config& config) {
  const size_t type_log_size = tlx::integer_log2_ceil(sizeof(T));
  const size_t min = config.begin_logn >= type_log_size ? config.begin_logn - type_log_size : 1;
  const size_t max = config.end_logn >= type_log_size ? config.end_logn - type_log_size : 1;
  return {min, max};
}

template<class T>
constexpr int numRuns(const Config& config, size_t size, bool parallel_algo) {
  if (config.runs > 0) return config.runs;

  if (parallel_algo && ((sizeof(T) * size) < (1ul << 33))) return 15;
  else if (!parallel_algo && (sizeof(T) * size) < (1ul << 30)) return 15;
  else return 2;
}

template<class T, class Generator, class Algo, template<class T1> class Vector>
void exec(const Config& config) {
  
  // std::vector<T> v;
  const auto [min_log_size, max_log_size] = logSizes<T>(config);
  
  Generator gen;
  for (size_t size = (1ul << min_log_size); size <= (1ul << max_log_size); size *= 2) {
    // v.resize(size);
    Vector<T> v(size, std::max<size_t>(16, ALIGNMENT));
    assert(reinterpret_cast<uintptr_t>(v.get()) % ALIGNMENT == 0);
    for (int run = 0; run != numRuns<T>(config, size, Algo::isParallel()); ++run) {
      
      auto start_gen = std::chrono::high_resolution_clock::now();
      gen(v.get(), v.get() + size);
      const auto copyback = !Algo::isParallel() || config.copyback;
      if (copyback) {
        // Copy data into a new array by the main thread as the
        // parallel generators may create pages at all numa nodes.
        Vector<T> v1(size, std::max<size_t>(16, ALIGNMENT));
        std::copy(v.get(), v.get() + size, v1.get());
        v = std::move(v1);
      }
      auto finish_gen = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double, std::milli> elapsed_gen = finish_gen - start_gen;
      

      auto start_checker = std::chrono::high_resolution_clock::now();
      ParallelChecker<T> checker;
      checker.add_pre(v.get(), v.get() + size);
      auto finish_checker = std::chrono::high_resolution_clock::now();
      double time_checker = std::chrono::duration<double, std::milli>(
        finish_checker - start_checker).count();

      const auto [preprocessing, sorting] = Algo::template sort<T, Vector>(
        v.get(), v.get() + size, config.num_threads);

      start_checker = std::chrono::high_resolution_clock::now();
      checker.add_post(v.get(), v.get() + size, Datatype<T>::getComparator());
      finish_checker = std::chrono::high_resolution_clock::now();
      time_checker += std::chrono::duration<double, std::milli>(
        finish_checker - start_checker).count();
          
      std::cout << "RESULT"
                << "\tmachine=" << config.machine
                << "\tgen=" << Generator::name()
                << "\tdatatype=" << Datatype<T>::name()
                << "\talgo=" << Algo::name()
                << "\tparallel=" << Algo::isParallel()
                << "\tthreads=" << config.num_threads
                << "\tvector=" << Vector<T>::name()
                << "\tcopyback=" << copyback
                << "\tsize=" << size
                << "\trun=" << run
                << "\tbenchmarkconfigerror=0"
                << "\tcheckermilli=" << time_checker
                << "\tgeneratormilli=" << elapsed_gen.count()
                << "\tpreprocmilli=" << preprocessing
                << "\tmilli=" << sorting
                << "\tsortedsequence=" << checker.is_likely_sorted(Datatype<T>::getComparator())
                << "\tpermutation=" << checker.is_likely_permutated()
                << config.info;
      
#ifdef IPS4O_TIMER
      std::cout << "\tbasecase=" << g_base_case.getTime()
		<< "\tsampling=" << g_sampling.getTime()
		<< "\tclassificationphase=" << g_classification.getTime()
		<< "\tpermutationphase=" << g_permutation.getTime()
		<< "\tcleanup=" << g_cleanup.getTime()
		<< "\toverhead=" << g_overhead.getTime()
		<< "\temptyblock=" << g_empty_block.getTime()
		<< "\ttotal=" << g_total.getTime();
      g_base_case.reset();      
      g_sampling.reset();      
      g_classification.reset();      
      g_permutation.reset();      
      g_cleanup.reset();      
      g_overhead.reset();
      g_empty_block.reset();
      g_total.reset();      
#endif

	std::cout << std::endl;
    }
  }
}

template<class T, class Generator, class Algo>
void selectAndExecVector(const Config& config) {
  if (std::find(config.vectors.begin(), config.vectors.end(), AlignedUniquePtr<T>::name()) != config.vectors.end()) {
    exec<T, Generator, Algo, AlignedUniquePtr>(config);
  }
  if (std::find(config.vectors.begin(), config.vectors.end(), Numa::AlignedArray<T>::name()) != config.vectors.end()) {
    exec<T, Generator, Algo, Numa::AlignedArray>(config);
  }
}

template<class T, class Generator, class Algorithms>
void selectAndExecAlgo(const Config& config) {
  using Algorithm = typename Algorithms::SequenceClass;
  for (const auto algo : config.algos) {
    if (!Algorithm::name().compare(algo)) {
      if constexpr(Algorithm::template accepts<T>()) {
          selectAndExecVector<T, Generator, Algorithm>(config);
        } else {
        std::cout << "RESULT"
                  << "\talgo=" << Algorithm::name()
                  << "\tconfigwarning=1"
                  << "\tdatatype=" << Datatype<T>::name()<< std::endl;
      }
    }
  }

  if constexpr (!Algorithms::isLast()) {
      selectAndExecAlgo<T, Generator, typename Algorithms::SubSequence>(config);
    }
}

template<class T, class Algorithms, class Generators>
void selectAndExecGenerators(const Config& config) {
  using Generator = typename Generators::SequenceClass;
  for (const auto generator : config.generators) {
    if (!Generator::name().compare(generator)) {
      if constexpr (Generator::template accepts<T>()) {
          selectAndExecAlgo<T, Generator, Algorithms>(config);
        } else {
        std::cout << "RESULT"
                  << "\tgen=" << Generator::name()
                  << "\tconfigwarning=1"
                  << "\tdatatype=" << Datatype<T>::name()<< std::endl;
      }
    }
  }

  if constexpr (!Generators::isLast()) {
      selectAndExecGenerators<T, Algorithms, typename Generators::SubSequence>(config);
    }
}

template<class Algorithms, class Datatypes>
void selectAndExecDatatype(const Config& config) {
  
  using TypeDescription = typename Datatypes::SequenceClass;
  using T = typename TypeDescription::value_type;
  
  const std::string type_name = TypeDescription::name();
  for (const auto datatype : config.datatypes) {
    if (!type_name.compare(datatype)) {
      selectAndExecGenerators<T, Algorithms, Generators>(config);
    }
  }

  if constexpr (!Datatypes::isLast()) {
      selectAndExecDatatype<Algorithms, typename Datatypes::SubSequence>(config);
    }
}

template<class Algorithms>
void benchmark(const Config& config) {
  selectAndExecDatatype<Algorithms, Datatypes>(config);
}

inline
Config readParameters(int argc, char* argv[], std::vector<std::string> algo_allowed) {
  Config config;

  try {

    TCLAP::CmdLine cmd("Benchmark of different Algorithms", ' ', "0.1");

    std::vector<std::string> generator_allowed = NameExtractor<Generators>();
    std::vector<std::string> datatype_allowed = NameExtractor<Datatypes>();
    std::vector<std::string> vector_allowed = get_vector_types();

    TCLAP::ValuesConstraint<std::string> generator_allowedVals(generator_allowed);

    TCLAP::ValuesConstraint<std::string> algo_allowedVals(algo_allowed);
    
    TCLAP::ValuesConstraint<std::string> datatype_allowedVals(datatype_allowed);

    TCLAP::ValuesConstraint<std::string> vector_allowedVals(vector_allowed);

    TCLAP::SwitchArg copyback_arg("c", "copyback", "Copy generated values into a new array by the master thread. Inputs for sequential algorithms are always copied back.", false);

    TCLAP::MultiArg<std::string> generator_arg("g","generator","Name of the generator. If no generator is specified, all generators are executed.",
                                               false, &generator_allowedVals);
    TCLAP::MultiArg<std::string> algo_arg("a","algorithm","Name of the algorithm. If no algorithm is specified, all algorithms are executed.",
                                          false , &algo_allowedVals);
    TCLAP::MultiArg<std::string> datatype_arg("d","datatype","Name of the datatype. If no datatype is specified, all datatypes are executed.",
                                              false, &datatype_allowedVals);
    
    TCLAP::MultiArg<std::string> vector_arg("v","vector","Name of the vector. If no vector is specified, all vectors are executed.",
					    false, &vector_allowedVals);
    
    TCLAP::ValueArg<long> runs_arg("r","runs",
                                   "Number of runs. If the number of runs is not set, each algorithm is executed 15 times for inputs less than 2^33 bytes and 2 times for larger inputs.",
                                   false, -1,"long");

    TCLAP::ValueArg<std::string> machine_arg("m","machine","Name of the machine",
                                             true,"","string");

    TCLAP::ValueArg<std::string> info_arg("i","info","Additional information provided by the user about this run. The information is appended to the result line",
                                          false,"","string");

    TCLAP::ValueArg<long> threads_arg("t","threads",
                                      "Number of threads",
                                      true, 0,"long");
    
    TCLAP::ValueArg<long> begin_logsize_arg("b","beginlogsize",
                                            "The logarithm of the minimum input size in bytes.",
                                            true, 0,"long");
    
    TCLAP::ValueArg<long> end_logsize_arg("e","endlogsize",
                                          "The logarithm of the maximum input size in bytes (incl)",
                                          true, 0,"long");

    cmd.add(copyback_arg);
    cmd.add(machine_arg);
    cmd.add(info_arg);
    cmd.add(algo_arg);
    cmd.add(generator_arg);
    cmd.add(datatype_arg);
    cmd.add(vector_arg);
    cmd.add(runs_arg);
    cmd.add(threads_arg);
    cmd.add(begin_logsize_arg);
    cmd.add(end_logsize_arg);

    cmd.parse( argc, argv );

    config.copyback = copyback_arg.getValue();
    config.algos = algo_arg.getValue();
    config.generators = generator_arg.getValue();
    config.datatypes = datatype_arg.getValue();
    config.vectors = vector_arg.getValue();

    if (config.algos.empty()) {
      config.algos = algo_allowed;
    }
    if (config.generators.empty()) {
      config.generators = generator_allowed;
    }
    if (config.datatypes.empty()) {
      config.datatypes = datatype_allowed;
    }
    if (config.vectors.empty()) {
      config.vectors = vector_allowed;
    }

    config.machine = machine_arg.getValue();
    config.info = info_arg.getValue();
    config.num_threads = threads_arg.getValue();
    config.runs = runs_arg.getValue();
    config.begin_logn = begin_logsize_arg.getValue();
    config.end_logn = end_logsize_arg.getValue();

  } catch (TCLAP::ArgException &e)  // catch exceptions
  { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; return Config();}

  return config;
  
}

