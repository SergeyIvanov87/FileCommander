#include "CommandBase.h"

void CommandBase::setResultConsumer(std::shared_ptr<IResutConsumer> resultConsumerPtr)
{
    m_resultConsumer = resultConsumerPtr;
}
