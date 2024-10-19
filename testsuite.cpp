#include <fstream>
#include <chrono>
#include <thread>
#include <boost/process.hpp>
#include <filesystem>
const int wt = 1000;
const int ml = 1048576;
#define res "1"
struct status
{
    // 0=OK, 1=TLE, 2=MLE, 3=RTE
    int status;
    // in ms
    int time;
    // in kB
    int memory;
};
std::chrono::high_resolution_clock Clock;
int process_mem_usage(int id)
{
    std::ifstream stat_stream("/proc/" + std::to_string(id) + "/stat", std::ios_base::in);
    std::string pid, comm, state, ppid, pgrp, session, tty_nr;
    std::string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    std::string utime, stime, cutime, cstime, priority, nice;
    std::string O, itrealvalue, starttime, vsize;
    int rss;
    stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >> stime >> cutime >> cstime >> priority >> nice >> O >> itrealvalue >> starttime >> vsize >> rss;
    stat_stream.close();
    int page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024;
    return rss * page_size_kb;
}
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
status generate(int i)
{
    boost::process::child z("./gen " + std::to_string(i), boost::process::std_out > "test.inp");
    auto t1 = Clock.now(), t2 = Clock.now();
    int x = 0;
    while (std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() <= wt && z.running() && x <= ml)
    {
        t2 = Clock.now();
        x = std::max(x, process_mem_usage(z.id()));
    }
    int t = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    status s{0, t, x};
    if (z.running())
    {
        z.terminate();
        if (t > wt)
            s.status = 1;
        else
            s.status = 2;
    }
    else if (z.exit_code())
        s.status = 3;
    z.wait();
    return s;
}
std::pair<int, int> compile_ans()
{
    std::chrono::steady_clock::time_point startc = std::chrono::steady_clock::now();
    boost::process::child x("g++ -O2 -march=native ans.cpp -o ans");
    x.wait();
    std::chrono::steady_clock::time_point endc = std::chrono::steady_clock::now();
    int c_time = std::chrono::duration_cast<std::chrono::milliseconds>(endc - startc).count();
    return {x.exit_code(), c_time};
}
status ans()
{
    boost::process::child z("./ans", boost::process::std_in < "test.inp", boost::process::std_out > "base.out");
    auto t1 = Clock.now(), t2 = Clock.now();
    int x = 0;
    while (std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() <= wt && z.running() && x <= ml)
    {
        t2 = Clock.now();
        x = std::max(x, process_mem_usage(z.id()));
    }
    int t = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    status s{0, t, x};
    if (z.running())
    {
        z.terminate();
        if (t > wt)
            s.status = 1;
        else
            s.status = 2;
    }
    else if (z.exit_code())
        s.status = 3;
    z.wait();
    return s;
}
std::pair<int, int> compile_check()
{
    std::chrono::steady_clock::time_point startc = std::chrono::steady_clock::now();
    boost::process::child x("g++ -O2 -march=native check.cpp -o check");
    x.wait();
    std::chrono::steady_clock::time_point endc = std::chrono::steady_clock::now();
    int c_time = std::chrono::duration_cast<std::chrono::milliseconds>(endc - startc).count();
    return {x.exit_code(), c_time};
}
status check()
{
    boost::process::child z("./check", boost::process::std_in < "test.inp", boost::process::std_out > "test.out");
    auto t1 = Clock.now(), t2 = Clock.now();
    int x = 0;
    while (std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() <= wt && z.running() && x <= ml)
    {
        t2 = Clock.now();
        x = std::max(x, process_mem_usage(z.id()));
    }
    int t = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    status s{0, t, x};
    if (z.running())
    {
        z.terminate();
        if (t > wt)
            s.status = 1;
        else
            s.status = 2;
    }
    else if (z.exit_code())
        s.status = 3;
    z.wait();
    return s;
}
void test(int testcase)
{
    std::ofstream cout("res" res ".out");
    std::filesystem::remove_all("wa_tests");
    std::filesystem::create_directories("wa_tests");
    std::pair<int, int> c1 = compile_generator();
    std::pair<int, int> c2 = compile_ans();
    std::pair<int, int> c3 = compile_check();
    int AC = 0;
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
        std::filesystem::remove("test.inp");
        std::filesystem::remove("test.out");
        std::filesystem::remove("base.out");
        cout << "Test #" << i << ":" << std::endl;
        status a1 = generate(i);
        cout << "Generator: ";
        if (a1.status == 0)
            cout << "OK [" << a1.time << " ms, " << a1.memory / 1000.0 << " MB]" << std::endl;
        else if (a1.status == 1)
            cout << "TLE [>" << wt << " ms, " << a1.memory / 1000.0 << " MB]" << std::endl;
        else if (a1.status == 2)
            cout << "MLE [" << a1.time << " ms, >" << ml / 1000.0 << " MB]" << std::endl;
        else
            cout << "RTE [" << a1.time << " ms, " << a1.memory / 1000.0 << " MB]" << std::endl;
        if (a1.status == 0)
        {
            status a_a = ans();
            status a_t = check();
            cout << "Base: ";
            if (a_a.status == 0)
                cout << "OK [" << a_a.time << " ms, " << a_a.memory / 1000.0 << " MB]" << std::endl;
            else if (a_a.status == 1)
                cout << "TLE [>" << wt << " ms, " << a_a.memory / 1000.0 << " MB]" << std::endl;
            else if (a_a.status == 2)
                cout << "MLE [" << a_a.time << " ms, >" << ml / 1000.0 << " MB]" << std::endl;
            else
                cout << "RTE [" << a_a.time << " ms, " << a_a.memory / 1000.0 << " MB]" << std::endl;
            cout << "Test: ";
            if (a_t.status == 0)
                cout << "OK [" << a_t.time << " ms, " << a_t.memory / 1000.0 << " MB]" << std::endl;
            else if (a_t.status == 1)
                cout << "TLE [>" << wt << " ms, " << a_t.memory / 1000.0 << " MB]" << std::endl;
            else if (a_t.status == 2)
                cout << "MLE [" << a_t.time << " ms, >" << ml / 1000.0 << " MB]" << std::endl;
            else
                cout << "RTE [" << a_t.time << " ms, " << a_t.memory / 1000.0 << " MB]" << std::endl;
            if (a_a.status == 0 && a_t.status == 0)
            {
                cout << "Status: ";
                if (compareFiles("base.out", "test.out"))
                {
                    cout << "AC" << std::endl;
                    AC++;
                }
                else
                {
                    cout << "WA" << std::endl;
                    std::filesystem::copy("test.inp", "wa_tests/test_wa_" + std::to_string(i) + ".inp");
                    std::filesystem::copy("base.out", "wa_tests/base_wa_" + std::to_string(i) + ".out");
                    std::filesystem::copy("test.out", "wa_tests/test_wa_" + std::to_string(i) + ".out");
                }
            }
        }
    }
    std::filesystem::remove("test.inp");
    std::filesystem::remove("test.out");
    std::filesystem::remove("base.out");
    std::filesystem::remove("ans");
    std::filesystem::remove("check");
    std::filesystem::remove("gen");
    cout << "AC: " << AC << "/" << testcase << " tests." << std::endl;
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