#include <experimental/filesystem>
#include <algorithm>
#include <fstream>
#include <list>
#include <string.h>
#include "CommandBase.hpp"
#include "CompareDirCmd.h"
#include "Utils/FileSystemUtils.hpp"

namespace fs = std::experimental::filesystem;


CompareDirCmd::CompareDirCmd(const std::string &leftDirpath, const std::string &rightDirpath) :
 AsyncCommand(),
 m_leftDirPath(leftDirpath),
 m_rightDirPath(rightDirpath)
{
}

void CompareDirCmd::execute()
{
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


    FileSystemUtils::FileExtractor collector;
    auto ldFiles = FileSystemUtils::collectEntriesFromDir(m_leftDirPath, collector, *res, false);
    if(res->isInterrupted()) return;
    auto rdFiles = FileSystemUtils::collectEntriesFromDir(m_rightDirPath, collector, *res, false);
    if(res->isInterrupted()) return;

    std::sort(ldFiles.begin(), ldFiles.end());
    std::sort(rdFiles.begin(), rdFiles.end());

    std::list<fs::path> commonFiles;
    std::set_intersection(ldFiles.begin(), ldFiles.end(), rdFiles.begin(), rdFiles.end(), std::back_inserter(commonFiles));
    for (auto it = commonFiles.begin(); it != commonFiles.end(); ++it)
    {
        fs::path l(m_leftDirPath);
        l /=  *it;
        fs::path r(m_rightDirPath);
        r /= *it;

        std::ifstream f1(l.string(), std::ios::binary);
        std::ifstream f2(r.string(), std::ios::binary);
        if(!FileSystemUtils::checkFilesEqual(f1, f2))
        {
            it = commonFiles.erase(it);
        }

        if(res->isInterrupted())
        {
            return;
        }
    }

    res->notify("Equal files list:");
    if(!commonFiles.empty())
    {
        for( const auto fileName : commonFiles)
        {
            res->notify(fileName.string());
        }
    }
    else
    {
        res->notify("<There are no equal files>");
    }
    res->setFinalize();
}


std::string CompareDirCmd::to_string() const
{
    std::string ret("Compare files in binary way from directories: ");
    ret = ret + m_leftDirPath + " and " + m_rightDirPath;
    return ret;
}
