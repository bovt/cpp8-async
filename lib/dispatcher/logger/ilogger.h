#pragma once
#include <cctype>
/**
* \brief  Интерфейс классов логгеров
*/
class ILogger {
    /**
    * \brief  статистика поступвхиших команд в логгер
    */
    struct Stat {
        std::size_t Commands = 0;
        std::size_t Bulks = 0;

        /**
        * \brief  добавить команд в статистику логгера
        * \details кол-во блоков увеличится на 1, кол во команд на aCommands
        * \param[in] aCommands - кол-во добавленных команд
        */
        void Add(const std::size_t aCommands) {
            Commands += aCommands;
            Bulks += 1;
        }
    };
public:
    /**
    * \brief  сохранение лога
    * \details в зависимости от реализации логгера будет произведено сохранение лога
    * \param[in] aStr - лог полученный в рез-те обработки команд
    * \param[in] aCommands - кол-во команд, обработаных
    */
    void Save(std::string aStr, const std::size_t aCommands) {
        m_Statistic.Add(aCommands);
        SaveLog(aStr);
    }

    /**
    * \brief  Освобождение ресурсов логгера
    * \details Освобождение ресурсов логгера, дальнейшая полезная работа лога осуществлятсья не будет
    */
    virtual void Exit() = 0;

    virtual ~ILogger() {}


    /**
    * \brief  Получить статистику логгера
    * \details вернет кол-во блоков и кол-во команд
    */
    const Stat GetStat() {
        return m_Statistic;
    }

    /**
    * \brief  Получить название типа логгера
    */
    virtual std::string GetType() const = 0;
private:
    /**
    * \brief  сохранение лога
    * \details в зависимости от реализации логгера будет произведено сохранение лога
    * \param[in] aStr - лог полученный в рез-те обработки команд
    */
    virtual void SaveLog(std::string aStr) = 0;
    
    /**
    * \brief  статистика логгера
    */
    Stat m_Statistic = {};
};

