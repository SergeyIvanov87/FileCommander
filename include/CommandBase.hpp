#ifndef COMMAND_BASE_HPP
#define COMMAND_BASE_HPP

#include <cassert>
#include "Widget/EventProcessor.h"

template <class Arg>
void IResutConsumer::notify(Arg &&arg)
{
    {
        std::unique_lock<std::mutex> lock(m_descriptionMutex);
        desription << arg << "\n";
    }
}

template <class First, class ...Args>
void IResutConsumer::notify(First &&first, Args &&...args)
{
    {
        std::unique_lock<std::mutex> lock(m_descriptionMutex);
        desription << first;
    }
    notify(std::forward<Args>(args)...);
}


template<class SpecificCommand>
typename CommandTraits<SpecificCommand>::CommandResultTypePtr
            CommandBase::constructCmdResult(EventProcessor *parent, SpecificCommand *cmd)
{
    using Result = typename CommandTraits<SpecificCommand>::CommandResultType;
    using ResultPtr = typename CommandTraits<SpecificCommand>::CommandResultTypePtr;

    ResultPtr res;
    if(cmd and parent)
    {
        res.reset(new Result(parent->getNextCmdId(), parent));
        cmd->setResultConsumer(res);
        return res;
    }
    assert(false && "constructCmdResult failed");
    return res;
}

template<class SpecificCommand, class ...Args>
std::unique_ptr<SpecificCommand>
        CommandBase::createCommand(Args &&...args)
{
    return std::unique_ptr<SpecificCommand> ( new SpecificCommand(std::forward<Args>(args)...));
}
#endif //COMMAND_BASE_HPP
