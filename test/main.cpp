#include "lib/bulk_async.h"
#include <algorithm>   
#include <chrono>          
#include <set>
#include <sstream>
#include <thread>
#include <vector>

void FillSet(std::set<std::string>& aSet, std::stringstream&  aSS) {
    std::string str;
    while (std::getline(aSS, str)) {
        aSet.insert(str);
    }
}

void ThreadFunc() {
    std::string testCmd = "cmd1";
    std::stringstream ss;
    auto handl1 = async::connect(3, ss);
    auto handl2 = async::connect(2, ss);
    async::receive(handl1, testCmd.data(), testCmd.size());
    async::receive(handl1, testCmd.data(), testCmd.size());

    async::receive(handl2, testCmd.data(), testCmd.size());
    async::receive(handl2, testCmd.data(), testCmd.size());
    async::receive(handl2, testCmd.data(), testCmd.size());
    async::receive(handl2, testCmd.data(), testCmd.size());

    async::receive(handl1, testCmd.data(), testCmd.size());
    async::receive(handl1, testCmd.data(), testCmd.size());
    async::disconnect(handl1);

    async::receive(handl2, testCmd.data(), testCmd.size());
    async::receive(handl2, testCmd.data(), testCmd.size());
    async::receive(handl2, testCmd.data(), testCmd.size());
    async::receive(handl2, testCmd.data(), testCmd.size());
    async::disconnect(handl2);
    
    std::stringstream expectedSS;
    expectedSS << "bulk: cmd1, cmd1"                              << std::endl;
    expectedSS << "bulk: cmd1, cmd1"                              << std::endl;
    expectedSS << "bulk: cmd1, cmd1, cmd1"                        << std::endl;
    expectedSS << "bulk: cmd1"                                    << std::endl;
    expectedSS << "main stream: 4 lines, 2 blocks, 4 commands"    << std::endl;
    expectedSS << "log thread: 2 - blocks, 4 - commands"          << std::endl;
    expectedSS << "file_1 thread: 1 - blocks, 1 - commands"       << std::endl;
    expectedSS << "file_2 thread: 1 - blocks, 3 - commands"       << std::endl;
    expectedSS << "bulk: cmd1, cmd1"                              << std::endl;
    expectedSS << "bulk: cmd1, cmd1"                              << std::endl;
    expectedSS << "main stream: 8 lines, 4 blocks, 8 commands"     << std::endl;
    expectedSS << "log thread: 4 - blocks, 8 - commands"           << std::endl;
    expectedSS << "file_1 thread: 2 - blocks, 4 - commands"        << std::endl;
    expectedSS << "file_2 thread: 2 - blocks, 4 - commands"        << std::endl;

    std::set<std::string> set1;
    std::set<std::string> set2;
    FillSet(set1, ss);
    FillSet(set2, expectedSS);
    bool res = set1 == set2;
}

int main() {
    //—оздание объектов из разных потоков, должно быть 42 bulk суммарно (42 файла)
    std::vector<std::thread> threads;
    for (int i = 0; i <= 6; ++i) {
        threads.emplace_back(std::thread{ []() { ThreadFunc(); }});
    }
    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}