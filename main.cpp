#include <iostream>
#include <string>
#include <list>
#include "CommandIdentitier.h"
#include "CompareDirCmd.h"
#include "SearchDuplicateFilesMD5Cmd.h"
#include "SearchDuplicateFilesSBCmd.h"
#include "CommandBase.hpp"
#include "Widget/EventProcessor.hpp"

#include <unistd.h>

template<class Container>
Container parseString(const std::string &str, char delim = ' ')
{
    Container ret;
    if(str.empty())
    {
        return ret;
    }

    size_t pos = 0, newPos = 0;
    while((newPos = str.find(delim, pos)) != std::string::npos)
    {
        ret.emplace_back(str, pos, newPos - pos);
        pos = newPos + 1;
    }

    ret.emplace_back(str, pos, newPos);
    return ret;
}

void usage()
{
    std::cout << "Available commands: 'quit', 'compareDir', 'searchDupFiles'" << std::endl;
    std::cout << "For example:\ncompareDir <leftDirector> <rightDIrectory>" << std::endl;
    std::cout << "\tsearchDupFilesMD5 <leftDirector> <rightDIrectory>" << std::endl;
    std::cout << "\tsearchDupFilesSizeBased <leftDirector> <rightDIrectory>" << std::endl;
}

int main(int argc, char* argv[])
{
    static constexpr size_t cmdMaxSize = 1024;

    EventProcessor eventProcessor;
    eventProcessor.start();

    while(!(std::cin.fail() or std::cin.eof()))
    {
        char command[cmdMaxSize];
        std::cout << "Enter a command: " << std::endl;
        std::cin.getline(command, cmdMaxSize);

        auto cont = parseString<std::list<std::string>>(command);
        if(cont.empty())
        {
            std::cout << "Enter a command: " << std::endl;
            usage();
            continue;
        }

        auto cmdId = stringToCommandIdentifier(*cont.begin());
        if(cmdId == CommandIdentifier::CommandIdentifiersMax)
        {
            std::cout << "Invalid command" << std::endl;
            usage();
            continue;
        }

        cont.erase(cont.begin());

        std::shared_ptr<AsyncCommandResult> res;
        switch(cmdId)
        {
            case CommandIdentifier::compareDir:
            {
                if(cont.size() != 2)
                {
                    std::cout << "Invalid arguments" << std::endl;
                    usage();
                    break;
                }
                res = eventProcessor.pushCommand(CommandBase::createCommand<CompareDirCmd>(*cont.begin(), *cont.rbegin()));
                break;
            }
            case CommandIdentifier::searchDupFilesMD5:
            {
                if(cont.size() != 2)
                {
                    std::cout << "Invalid arguments" << std::endl;
                    usage();
                    break;
                }
                res = eventProcessor.pushCommand(CommandBase::createCommand<SearchDuplicateFilesMD5Cmd>(*cont.begin(), *cont.rbegin()));
                break;
            }
            case CommandIdentifier::searchDupFilesSizeBased:
            {
                if(cont.size() != 2)
                {
                    std::cout << "Invalid arguments" << std::endl;
                    usage();
                    break;
                }
                res = eventProcessor.pushCommand(CommandBase::createCommand<SearchDuplicateFilesSBCmd>(*cont.begin(), *cont.rbegin()));
                break;
            }

            case CommandIdentifier::quit:
            {
                std::cin.setstate(std::ios_base::eofbit);
                break;
            }
            default:
            {
                std::cout << "Command: " << CommandIdentifierToString(cmdId) << " not in processing scope" << std::endl;
                break;
            }
        }


        if(res)
        {
            while(!res->isFinalized())
            {
                auto ret = res->getLastResult();
                if(!ret.empty())
                {
                    std::cout << ret << std::endl;
                }
                usleep(100);
            }

            std::cout << res->getLastResult() << std::endl;
        }
    }
    std::cout << "Stopped..." << std::endl;
    return 0;
}
