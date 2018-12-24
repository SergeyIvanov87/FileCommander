#ifndef EVENT_PROCESSOR_H
#define EVENT_PROCESSOR_H
#include <thread>
#include <atomic>
#include <mutex>
#include <map>
#include <condition_variable>
#include "AsyncCommand.h"
#include "SyncCommand.h"

class EventProcessor
{
public:
    using CommandId = size_t;
    EventProcessor() = default;
    ~EventProcessor();

    template<class Cmd>
    typename CommandTraits<Cmd>::CommandResultTypePtr pushCommand(std::unique_ptr<Cmd> &&cmd);

    template<class Cmd>
    typename std::enable_if<std::is_base_of<SyncCommand, Cmd>::value,
                        typename  CommandTraits<Cmd>::CommandResultTypePtr>::type
                        pushCommandImpl(std::unique_ptr<Cmd> &&cmd);
    template<class Cmd>
    typename std::enable_if<std::is_base_of<AsyncCommand, Cmd>::value,
                        typename CommandTraits<Cmd>::CommandResultTypePtr>::type
                        pushCommandImpl(std::unique_ptr<Cmd> &&cmd);

    void start();
    void stop();

    size_t getNextCmdId();
private:
    void executor();

    std::atomic<bool> m_stop;
    std::unique_ptr<std::thread> m_workerThread;

    std::mutex m_cndMutex;
    std::condition_variable m_waitCmdCondition;
    std::map<CommandId, std::unique_ptr<AsyncCommand>> m_pendingCommands;
};

#endif //EVENT_PROCESSOR_H
