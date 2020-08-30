#!/bin/bash

. benchmark.sh

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

numa_test_numa 0 32
numa_test_interleaved 0 32
numa_test_striped 0 32
numa_test_copyback 0 32

numa_test_numa 33 37
numa_test_interleaved 33 37
numa_test_striped 33 37
numa_test_copyback 33 37

sequential_test 0 30
sequential_test 31 35
