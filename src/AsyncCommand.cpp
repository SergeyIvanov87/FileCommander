#include "Widget/EventProcessor.h"
#include "AsyncCommand.h"
#include "CommandBase.hpp"

AsyncCommandResult::AsyncCommandResult(size_t index, EventProcessor *parent) :
    CommandResultBase<AsyncCommandResult>(index),
    m_parent(parent),
    m_interrupted(false)
{
}

void AsyncCommandResult::setFinalize()
{
    std::unique_lock<std::mutex> lock(m_procesingMutex);
    m_isFinished = true;
}

std::string AsyncCommandResult::getLastResult()
{
    std::unique_lock<std::mutex> lock(m_descriptionMutex);
    std::string ret = desription.str();
    std::stringstream().swap(desription);
    return ret;
}

bool AsyncCommandResult::isFinalized() const
{
    std::unique_lock<std::mutex> lock(m_procesingMutex);
    return m_isFinished;
}

void AsyncCommandResult::setInterrupt()
{
    notify("Interrupted");
    m_interrupted.store(true);
}

bool AsyncCommandResult::isInterrupted() const
{
    return m_interrupted.load();
}
