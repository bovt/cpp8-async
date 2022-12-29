#pragma once
#include "command/icommand.h"
#include "logger/logger.h"
#include <iostream> 
/**
* \brief   простейшая имплементация интерфейса IMyCommand
* \details при исполнение возвращает свое имя
*/
class SimpleCommand final : public IMyCommand {
public:
    SimpleCommand(const log_text_t&& aName) : m_Name(std::move(aName)){}
    SimpleCommand(const log_text_t aName  ) : m_Name(aName) {}

    log_text_t Execute() override {
        return m_Name;
    }

    virtual ~SimpleCommand() = default;
private:
    SimpleCommand() = default;

    const log_text_t m_Name = "DefaultName";
};

