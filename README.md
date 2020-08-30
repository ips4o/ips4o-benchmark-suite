# In-place Parallel Super Scalar Samplesort (IPS⁴o) Benchmark Suite

This is the benchmark suite of IPS⁴o presented in the paper [In-place Parallel Super Scalar Samplesort (IPS⁴o)](https://arxiv.org/abs/1705.02257) (todo update link).
The benchmark suite compares our algorithms IPS⁴o [(GitHub link)](https://github.com/ips4o/ips4o), In-place Parallel Super Scalar Radix Sort (IPS²Ra) [(GitHub link)](https://github.com/ips4o/ips2ra), and Parallel Super Scalar Samplesort (PS⁴o) [(GitHub link)](https://github.com/ips4o/ps4o), to various sequential and parallel radix as well as comparison based sorting algorithms.
Here's the abstract:

(todo update)
> We present a sorting algorithm that works in-place, executes in parallel, is
> cache-efficient, avoids branch-mispredictions, and performs work O(n log n) for
> arbitrary inputs with high probability. The main algorithmic contributions are
> new ways to make distribution-based algorithms in-place: On the practical side,
> by using coarse-grained block-based permutations, and on the theoretical side,
> we show how to eliminate the recursion stack. Extensive experiments show that
> our algorithm IPS⁴o scales well on a variety of multi-core machines. We
> outperform our closest in-place competitor by a factor of up to 3. Even as
> a sequential algorithm, we are up to 1.5 times faster than the closest
> sequential competitor, BlockQuicksort.

## Results

You compile the figures used in our publication by invoking the script `benchmark/compile_figures.sh`.
This command compiles LaTeX files stored in the folders [benchmark/distributions](benchmark/distributions) and [benchmark/running_times](benchmark/running_times).
After compilation, take a look at the generated PDF files.

The measurements are stored in [benchmark/running_times/results.7z](benchmark/running_times/results.7z).
To reinsert the results into the LaTeX files in [benchmark/running_timers](benchmark/running_timers), you have to decompress the measurements and analyze these using the data series processing tool [SqlPlotTools](https://github.com/ips4o/sqlplot-tools.git) published by [Timo Bingmann](https://algo2.iti.kit.edu/bingmann.php).
To do so, just call the script [benchmark/analyze_data.sh](benchmark/analyze_data.sh).
The script requires an installation of the SqlPlotTools connected to a PostgreSQL database and the file archiver 7-Zip.

## Usage

```
git clone https://github.com/ips4o/ips4o-benchmark-suite.git
cd ips4o-benchmark-suite
git submodule update --recursive --init
mkdir build
cd build
cmake ..
make all 
# make benchmark_ips4o benchmark_ips4ooldparallel benchmark_ips4oparallel benchmark_ips2ra benchmark_ips2raparallel
```

The executables `benchmark_ips4o` and `benchmark_ips4oparallel` execute the sequential/parallel version of IPS⁴o.
The executables `benchmark_ips2ra` and `benchmark_ips2raparallel` execute the sequential/parallel version of IPS²Ra.
For the usage of the benchmarks, we refer to the help function, e.g., `./benchmark_ips4o --help`.
Besides IPS⁴o and IPS²Ra, the benchmark suite contains many more sorting algorithms.
The benchmark `benchmark_ippradixsort` is disabled by default as it requires an installation of the [Intel® Integrated Performance Primitives (IPP)](https://software.intel.com/content/www/us/en/develop/tools/integrated-performance-primitives.html).
If you have installed IPP, you may enable the CMake property `IPPRADIXSORT`, e.g., via `ccmake`.
Some sorting algorithms require a compiler supporting the Cilk Plus C++ language extension.
Thus, make sure that your compiler supports Cilk Plus out of the box or add your own Cilk Plus library to the [CMakeLists.txt](CMakeLists.txt) file.
The sorting algorithms are loaded as submodules into the folder [extern](extern).
The file [authors](authors) lists the authors of the source code used in this benchmark suite.
Additionally, the file specifies for each submodule the url, authors, and license.

### Sequential Algorithms

The target names start with the prefix `benchmark_`.

Target Name | Name in Paper | Source | Note
--- | --- | --- | ---
benchmark_ips4o | I1S⁴o| [GitHub link](https://github.com/ips4o/ips4o) | Release Version
benchmark_ips2ra | I1S²ra | [GitHub link](https://github.com/ips4o/ips2ra)
benchmark_ps4o |1S⁴o | [GitHub link](https://github.com/ips4o/ps4o)
benchmark_skasort | SkaSort | [GitHub link](https://github.com/skarupke/ska_sort) |
benchmark_yaroslavskiy | DualPivot | [GitHub link](https://github.com/weissan/BlockQuicksort) |
benchmark_blockquicksort | BlockQ | [GitHub link](https://github.com/weissan/BlockQuicksort) |
benchmark_pdqsort | BlockPDQ | [GitHub link](https://github.com/orlp/pdqsort) |
benchmark_wikisort | WikiSort | [GitHub link](https://github.com/BonzaiThePenguin/WikiSort)
benchmark_timsort | Timsort | [GitHub link](https://github.com/timsort/cpp-TimSort) |
benchmark_stdsort | std::sort | GCC STL library |
benchmark_ssss | S⁴oS | [GitHub link](https://github.com/lorenzhs/ssssort) |
benchmark_quickxsort | QMsort | [GitHub link](https://github.com/weissan/QuickXsort) |
benchmark_ippradixsort | ippradix | [Website](https://software.intel.com/content/www/us/en/develop/tools/integrated-performance-primitives.html) |
benchmark_learnedsort | LearnedSort | [GitHub link](https://github.com/learnedsystems/LearnedSort) |

### Parallel Algorithms

The target names start with the prefix `benchmark_`.

Target Name | Name in Paper | Source | Note
--- | --- | --- | ---
ips4oparallel | IPS⁴o| [GitHub link](https://github.com/ips4o/ips4o) | Release Version
ips2raparallel | IPS²ra| [GitHub link](https://github.com/ips4o/ips2ra)
ps4oparallel |PS⁴o | [GitHub link](https://github.com/ips4o/ps4o)
ips4ooldparallel |IPS⁴oNT | [GitHub link](https://github.com/SaschaWitt/ips4o) | Previous Version
aspasparallel | ASPaS| [GitHub link](https://github.com/vtsynergy/aspas_sort)
raduls | RADULS2 | [GitHub link](https://github.com/refresh-bio/RADULS)
mcstlbq | MCSTLbq | GCC STL library | See also [MCSTL](https://algo2.iti.kit.edu/singler/mcstl/)
mcstlubq |MCSTLubq | GCC STL library | See also [MCSTL](https://algo2.iti.kit.edu/singler/mcstl/)
mcstlmwm | MCSTLmwm | GCC STL library | See also [MCSTL](https://algo2.iti.kit.edu/singler/mcstl/)
tbbparallelsort |TBB | [GitHub link](https://github.com/wjakob/tbb)
regionsort | RegionSort | [GitHub link](https://github.com/ips4o/parallel-inplace-radixsort)
pbbsradixsort | PBBS | [Website](http://www.cs.cmu.edu/~pbbs/) | See also [our GitHub fork](https://github.com/ips4o/pbbs-sorting)
pbbssamplesort | PBBR | [Website](http://www.cs.cmu.edu/~pbbs/) | See also [our GitHub fork](https://github.com/ips4o/pbbs-sorting)
imsdradix | IMSDradix | [Website](www.cs.columbia.edu/~orestis/) | See also [our GitHub fork](https://github.com/ips4o/InPlaceMSDradixsort)


## Reproducablity

The benchmarks have been executed on four machines.
On each machine, the benchmarks have been executed with

```
git clone https://github.com/ips4o/ips4o-benchmark-suite.git
cd ips4o-benchmark-suite
git submodule update --recursive --init
cd ./benchmark/running_times
./run.sh <machine-name>
```

in the folder [benchmark/running_times](benchmark/running_times).
After the benchmarks were executed, we analyzed the measurements with the script [benchmark/analyze_data.sh](benchmark/analyze_data.sh).
The script loads the measurements into a database and inserts the results into LaTeX files stored in the folder [benchmark/running_times](benchmark/running_times).
The script requires an installation of the [SqlPlotTools](https://github.com/ips4o/sqlplot-tools.git) published by [Timo Bingmann](https://algo2.iti.kit.edu/bingmann.php) connected to a PostgreSQL database and the file archiver 7-Zip.
The script expects the machine names `i10pc132`, `i10pc133`, `i10pc135`, and `i10pc136`.
The LaTeX files are included unmodified in the paper [In-place Parallel Super Scalar Samplesort (IPS⁴o)](https://arxiv.org/abs/1705.02257).
You may create PDF files for the figures of the paper by compiling the `*_plot.tex` files in [benchmark/running_times](benchmark/running_times) with `pdflatex` or by invoking the script [benchmarks/compile_figures.sh](benchmarks/compile_figures.sh).

For the `run.sh` command, you need an installation of the [Intel® Integrated Performance Primitives (IPP)](https://software.intel.com/content/www/us/en/develop/tools/integrated-performance-primitives.html) as well as Cilk Plus.
For Cilk Plus, you require a compiler supporting the Cilk Plus C++ language extension or you need provide your own Cilk Plus library which you add to the [CMakeLists.txt](CMakeLists.txt) file.


## Licensing

This benchmark suite is provided under the GPL-3.0 License described in the [LICENSE file](LICENSE).
If you use this benchmark suite in an academic setting please cite the [eponymous paper](https://arxiv.org/abs/1705.02257) (todo update) using the BibTeX entry

(todo update)
```bibtex 
@InProceedings{axtmann2017,
  author =	{Michael Axtmann and
                Sascha Witt and
                Daniel Ferizovic and
                Peter Sanders},
  title =	{{In-Place Parallel Super Scalar Samplesort (IPSSSSo)}},
  booktitle =	{25th Annual European Symposium on Algorithms (ESA 2017)},
  pages =	{9:1--9:14},
  series =	{Leibniz International Proceedings in Informatics (LIPIcs)},
  year =	{2017},
  volume =	{87},
  publisher =	{Schloss Dagstuhl--Leibniz-Zentrum fuer Informatik},
  doi =		{10.4230/LIPIcs.ESA.2017.9},
}
```
