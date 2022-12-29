#pragma once
#include <cctype>
#include <iostream>
/*! \mainpage Otus_11 (BulkMT_ASYNC)
 *
 * \r\n  ƒиспетчер задач (многопоточный асинхронный)
 *
 */
namespace async {
    using handle_t = std::uint64_t;

    /**
    * \brief  создание обработчика команд
    * \param[in] bulk - кол-во команд в куче
    * \param[in] aOutStream - поток вывода лога
    * \return id обработчика (если 0 - то логгер не создан)
    */
    handle_t connect(std::size_t bulk, std::ostream& aOutStream = std::cout);

    /**
    * \brief  обработка команды
    * \param[in] handle - id обработчика
    * \param[in] data - указатель на начало буффера
    * \param[in] size - размер буффера
    */
    void receive(handle_t handle, const char *data, std::size_t size);

    /**
    * \brief  уничтожение обработчика
    * \param[in] handle - id обработчика
    */
    void disconnect(handle_t handle);
}