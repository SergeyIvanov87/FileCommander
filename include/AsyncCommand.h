#ifndef ASYNC_COMMAND_H
#define ASYNC_COMMAND_H
#include <memory>
#include <string>
#include <mutex>
#include <atomic>
#include "CommandBase.h"

class AsyncCommand : public CommandBase
{
protected:
    AsyncCommand() = default;
public:
    virtual ~AsyncCommand() = default;
};

class EventProcessor;
class AsyncCommandResult : public CommandResultBase<AsyncCommandResult>
{
public:
    using ResultConsumer = typename CommandResultBase<AsyncCommandResult>::ResultConsumer;
    using ResultProducer = typename CommandResultBase<AsyncCommandResult>::ResultProducer;

    AsyncCommandResult(size_t index, EventProcessor *parent);
    virtual ~AsyncCommandResult() = default;

    void setFinalize() override;
    std::string getLastResult() override;
    bool isFinalized() const override;

    //can be interrupted
    void setInterrupt() override;
    bool isInterrupted() const override;
private:
    mutable std::mutex m_procesingMutex;
    bool m_isFinished = false;
    EventProcessor *m_parent;
    std::atomic<bool> m_interrupted;
};

#endif //ASYNC_COMMAND_H
