#pragma once
#include <string> 
/**
* \brief  Интерфейс команды
*/
class IMyCommand {
protected:
    using log_text_t = std::string;
public:
    
    /**
    * \brief  Выполнить команду
    * \return log_text_t - лог результатов выполнения команды
    */
    virtual log_text_t Execute() = 0;

    virtual ~IMyCommand() = default;
};
