**Stress testing application**

This is a simple stress testing application.

It compare the solutions in ans.cpp and check.cpp and report the running time.

It also reports errors e.g. TLE/RTE/MLE.

Requires C++17 (for `<filesystem>`),`boost::process`.

INSTALLATION:

Run `g++ (or other compiler) tester.cpp -o tester`.

USAGE:

The program accepts three arguments: `./tester nt wt ml`, with `nt` being the number of tests, `wt` being the time limit (in ms), `ml` being the memory limit in MB (not MiB!). `ml` only counts the resident set size, so programs can allocate a lot of memory but if they're mostly unused pages, the reported memory usage can be small. `nt` defaults to 100, `wt` defaults to 1000 and `ml` defaults to 256 (can be changed in the code).

