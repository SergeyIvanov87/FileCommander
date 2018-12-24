#ifndef SEARCH_DUPLICATE_FILES_SB_CMD_H
#define SEARCH_DUPLICATE_FILES_SB_CMD_H


#include <sstream>
#include "AsyncCommand.h"

class EventProcessor;
class SearchDuplicateFilesSBCmd : public AsyncCommand
{
public:
    using CommandResultType = AsyncCommandResult;
    using ResultConsumer = typename CommandResultType::ResultConsumer;
    using ResultProducer = typename CommandResultType::ResultProducer;

    SearchDuplicateFilesSBCmd(const std::string &leftDirpath, const std::string &rightDirpath);
    ~SearchDuplicateFilesSBCmd() = default;
    void execute() override;
    std::string to_string() const override;

private:
    std::string m_leftDirPath;
    std::string m_rightDirPath;
};

template <>
struct CommandTraits<SearchDuplicateFilesSBCmd>
{
    using ResultProducer = typename SearchDuplicateFilesSBCmd::ResultProducer;
    using ResultProducerPtr = std::shared_ptr<ResultProducer>;

    using ResultConsumer = typename SearchDuplicateFilesSBCmd::ResultConsumer;
    using ResultConsumerPtr = std::shared_ptr<ResultConsumer>;

    using CommandResultType = typename SearchDuplicateFilesSBCmd::CommandResultType;
    using CommandResultTypePtr = std::shared_ptr<CommandResultType>;
};

#endif //SEARCH_DUPLICATE_FILES_SB_CMD_H
