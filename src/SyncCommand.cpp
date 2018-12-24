#include "SyncCommand.h"

void SyncCommandResult::setFinalize()
{
}

std::string SyncCommandResult::getLastResult()
{
    //TODO no need inner lock actually...

    std::string ret = desription.str();
    std::stringstream().swap(desription);
    return ret;

}

bool SyncCommandResult::isFinalized() const
{
    return true;
}

