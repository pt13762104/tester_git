#include <random>
#include <iostream>
#define randdist std::uniform_int_distribution<std::mt19937_64::result_type>
std::random_device dev;
std::mt19937 rng(dev());
using namespace std;
int main(int argc, char **argv)
{
    // add your test format
    // argv[1] is the testcase number
    int testnum = stoi(argv[1]);
}