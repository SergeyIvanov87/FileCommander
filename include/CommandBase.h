#ifndef COMMAND_BASE_H
#define COMMAND_BASE_H
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include "Interfaces/WatchConsumer.h"

//Traits for command class
template <class Command>
struct CommandTraits
{ /*No any traits provided for unspecified command*/ };



/* CommandBase
 * Base class for command
 */
class EventProcessor;
struct CommandBase
{
    template<class SpecificCommand, class ...Args>
    static std::unique_ptr<SpecificCommand> createCommand(Args &&...args);

    template<class SpecificCommand>
    static typename CommandTraits<SpecificCommand>::CommandResultTypePtr constructCmdResult(EventProcessor *parent, SpecificCommand *cmd);

    virtual void execute() = 0;
    virtual std::string to_string() const = 0;
protected:
    CommandBase() = default;
    ~CommandBase() = default;

    void setResultConsumer(std::shared_ptr<IResutConsumer> resultConsumerPtr);
    std::weak_ptr<IResutConsumer> m_resultConsumer;
};

/* Specific result of specific command execution
 * Provide async monitoring by using IResutConsumer & IResultProvider interface
 */
template<class Impl>
struct CommandResultBase : public IResutConsumer, public IResultProvider
{
    using ResultBaseImpl = Impl;
    using ResultConsumer = IResutConsumer;
    using ResultProducer = IResultProvider;

    CommandResultBase(size_t index) :
     IResutConsumer(), IResultProvider(), m_index(index)
    {}

    virtual ~CommandResultBase() = default;
    size_t getIndex() const
    {
        return m_index;
    }
private:
    size_t m_index;
};
#endif //COMMAND_BASE_H
