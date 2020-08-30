#!/bin/bash

MACHINE=$1

export CC=/software/gcc/7.5.0/bin/gcc
export CXX=/software/gcc/7.5.0/bin/g++

export LD_LIBRARY_PATH=

. benchmark.sh

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/software/gcc/7.5.0/lib64

parallel_test_numa 0 32
parallel_test_striped 0 32
parallel_test_interleaved 0 32
parallel_test_copyback 0 32

parallel_test_numa 33 37
parallel_test_striped 33 37
parallel_test_interleaved 33 37
parallel_test_copyback 33 37

speedup_test_numa 33
speedup_test_striped 33
speedup_test_interleaved 33
speedup_test_copyback 33

sequential_test 0 30

numa_test_numa 0 32
numa_test_interleaved 0 32
numa_test_striped 0 32
numa_test_copyback 0 32

numa_test_numa 33 37
numa_test_interleaved 33 37
numa_test_striped 33 37
numa_test_copyback 33 37

sequential_test 31 35

export LD_LIBRARY_PATH=

. benchmark_subroutines.sh

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/software/gcc/7.5.0/lib64

sequential_subroutines_test 0 30
parallel_subroutines_test 0 32

sequential_subroutines_test 31 35
parallel_subroutines_test 33 37
