#!/bin/bash

set -x # echo on
set +e # Disable exit on non 0

TOTAL_MEMORY=$(awk '/MemTotal/{print $2}' /proc/meminfo)

ulimit -Sv $TOTAL_MEMORY

PARALLEL_ALGOS=(benchmark_ips2raparallel benchmark_ips4oparallel)
SEQUENTIAL_ALGOS=(benchmark_ips4o benchmark_ips2ra)

GENERATORS=(random)

function mydate {
    DATE=$(date '+%Y-%m-%d_%H_%M_%S')
    echo $DATE
}

CORES_PER_SOCKET=`grep "^cpu cores" /proc/cpuinfo | cut -d: -f2 | sort -n | tail -n1 | xargs`
TOTAL_CORES=$(((`grep "^physical id" /proc/cpuinfo | cut -d: -f2 | sort -n | tail -n1 | xargs` + 1) * CORES_PER_SOCKET))

TAB=$'\t'

BUILD_DIR="../../RELEASE_TIMER_${MACHINE}"

mkdir -p $BUILD_DIR
cd $BUILD_DIR
cmake .. -DIPPRADIXSORT=Enable -DIPS4O_TIMER=Enable
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

# Parallel algorithms on all sockets with all datatypes and generators.
# use numactl to allocate array 'vector' round robin.
function parallel_subroutines_test {
    make -j ${TOTAL_CORES} ${PARALLEL_ALGOS[*]} VERBOSE=1

    MIN_LOG_PARALLEL=$1
    MAX_LOG_PARALLEL=$2

    OUTPUT_FILE=b1_parallel_subroutines_${MACHINE}_$(mydate)
    print_infos ${OUTPUT_FILE}
    for algo in ${PARALLEL_ALGOS[*]}; do
        echo "Run parallel subroutines benchmark; algorithm=" $algo | tee ${OUTPUT_FILE}.log
        # Allocate memory round robin on all numa nodes
        for type in uint64; do
            numactl -i all ${BUILD_DIR}/$algo -b $MIN_LOG_PARALLEL -e $MAX_LOG_PARALLEL -t $((2 * TOTAL_CORES)) -g random -v vector -d $type -m ${MACHINE} -i "${TAB}meminterleaved=1" >> ${OUTPUT_FILE}.txt 2>> ${OUTPUT_FILE}.err
            echo "Finished: algorithm=" $algo | tee ${OUTPUT_FILE}.log
        done
    done
}

# Sequential algorithms.
function sequential_subroutines_test {
    make -j ${TOTAL_CORES} ${SEQUENTIAL_ALGOS[*]} VERBOSE=1

    MIN_LOG_SEQUENTIAL=$1
    MAX_LOG_SEQUENTIAL=$2

    OUTPUT_FILE=b1_sequential_subroutines_${MACHINE}_$(mydate)
    print_infos ${OUTPUT_FILE}
    for algo in ${SEQUENTIAL_ALGOS[*]}; do
        echo "Run sequential subroutines benchmark; algorithm=" $algo | tee ${OUTPUT_FILE}.log
        for type in uint64; do
            ${BUILD_DIR}/$algo -b $MIN_LOG_SEQUENTIAL -e $MAX_LOG_SEQUENTIAL -t 1 -g random -v vector -d $type -m ${MACHINE} >> ${OUTPUT_FILE}.txt 2>> ${OUTPUT_FILE}.err
            echo "Finished: algorithm=" $algo | tee ${OUTPUT_FILE}.log
        done
    done
}
