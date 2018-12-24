#ifndef EVENT_PROCESSOR_HPP
#define EVENT_PROCESSOR_HPP
#include "EventProcessor.h"
//#include "CommandBase.hpp"

template<class Cmd>
typename CommandTraits<Cmd>::CommandResultTypePtr EventProcessor::pushCommand(std::unique_ptr<Cmd> &&cmd)
{
    return pushCommandImpl(std::move(cmd));
}

template<class Cmd>
typename std::enable_if<
                        std::is_base_of<
                                    SyncCommand, Cmd
                                    >::value,
                        typename  CommandTraits<Cmd>::CommandResultTypePtr
                        >::type
         EventProcessor::pushCommandImpl(std::unique_ptr<Cmd> &&cmd)
{
    //Sync command should be executed in the same thread
    typename CommandTraits<Cmd>::CommandResultTypePtr resultPtr = cmd.constructCmdResult(this);
    cmd.execute();
    return resultPtr;
}


template<class Cmd>
typename std::enable_if<
                        std::is_base_of<
                                    AsyncCommand, Cmd
                                    >::value,
                        typename CommandTraits<Cmd>::CommandResultTypePtr
                        >::type
         EventProcessor::pushCommandImpl(std::unique_ptr<Cmd> &&cmd)
{
    typename CommandTraits<Cmd>::CommandResultTypePtr resultPtr =
            CommandBase::constructCmdResult<Cmd>(this, cmd.get());
    {
        //push cmd for async processing
        std::unique_lock<std::mutex> lk(m_cndMutex);
        m_pendingCommands.emplace(resultPtr->getIndex(), std::unique_ptr<AsyncCommand>(cmd.release()));
        m_waitCmdCondition.notify_all();
    }
    return resultPtr;
}
#endif //EVENT_PROCESSOR_HPP
