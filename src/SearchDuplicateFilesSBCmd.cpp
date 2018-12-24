#include <experimental/filesystem>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <list>
#include <string.h>
#include "CommandBase.hpp"
#include "SearchDuplicateFilesSBCmd.h"
#include "Utils/FileSystemUtils.hpp"



SearchDuplicateFilesSBCmd::SearchDuplicateFilesSBCmd(const std::string &leftDirpath, const std::string &rightDirpath) :
 AsyncCommand(),
 m_leftDirPath(leftDirpath),
 m_rightDirPath(rightDirpath)
{
}

void SearchDuplicateFilesSBCmd::execute()
{
    namespace fs = std::experimental::filesystem;
    auto res = m_resultConsumer.lock();
    if(!fs::is_directory(m_leftDirPath))
    {
        res->notify("Left directory is invalid: ", m_leftDirPath);
        res->setFinalize();
        return;
    }

    if(!fs::is_directory(m_rightDirPath))
    {
        res->notify("Right directory is invalid: ", m_rightDirPath);
        res->setFinalize();
        return;
    }

    FileSystemUtils::FileSizeExtractor collector;
    auto duplicatedFiles = FileSystemUtils::collectEntriesFromDir(m_leftDirPath, collector, *res, true);
    duplicatedFiles.merge(FileSystemUtils::collectEntriesFromDir(m_rightDirPath, collector, *res, true));

    std::vector<std::pair<std::string, std::string>> dup;
    if(!duplicatedFiles.empty())
    {
        auto oldVal = duplicatedFiles.begin();
        for(auto it = std::next(duplicatedFiles.begin()); it != duplicatedFiles.end(); ++it)
        {
            if(it->first == oldVal->first)
            {
                std::ifstream f1(it->second.string(), std::ios::binary);
                std::ifstream f2(oldVal->second.string(), std::ios::binary);
                if(FileSystemUtils::checkFilesEqual(f1, f2))
                {
                    dup.push_back(std::make_pair(oldVal->second, it->second));
                }
            }
            else
            {
                oldVal = it;
            }
        }
    }
    res->notify("Collected Duplicated Files:");
    for(const auto &l : dup)
    {
        res->notify("[", l.first, "]\nwith\n[", l.second, "]\n");
    }

    res->setFinalize();
}


std::string SearchDuplicateFilesSBCmd::to_string() const
{
    std::string ret("Search identically content files in directories: ");
    ret = ret + m_leftDirPath + " and " + m_rightDirPath;
    return ret;
}
