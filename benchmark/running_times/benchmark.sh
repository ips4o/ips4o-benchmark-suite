#!/bin/bash

set -x # echo on
set +e # Disable exit on non 0

TOTAL_MEMORY=$(awk '/MemTotal/{print $2}' /proc/meminfo)

ulimit -Sv $TOTAL_MEMORY

PARALLEL_ALGOS=(benchmark_ips4ooldparallel benchmark_aspasparallel benchmark_raduls benchmark_ips4oparallel benchmark_ps4oparallel benchmark_mcstlbq benchmark_mcstlubq benchmark_tbbparallelsort benchmark_regionsort benchmark_pbbsradixsort benchmark_pbbssamplesort benchmark_mcstlmwm benchmark_imsdradix benchmark_ips2raparallel)
SEQUENTIAL_ALGOS=(benchmark_skasort benchmark_yaroslavskiy benchmark_blockquicksort benchmark_wikisort benchmark_timsort benchmark_pdqsort benchmark_stdsort benchmark_ssss benchmark_ips4o benchmark_ps4o  benchmark_quickxsort benchmark_ippradixsort benchmark_learnedsort benchmark_ips2ra)

DATATYPES=(double uint32 uint64 pair qtuple byte)
GENERATORS=(random sorted reverse rootdupls zero zipf qtuple byte exponential twicedupes eightdupes almostsorted)

function mydate {
    DATE=$(date '+%Y-%m-%d_%H_%M_%S')
    echo $DATE
}

CORES_PER_SOCKET=`grep "^cpu cores" /proc/cpuinfo | cut -d: -f2 | sort -n | tail -n1 | xargs`
TOTAL_CORES=$(((`grep "^physical id" /proc/cpuinfo | cut -d: -f2 | sort -n | tail -n1 | xargs` + 1) * CORES_PER_SOCKET))

TAB=$'\t'

BUILD_DIR="../../RELEASE_${MACHINE}"

mkdir -p $BUILD_DIR
cd $BUILD_DIR
cmake .. -DIPPRADIXSORT=Enable
ls -l
make -j ${TOTAL_CORES} ${SEQUENTIAL_ALGOS[*]} ${PARALLEL_ALGOS[*]} VERBOSE=1
cd -

# OUT_FILE="stats_stackless-${MACHINE}.txt"
# /bin/cp /dev/null "$OUT_FILE"

function print_infos {
    INFO_FILE=${1}.info
    echo "This files contains information about this benchmark run (all files with the machine and date of this file)" >> ${INFO_FILE}
    gcc --version >> ${INFO_FILE}
    git rev-parse HEAD >> ${INFO_FILE}
    git diff >> ${INFO_FILE}
    git status >> ${INFO_FILE}
}

# Parallel algorithms on all sockets with all datatypes and
# generators.  Allocate array 'NumaAlignedArray' in a striped manner
# by pinning the memory pages.
function numa_test_numa {
    make -j ${TOTAL_CORES} ${PARALLEL_ALGOS[*]} VERBOSE=1

    MIN_LOG_PARALLEL=$1
    MAX_LOG_PARALLEL=$2

    OUTPUT_FILE=b1_parallel_numa_${MACHINE}_$(mydate)
    print_infos ${OUTPUT_FILE}
    for algo in ${PARALLEL_ALGOS[*]}; do
        echo "Run parallel numa benchmark; algorithm=" $algo | tee ${OUTPUT_FILE}.log
        # Allocate memory round robin on all numa nodes
        for type in double uint64; do
            ${BUILD_DIR}/$algo -b $MIN_LOG_PARALLEL -e $MAX_LOG_PARALLEL -t $((2 * TOTAL_CORES)) -g random -v NumaAlignedArray -d $type -m ${MACHINE} -i "${TAB}meminterleaved=0" >> ${OUTPUT_FILE}.txt 2>> ${OUTPUT_FILE}.err
            echo "Finished: algorithm=" $algo | tee ${OUTPUT_FILE}.log
        done
    done
}

# Parallel algorithms on all sockets with all datatypes and
# generators.  Use numactl to allocate array 'vector' and array
# 'NumaAlignedArray' round robin
function numa_test_interleaved {
    make -j ${TOTAL_CORES} ${PARALLEL_ALGOS[*]} VERBOSE=1

    MIN_LOG_PARALLEL=$1
    MAX_LOG_PARALLEL=$2

    OUTPUT_FILE=b1_parallel_numa_${MACHINE}_$(mydate)
    print_infos ${OUTPUT_FILE}
    for algo in ${PARALLEL_ALGOS[*]}; do
        echo "Run parallel numa benchmark; algorithm=" $algo | tee ${OUTPUT_FILE}.log
        # Allocate memory round robin on all numa nodes
        for type in double uint64; do
            numactl -i all ${BUILD_DIR}/$algo -b $MIN_LOG_PARALLEL -e $MAX_LOG_PARALLEL -t $((2 * TOTAL_CORES)) -g random -v vector -d $type -m ${MACHINE} -i "${TAB}meminterleaved=1" >> ${OUTPUT_FILE}.txt 2>> ${OUTPUT_FILE}.err
            echo "Finished: algorithm=" $algo | tee ${OUTPUT_FILE}.log
        done
    done
}

# Parallel algorithms on all sockets with all datatypes and
# generators.  Allocate array 'vector' in a striped manner (implicitly
# with the parallel generator).
function numa_test_striped {
    make -j ${TOTAL_CORES} ${PARALLEL_ALGOS[*]} VERBOSE=1

    MIN_LOG_PARALLEL=$1
    MAX_LOG_PARALLEL=$2

    OUTPUT_FILE=b1_parallel_numa_${MACHINE}_$(mydate)
    print_infos ${OUTPUT_FILE}
    for algo in ${PARALLEL_ALGOS[*]}; do
        echo "Run parallel numa benchmark; algorithm=" $algo | tee ${OUTPUT_FILE}.log
        # Allocate memory round robin on all numa nodes
        for type in double uint64; do
            ${BUILD_DIR}/$algo -b $MIN_LOG_PARALLEL -e $MAX_LOG_PARALLEL -t $((2 * TOTAL_CORES)) -g random -d $type -v vector -m ${MACHINE} -i "${TAB}meminterleaved=0" >> ${OUTPUT_FILE}.txt 2>> ${OUTPUT_FILE}.err
            echo "Finished: algorithm=" $algo | tee ${OUTPUT_FILE}.log
        done
    done
}

# Parallel algorithms on all sockets with all datatypes and
# generators.  Allocate array 'vector' on master thread's NUMA node
# (by copying the data into a new array with the master thread)
function numa_test_copyback {
    make -j ${TOTAL_CORES} ${PARALLEL_ALGOS[*]} VERBOSE=1

    MIN_LOG_PARALLEL=$1
    MAX_LOG_PARALLEL=$2

    OUTPUT_FILE=b1_parallel_numa_${MACHINE}_$(mydate)
    print_infos ${OUTPUT_FILE}
    for algo in ${PARALLEL_ALGOS[*]}; do
        echo "Run parallel numa benchmark; algorithm=" $algo | tee ${OUTPUT_FILE}.log
        # Allocate memory round robin on all numa nodes
        for type in double uint64; do
            ${BUILD_DIR}/$algo -b $MIN_LOG_PARALLEL -e $MAX_LOG_PARALLEL -t $((2 * TOTAL_CORES)) -g random -d $type -v vector -c -m ${MACHINE} -i "${TAB}meminterleaved=0" >> ${OUTPUT_FILE}.txt 2>> ${OUTPUT_FILE}.err
            echo "Finished: algorithm=" $algo | tee ${OUTPUT_FILE}.log
        done
    done
}

# Speedup of all parallel algorithms for double, uint64_t, and
# uint32_t.  Allocate array 'vector' on master thread's NUMA node (by
# copying the data into a new array with the master thread)
function speedup_test_copyback {
    make -j ${TOTAL_CORES} ${PARALLEL_ALGOS[*]} VERBOSE=1

    SPEEDUP_LOG_SIZE=$1

    OUTPUT_FILE=b1_speedup_${MACHINE}_$(mydate)
    print_infos ${OUTPUT_FILE}
    for c in `seq 0 $((TOTAL_CORES - 1))`; do
        for algo in ${PARALLEL_ALGOS[*]}; do
            echo "Run speedup benchmark; algorithm=" $algo | tee ${OUTPUT_FILE}.log
            NUM_NODES=$((c / CORES_PER_SOCKET))
            for type in double uint64; do
                numactl -C 0-$c,$((TOTAL_CORES))-$((c + TOTAL_CORES)) \
                        ${BUILD_DIR}/$algo -b $SPEEDUP_LOG_SIZE -e $SPEEDUP_LOG_SIZE -t $((2 * c + 2)) -d $type -g random -v vector -c -m ${MACHINE} -i "${TAB}meminterleaved=0" >> ${OUTPUT_FILE}.txt 2>> ${OUTPUT_FILE}.err
                echo "Finished: algorithm=" $algo | tee ${OUTPUT_FILE}.log
            done
        done
    done
}

# Speedup of all parallel algorithms for double, uint64_t, and
# uint32_t.  Allocate array 'NumaAlignedArray' in a striped manner by
# pinning memory pages.
function speedup_test_numa {
    make -j ${TOTAL_CORES} ${PARALLEL_ALGOS[*]} VERBOSE=1

    SPEEDUP_LOG_SIZE=$1

    OUTPUT_FILE=b1_speedup_${MACHINE}_$(mydate)
    print_infos ${OUTPUT_FILE}
    for c in `seq 0 $((TOTAL_CORES - 1))`; do
        for algo in ${PARALLEL_ALGOS[*]}; do
            echo "Run speedup benchmark; algorithm=" $algo | tee ${OUTPUT_FILE}.log
            NUM_NODES=$((c / CORES_PER_SOCKET))
            for type in double uint64; do
                numactl -C 0-$c,$((TOTAL_CORES))-$((c + TOTAL_CORES)) \
                        ${BUILD_DIR}/$algo -b $SPEEDUP_LOG_SIZE -e $SPEEDUP_LOG_SIZE -t $((2 * c + 2)) -d $type -g random -v NumaAlignedArray -m ${MACHINE} -i "${TAB}meminterleaved=0" >> ${OUTPUT_FILE}.txt 2>> ${OUTPUT_FILE}.err
                echo "Finished: algorithm=" $algo | tee ${OUTPUT_FILE}.log
            done
        done
    done
}

# Speedup of all parallel algorithms for double, uint64_t, and
# uint32_t.  Allocate array 'vector' in a striped manner (implicitly
# with the parallel generator).
function speedup_test_striped {
    make -j ${TOTAL_CORES} ${PARALLEL_ALGOS[*]} VERBOSE=1

    SPEEDUP_LOG_SIZE=$1

    OUTPUT_FILE=b1_speedup_${MACHINE}_$(mydate)
    print_infos ${OUTPUT_FILE}
    for c in `seq 0 $((TOTAL_CORES - 1))`; do
        for algo in ${PARALLEL_ALGOS[*]}; do
            echo "Run speedup benchmark; algorithm=" $algo | tee ${OUTPUT_FILE}.log
            NUM_NODES=$((c / CORES_PER_SOCKET))
            for type in double uint64; do
                # Allocate memory round robin on numa nodes 0-$NUM_NODES (second value is inclusive)
                # Use cores 0-$c and cores $((TOTAL_CORES))-$((c + TOTAL_CORES)).
                # The first range are physical cores,
                # the second range are the hardware threads for hyperthreading.
                numactl -C 0-$c,$((TOTAL_CORES))-$((c + TOTAL_CORES)) \
                        ${BUILD_DIR}/$algo -b $SPEEDUP_LOG_SIZE -e $SPEEDUP_LOG_SIZE -t $((2 * c + 2)) -d $type -g random -v vector -m ${MACHINE} -i "${TAB}meminterleaved=0" >> ${OUTPUT_FILE}.txt 2>> ${OUTPUT_FILE}.err
                echo "Finished: algorithm=" $algo | tee ${OUTPUT_FILE}.log
            done
        done
    done
}

# Speedup of all parallel algorithms for double, uint64_t, and uint32_t.
# Use numactl to allocate array 'vector' round robin.
function speedup_test_interleaved {
    make -j ${TOTAL_CORES} ${PARALLEL_ALGOS[*]} VERBOSE=1

    SPEEDUP_LOG_SIZE=$1

    OUTPUT_FILE=b1_speedup_${MACHINE}_$(mydate)
    print_infos ${OUTPUT_FILE}
    for c in `seq 0 $((TOTAL_CORES - 1))`; do
        for algo in ${PARALLEL_ALGOS[*]}; do
            echo "Run speedup benchmark; algorithm=" $algo | tee ${OUTPUT_FILE}.log
            NUM_NODES=$((c / CORES_PER_SOCKET))
            for type in double uint64; do
                # Allocate memory round robin on numa nodes 0-$NUM_NODES (second value is inclusive)
                # Use cores 0-$c and cores $((TOTAL_CORES))-$((c + TOTAL_CORES)).
                # The first range are physical cores,
                # the second range are the hardware threads for hyperthreading.
                numactl -i 0-$NUM_NODES -C 0-$c,$((TOTAL_CORES))-$((c + TOTAL_CORES)) \
                        ${BUILD_DIR}/$algo -b $SPEEDUP_LOG_SIZE -e $SPEEDUP_LOG_SIZE -t $((2 * c + 2)) -d $type -g random -v vector -m ${MACHINE} -i "${TAB}meminterleaved=1" >> ${OUTPUT_FILE}.txt 2>> ${OUTPUT_FILE}.err
                echo "Finished: algorithm=" $algo | tee ${OUTPUT_FILE}.log
            done
        done
    done
}

# Parallel algorithms on all sockets with all datatypes and
# generators.  Allocate array 'vector' on master thread's NUMA node
# (by copying the data into a new array with the master thread)
function parallel_test_copyback {
    make -j ${TOTAL_CORES} ${PARALLEL_ALGOS[*]} VERBOSE=1

    MIN_LOG_PARALLEL=$1
    MAX_LOG_PARALLEL=$2

    OUTPUT_FILE=b1_parallel_${MACHINE}_$(mydate)
    print_infos ${OUTPUT_FILE}
    for algo in ${PARALLEL_ALGOS[*]}; do
        echo "Run parallel benchmark; algorithm=" $algo | tee ${OUTPUT_FILE}.log
        for gen in ${GENERATORS[*]}; do
            for type in ${DATATYPES[*]}; do
                ${BUILD_DIR}/$algo -b $MIN_LOG_PARALLEL -e $MAX_LOG_PARALLEL -t $((2 * TOTAL_CORES)) -g $gen -d $type -v vector -c -m ${MACHINE} -i "${TAB}meminterleaved=0" >> ${OUTPUT_FILE}.txt 2>> ${OUTPUT_FILE}.err
                echo "Finished: algorithm=" $algo | tee ${OUTPUT_FILE}.log
            done
        done
    done
}

# Parallel algorithms on all sockets with all datatypes and
# generators.  Allocate array 'NumaAlignedArray' in a striped manner
# by pinning the memory pages.
function parallel_test_numa {
    make -j ${TOTAL_CORES} ${PARALLEL_ALGOS[*]} VERBOSE=1

    MIN_LOG_PARALLEL=$1
    MAX_LOG_PARALLEL=$2

    OUTPUT_FILE=b1_parallel_${MACHINE}_$(mydate)
    print_infos ${OUTPUT_FILE}
    for algo in ${PARALLEL_ALGOS[*]}; do
        echo "Run parallel benchmark; algorithm=" $algo | tee ${OUTPUT_FILE}.log
        for gen in ${GENERATORS[*]}; do
            for type in ${DATATYPES[*]}; do
                ${BUILD_DIR}/$algo -b $MIN_LOG_PARALLEL -e $MAX_LOG_PARALLEL -t $((2 * TOTAL_CORES)) -g $gen -d $type -v NumaAlignedArray -m ${MACHINE} -i "${TAB}meminterleaved=0" >> ${OUTPUT_FILE}.txt 2>> ${OUTPUT_FILE}.err
                echo "Finished: algorithm=" $algo | tee ${OUTPUT_FILE}.log
            done
        done
    done
}

# Parallel algorithms on all sockets with all datatypes and
# generators.  Allocate array 'vector' in a striped manner (implicitly
# with the parallel generator)
function parallel_test_striped {
    make -j ${TOTAL_CORES} ${PARALLEL_ALGOS[*]} VERBOSE=1

    MIN_LOG_PARALLEL=$1
    MAX_LOG_PARALLEL=$2

    OUTPUT_FILE=b1_parallel_${MACHINE}_$(mydate)
    print_infos ${OUTPUT_FILE}
    for algo in ${PARALLEL_ALGOS[*]}; do
        echo "Run parallel benchmark; algorithm=" $algo | tee ${OUTPUT_FILE}.log
        for gen in ${GENERATORS[*]}; do
            for type in ${DATATYPES[*]}; do
                ${BUILD_DIR}/$algo -b $MIN_LOG_PARALLEL -e $MAX_LOG_PARALLEL -t $((2 * TOTAL_CORES)) -g $gen -d $type -v vector -m ${MACHINE} -i "${TAB}meminterleaved=0" >> ${OUTPUT_FILE}.txt 2>> ${OUTPUT_FILE}.err
                echo "Finished: algorithm=" $algo | tee ${OUTPUT_FILE}.log
            done
        done
    done
}

# Parallel algorithms on all sockets with all datatypes and generators.
# Use numactl to allocate array 'vector' round robin
function parallel_test_interleaved {
    make -j ${TOTAL_CORES} ${PARALLEL_ALGOS[*]} VERBOSE=1

    MIN_LOG_PARALLEL=$1
    MAX_LOG_PARALLEL=$2
    
    OUTPUT_FILE=b1_parallel_${MACHINE}_$(mydate)
    print_infos ${OUTPUT_FILE}
    for algo in ${PARALLEL_ALGOS[*]}; do
        echo "Run parallel benchmark; algorithm=" $algo | tee ${OUTPUT_FILE}.log
        for gen in ${GENERATORS[*]}; do
            for type in ${DATATYPES[*]}; do
                numactl -i all ${BUILD_DIR}/$algo -b $MIN_LOG_PARALLEL -e $MAX_LOG_PARALLEL -t $((2 * TOTAL_CORES)) -g $gen -d $type -v vector -m ${MACHINE} -i "${TAB}meminterleaved=1" >> ${OUTPUT_FILE}.txt 2>> ${OUTPUT_FILE}.err
                echo "Finished: algorithm=" $algo | tee ${OUTPUT_FILE}.log
            done
        done
    done
}

# Sequential algorithms with all datatypes and generators.
function sequential_test {
    make -j ${TOTAL_CORES} ${SEQUENTIAL_ALGOS[*]} VERBOSE=1

    MIN_LOG_SEQUENTIAL=$1
    MAX_LOG_SEQUENTIAL=$2

    OUTPUT_FILE=b1_sequential_${MACHINE}_$(mydate)
    print_infos ${OUTPUT_FILE}
    for algo in ${SEQUENTIAL_ALGOS[*]}; do
        echo "Run sequential benchmark; algorithm=" $algo | tee ${OUTPUT_FILE}.log
        for gen in ${GENERATORS[*]}; do
            for type in ${DATATYPES[*]}; do
                ${BUILD_DIR}/$algo -b $MIN_LOG_SEQUENTIAL -e $MAX_LOG_SEQUENTIAL -t 1 -g $gen -d $type -v vector -m ${MACHINE} -i "${TAB}meminterleaved=0" >> ${OUTPUT_FILE}.txt 2>> ${OUTPUT_FILE}.err
                echo "Finished: algorithm=" $algo | tee ${OUTPUT_FILE}.log
            done
        done
    done
}
