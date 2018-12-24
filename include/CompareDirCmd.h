#ifndef COMPARE_DIR_H
#define COMPARE_DIR_H
#include <sstream>
#include "AsyncCommand.h"

class EventProcessor;
class CompareDirCmd : public AsyncCommand
{
public:
    using CommandResultType = AsyncCommandResult;
    using ResultConsumer = typename CommandResultType::ResultConsumer;
    using ResultProducer = typename CommandResultType::ResultProducer;

    CompareDirCmd(const std::string &leftDirpath, const std::string &rightDirpath);
    ~CompareDirCmd() = default;
    void execute() override;
    std::string to_string() const override;

private:
    std::string m_leftDirPath;
    std::string m_rightDirPath;
};

template <>
struct CommandTraits<CompareDirCmd>
{
    using ResultProducer = typename CompareDirCmd::ResultProducer;
    using ResultProducerPtr = std::shared_ptr<ResultProducer>;

    using ResultConsumer = typename CompareDirCmd::ResultConsumer;
    using ResultConsumerPtr = std::shared_ptr<ResultConsumer>;

    using CommandResultType = typename CompareDirCmd::CommandResultType;
    using CommandResultTypePtr = std::shared_ptr<CommandResultType>;
};

#endif //COMPARE_DIR_H
