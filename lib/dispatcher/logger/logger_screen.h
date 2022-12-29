#pragma once
#include "concurrent/concurrent_deque.h" 
#include "logger/ilogger.h"
#include <atomic>
#include <boost/format.hpp> 
#include <condition_variable>  
#include <cstring> 
#include <iostream>
#include <mutex>
#include <thread>

class LoggerScreen final : public ILogger {
public:
    LoggerScreen(std::ostream& aStream, std::shared_ptr<std::mutex> aMutex) : m_Stream(aStream), m_MutexPrint(aMutex) {
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

        m_MutexPrint.reset();
    };

    std::string GetType() const override {
        return "screen";
    }
private:   
    LoggerScreen() {};

    std::ostream& m_Stream = std::cout;
    std::condition_variable m_Condition;
    std::mutex m_MutexThread;
    std::shared_ptr<std::mutex> m_MutexPrint;
    ConcurentDeque<std::string> m_Deque;
    std::thread m_Thread;
    std::atomic<bool> m_Execute{ true };

    void PrintFunc() {
        while (m_Execute || !m_Deque.empty()) {
            if (m_Deque.empty()) {
                std::unique_lock<std::mutex> locker(m_MutexThread);
                m_Condition.wait(locker);
            }

            if (m_Deque.empty())
                continue;

            auto head = m_Deque.front();
            m_Deque.pop_front();
            {
                if (!m_MutexPrint)
                    return;
                std::lock_guard<std::mutex> lockPrint(*m_MutexPrint);
                m_Stream << boost::format("%2%") % std::this_thread::get_id() % head << std::endl;
            }
        }
        {
            if (!m_MutexPrint)
                return;
            std::lock_guard<std::mutex> lockPrint(*m_MutexPrint);
        }
    }


    void SaveLog(std::string aStr) override {
        m_Deque.push_back(aStr);
        std::unique_lock<std::mutex> locker(m_MutexThread);
        m_Condition.notify_one();
    }

    std::thread CreateThread() {
        return std::thread(&LoggerScreen::PrintFunc, this);
    }
};