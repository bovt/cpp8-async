#pragma once       
#include "logger/logger_screen.h"
#include "logger/logger_file.h"
#include <string> 
/**
* \brief  класс агрегатор для логгеров
* \details Screen logger яв-ся обязательным 
*/
class CmdLogger {
    using logger_t = std::unique_ptr<ILogger>;
public:
    /**
    * \brief  конструктор класса агрегатора для логгеров
    * \details ScreenLogger - обязательный, кол-во FileLogger - опционально
    * \param[in] aBulkSize - размер статической очереди
    */
    CmdLogger(std::ostream& aStream, const std::size_t aFileLoggersNumber, std::shared_ptr<std::mutex>& aMutex) : m_Stream(aStream), m_ScreenMutex(aMutex) {
        m_LoggerScreen = std::make_unique<LoggerScreen>(aStream, m_ScreenMutex);
        for (auto i = 0U; i < aFileLoggersNumber; ++i)
            m_FileLoggers.emplace_back(std::make_unique<LoggerFile>());
    }

    void Save(const std::string aStr, const std::size_t aCommandsNum) {
        ++m_CounterBulk;
        m_CounterCommands += aCommandsNum;
        if (m_LoggerScreen)
            m_LoggerScreen->Save(aStr, aCommandsNum);

        if (!m_FileLoggers.empty()) {
            const std::size_t id = m_CounterBulk % m_FileLoggers.size();
            SaveInFile(m_FileLoggers[id], aStr, aCommandsNum);
        }
    }

    void Exit() {
        if (m_LoggerScreen)
            m_LoggerScreen->Exit();
        
        for (auto& logger : m_FileLoggers) {
            if (logger)
                logger->Exit();
        }
    }

    void PrintStat(const std::size_t aLines, const std::size_t aBulks, const std::size_t aCommands) {
        {
            if (!m_ScreenMutex)
                return;
            std::lock_guard<std::mutex> lockPrint(*m_ScreenMutex);
            m_Stream << boost::format("main stream: %1% lines, %2% blocks, %3% commands") % aLines % aBulks % aCommands << std::endl;
        }
        PrintStat("log thread", m_LoggerScreen);

        std::size_t counter = 1;
        for (auto& logger : m_FileLoggers) {
            PrintStat(boost::str(boost::format("%1%_%2% thread")% logger->GetType() % counter), logger);
            ++counter;
        }
    }
private:
    CmdLogger() = default;
    std::shared_ptr<std::mutex> m_ScreenMutex = std::make_shared<std::mutex>();
    logger_t   m_LoggerScreen;
    std::vector<logger_t> m_FileLoggers;
    std::size_t m_CounterBulk = 0;
    std::size_t m_CounterCommands = 0;
    std::ostream& m_Stream;

    void SaveInFile(std::unique_ptr<ILogger>& aFileLogger, const std::string& aStr, const std::size_t aCommandsNum) {
        if (aFileLogger)
            aFileLogger->Save(aStr, aCommandsNum);
    }

    void PrintStat(const std::string aThrName, std::unique_ptr<ILogger>& aLogger) {
        if (!aLogger)
            return;

        const auto stat = aLogger->GetStat();
        if (!m_ScreenMutex)
            return;
        std::lock_guard<std::mutex> lockPrint(*m_ScreenMutex);
        m_Stream << boost::format("%1%: %2% - blocks, %3% - commands") % aThrName % stat.Bulks % stat.Commands << std::endl;
    }
};
