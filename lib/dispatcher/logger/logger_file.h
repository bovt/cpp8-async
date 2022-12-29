#pragma once
#include "logger/ilogger.h"
#include <atomic>
#include <boost/format.hpp> 
#include <condition_variable>  
#include <cstring> 
#include <filesystem>
#include <fstream>
#include <string> 
#include <time.h>
#include <thread>

class LoggerFile final : public ILogger {
public:
    LoggerFile() {
        m_Thread = CreateThread();
    }

    void Exit() override {
        m_Execute.store(false);
        {
            std::unique_lock<std::mutex> locker(m_MutexThread);
            m_Condition.notify_one();
        }

        if (m_Thread.joinable())
            m_Thread.join();
    };

    std::string GetType() const override {
        return "file";
    }
private:
    std::condition_variable m_Condition;
    std::mutex m_MutexThread;
    ConcurentDeque<std::string> m_Deque;
    std::atomic<bool> m_Execute{ true };
    std::thread m_Thread;
   
    void PrintFunc() {
        while (m_Execute || !m_Deque.empty()) {
            if (m_Deque.empty()) {
                std::unique_lock<std::mutex> locker(m_MutexThread);
                m_Condition.wait(locker);
            }

            if (m_Deque.empty())
                continue;

            auto head = m_Deque.front();
            SaveLogInFile(head);
            m_Deque.pop_front();
        }
    }

    void SaveLogInFile(std::string aStr) {
        auto createTime = time(nullptr);
        auto bulkTime = createTime;
        auto id = std::this_thread::get_id();
        for (int i= 0; i <= 1000; ++i) {
            std::string filename = boost::str(boost::format("bulk_%1%_id[%2%]_%3%.txt")%createTime%id%i);
            std::cout << "filename=" << filename << std::endl;
            if (std::filesystem::exists(filename))
                continue;
            std::cout << "filename=" << filename << std::endl;
            std::ofstream file(filename);
            if (file.is_open()) {
                file << aStr;
                return;
            }
        }
    }

    void SaveLog(std::string aStr) override {
        m_Deque.push_back(aStr);
        std::unique_lock<std::mutex> locker(m_MutexThread);
        m_Condition.notify_one();
    }

    std::thread CreateThread() {
        return std::thread(&LoggerFile::PrintFunc, this);
    }
};

