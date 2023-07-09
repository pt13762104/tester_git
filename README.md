**Stress testing application**

This is a simple stress testing application.

It compare the solutions in ans.cpp and check.cpp and report the running time.

It also reports errors e.g. TLE/RTE. MLE or other errors are not supported.

Requires C++17 (for `<filesystem>`),`boost::process`.

INSTALLATION:

Run `g++ (or other compiler) testsuite.cpp -o testsuite`.

USAGE:

If you want to change the time limit, change the line `#define wt 1000` to other limits (2000 is 2s,etc...). Then recompile.

Run `./testsuite` to run 100 tests by default. If you want to change the number of tests, run `./testsuite x`, with x beign the number of tests.