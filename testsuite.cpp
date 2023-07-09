#include <fstream>
#include <chrono>
#include <thread>
#include <boost/process.hpp>
#include <filesystem>
#define wt 1000
#define res "1"
bool compareFiles(const std::string &p1, const std::string &p2)
{
    std::ifstream f1(p1, std::ifstream::binary | std::ifstream::ate);
    std::ifstream f2(p2, std::ifstream::binary | std::ifstream::ate);
    if (f1.fail() || f2.fail())
    {
        return false;
    }
    if (f1.tellg() != f2.tellg())
    {
        return false;
    }
    f1.seekg(0, std::ifstream::beg);
    f2.seekg(0, std::ifstream::beg);
    return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                      std::istreambuf_iterator<char>(),
                      std::istreambuf_iterator<char>(f2.rdbuf()));
}
std::pair<int, int> compile_generator()
{
    std::chrono::steady_clock::time_point startc = std::chrono::steady_clock::now();
    boost::process::child x("g++ gen.cpp -o gen");
    x.wait();
    std::chrono::steady_clock::time_point endc = std::chrono::steady_clock::now();
    int c_time = std::chrono::duration_cast<std::chrono::milliseconds>(endc - startc).count();
    return {x.exit_code(), c_time};
}
std::pair<int, int> generate(int i)
{
    std::chrono::steady_clock::time_point starta = std::chrono::steady_clock::now();
    boost::process::child y("./gen " + std::to_string(i), boost::process::std_out > "test.inp");
    int y_stats = 0;
    if (!y.wait_for(std::chrono::milliseconds(wt)))
    {
        y.terminate();
        y_stats = 1;
    }
    if (y.exit_code() != 0 && y_stats != 1)
    {
        y_stats = 2;
    }
    std::chrono::steady_clock::time_point enda = std::chrono::steady_clock::now();
    int a_time = std::chrono::duration_cast<std::chrono::milliseconds>(enda - starta).count();
    return {y_stats, a_time};
}
std::pair<int, int> compile_ans()
{
    std::chrono::steady_clock::time_point startc = std::chrono::steady_clock::now();
    boost::process::child x("g++ ans.cpp -o ans");
    x.wait();
    std::chrono::steady_clock::time_point endc = std::chrono::steady_clock::now();
    int c_time = std::chrono::duration_cast<std::chrono::milliseconds>(endc - startc).count();
    return {x.exit_code(), c_time};
}
std::pair<int, int> ans()
{
    std::chrono::steady_clock::time_point starta = std::chrono::steady_clock::now();
    boost::process::child y("./ans", boost::process::std_in<"test.inp", boost::process::std_out> "base.out");
    int y_stats = 0;
    if (!y.wait_for(std::chrono::milliseconds(wt)))
    {
        y.terminate();
        y_stats = 1;
    }
    if (y.exit_code() != 0 && y_stats != 1)
    {
        y_stats = 2;
    }
    std::chrono::steady_clock::time_point enda = std::chrono::steady_clock::now();
    int a_time = std::chrono::duration_cast<std::chrono::milliseconds>(enda - starta).count();
    return {y_stats, a_time};
}
std::pair<int, int> compile_check()
{
    std::chrono::steady_clock::time_point startc = std::chrono::steady_clock::now();
    boost::process::child x("g++ check.cpp -o check");
    x.wait();
    std::chrono::steady_clock::time_point endc = std::chrono::steady_clock::now();
    int c_time = std::chrono::duration_cast<std::chrono::milliseconds>(endc - startc).count();
    return {x.exit_code(), c_time};
}
std::pair<int, int> check()
{
    std::chrono::steady_clock::time_point starta = std::chrono::steady_clock::now();
    boost::process::child y("./check", boost::process::std_in<"test.inp", boost::process::std_out> "test.out");
    int y_stats = 0;
    if (!y.wait_for(std::chrono::milliseconds(wt)))
    {
        y.terminate();
        y_stats = 1;
    }
    if (y.exit_code() != 0 && y_stats != 1)
    {
        y_stats = 2;
    }
    std::chrono::steady_clock::time_point enda = std::chrono::steady_clock::now();
    int a_time = std::chrono::duration_cast<std::chrono::milliseconds>(enda - starta).count();
    return {y_stats, a_time};
}
void test(int testcase)
{
    std::ofstream cout("res" res ".out");
    std::filesystem::remove_all("wa_tests");
    std::filesystem::create_directories("wa_tests");
    std::pair<int, int> c1 = compile_generator();
    std::pair<int, int> c2 = compile_ans();
    std::pair<int, int> c3 = compile_check();
    if (c1.first || c2.first || c3.first)
    {
        cout << "CE" << std::endl;
        return;
    }
    else
    {
        cout << "Compilling generator took " << c1.second << "ms." << std::endl;
        cout << "Compilling ans took " << c2.second << "ms." << std::endl;
        cout << "Compilling check took " << c3.second << "ms." << std::endl;
    }
    for (int i = 0; i < testcase; i++)
    {
        std::pair<int, int> a1 = generate(i);
        if (a1.first)
        {
            cout << "Failed to generate testcase #" << i << "." << std::endl;
        }
        else
        {
            std::pair<int, int> a_a = ans();
            std::pair<int, int> a_t = check();
            cout << "Test #" << i << ": ";
            if (a_a.first == 1)
            {
                cout << "TLE (base)" << std::endl;
            }
            else if (a_t.first == 1)
            {
                cout << "TLE" << std::endl;
            }
            else if (a_a.first == 2 || a_t.first == 2)
            {
                cout << "RTE. this could happen when you divide by zero (FPE) or accessing out-of-bound (segfault). try to debug your app." << std::endl;
            }
            else
            {
                if (compareFiles("base.out", "test.out"))
                {
                    cout << "AC. time to generate test: " << a1.second << "ms."
                         << " time for ans: " << a_a.second << "ms. time for test: " << a_t.second << "ms" << std::endl;
                }
                else
                {
                    cout << "WA. time to generate test: " << a1.second << "ms."
                         << " time for ans: " << a_a.second << "ms. time for test: " << a_t.second << "ms" << std::endl;
                    std::filesystem::copy("test.inp", "wa_tests/test_wa_" + std::to_string(i) + ".inp");
                    std::filesystem::copy("base.out", "wa_tests/base_wa_" + std::to_string(i) + ".out");
                    std::filesystem::copy("test.out", "wa_tests/test_wa_" + std::to_string(i) + ".out");
                }
            }
        }
        std::filesystem::remove("test.inp");
        std::filesystem::remove("test.out");
        std::filesystem::remove("base.out");
    }
    std::filesystem::remove("ans");
    std::filesystem::remove("check");
    std::filesystem::remove("gen");
}
int main(int argc, char **argv)
{
    if (argv[1] == NULL)
    {
        test(100);
    }
    else
    {
        test(std::stoi(argv[1]));
    }
}