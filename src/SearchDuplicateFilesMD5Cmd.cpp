#include <experimental/filesystem>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <list>
#include <string.h>
#include "CommandBase.hpp"
#include "SearchDuplicateFilesMD5Cmd.h"
#include "Utils/FileSystemUtils.hpp"



SearchDuplicateFilesMD5Cmd::SearchDuplicateFilesMD5Cmd(const std::string &leftDirpath, const std::string &rightDirpath) :
 AsyncCommand(),
 m_leftDirPath(leftDirpath),
 m_rightDirPath(rightDirpath)
{
}

void SearchDuplicateFilesMD5Cmd::execute()
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

    FileSystemUtils::FileMd5Extractor collector;
    auto duplicatedFiles = FileSystemUtils::collectEntriesFromDir(m_leftDirPath, collector, *res, true);
    duplicatedFiles.merge(FileSystemUtils::collectEntriesFromDir(m_rightDirPath, collector, *res, true));

    std::vector<std::pair<std::string, std::string>> dup;
    if(!duplicatedFiles.empty())
    {
        auto oldVal = duplicatedFiles.begin();
        for(auto it = std::next(duplicatedFiles.begin()); it != duplicatedFiles.end(); ++it)
        {
            //MD5 has a collistion in tooooooo rare cases
            if(it->first == oldVal->first)
            {
                dup.push_back(std::make_pair(oldVal->second, it->second));
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


std::string SearchDuplicateFilesMD5Cmd::to_string() const
{
    std::string ret("Search identically content files in directories: ");
    ret = ret + m_leftDirPath + " and " + m_rightDirPath;
    return ret;
}
