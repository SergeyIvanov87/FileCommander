#include "Widget/EventProcessor.h"

EventProcessor::~EventProcessor()
{
    stop();
}

size_t EventProcessor::getNextCmdId()
{
    static std::atomic<size_t > counter{};
    return counter.fetch_add(1);
}

void EventProcessor::start()
{
    if(!m_workerThread)
    {
        m_stop.store(false);
        m_workerThread.reset(new std::thread(&EventProcessor::executor, this));
    }
}

void EventProcessor::stop()
{
    m_stop.store(true);
    if(m_workerThread && m_workerThread->joinable())
    {
        m_waitCmdCondition.notify_all();
        m_workerThread->join();
    }
}

void EventProcessor::executor()
{
    while(!m_stop.load())
    {
        std::unique_ptr<AsyncCommand> receivedCmdPtr;
        {
            std::unique_lock<std::mutex> lk(m_cndMutex);
            m_waitCmdCondition.wait(lk, [this]()
            {
                return !m_pendingCommands.empty() or m_stop.load();
            });


            if(m_pendingCommands.empty())
            {//stop called
                break;
            }
            receivedCmdPtr = std::move(m_pendingCommands.begin()->second);
            m_pendingCommands.erase(m_pendingCommands.begin());
        }
        receivedCmdPtr->execute();
    }
}
