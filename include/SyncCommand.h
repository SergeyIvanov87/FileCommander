#ifndef SYNC_COMMAND_H
#define SYNC_COMMAND_H
#include "CommandBase.h"

class SyncCommand : public CommandBase
{
protected:
    SyncCommand() = default;
public:
    virtual ~SyncCommand() = default;
};


class SyncCommandResult : public CommandResultBase<SyncCommandResult>
{
public:
    using ResultConsumer = typename CommandResultBase<SyncCommandResult>::ResultConsumer;
    using ResultProducer = typename CommandResultBase<SyncCommandResult>::ResultProducer;

    SyncCommandResult() = default;
    virtual ~SyncCommandResult() = default;

    void setFinalize() override;
    std::string getLastResult() override;
    bool isFinalized() const override;
};
#endif //SYNC_COMMAND_H
