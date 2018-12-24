#ifndef SEARCH_DUPLICATE_FILES_CMD_H
#define SEARCH_DUPLICATE_FILES_CMD_H


#include <sstream>
#include "AsyncCommand.h"

class EventProcessor;
class SearchDuplicateFilesMD5Cmd : public AsyncCommand
{
public:
    using CommandResultType = AsyncCommandResult;
    using ResultConsumer = typename CommandResultType::ResultConsumer;
    using ResultProducer = typename CommandResultType::ResultProducer;

    SearchDuplicateFilesMD5Cmd(const std::string &leftDirpath, const std::string &rightDirpath);
    ~SearchDuplicateFilesMD5Cmd() = default;
    void execute() override;
    std::string to_string() const override;

private:
    std::string m_leftDirPath;
    std::string m_rightDirPath;
};

template <>
struct CommandTraits<SearchDuplicateFilesMD5Cmd>
{
    using ResultProducer = typename SearchDuplicateFilesMD5Cmd::ResultProducer;
    using ResultProducerPtr = std::shared_ptr<ResultProducer>;

    using ResultConsumer = typename SearchDuplicateFilesMD5Cmd::ResultConsumer;
    using ResultConsumerPtr = std::shared_ptr<ResultConsumer>;

    using CommandResultType = typename SearchDuplicateFilesMD5Cmd::CommandResultType;
    using CommandResultTypePtr = std::shared_ptr<CommandResultType>;
};

#endif //SEARCH_DUPLICATE_FILES_CMD_H
