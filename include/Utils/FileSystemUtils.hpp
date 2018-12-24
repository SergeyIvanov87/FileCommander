#ifndef FILE_SYSTEM_UTILS_HPP
#define FILE_SYSTEM_UTILS_HPP

#include <algorithm>
#include <fstream>
#include <list>
#include <string>
#include <string.h>
#include <stdio.h>
#include "Utils/FileSystemUtils.h"

namespace FileSystemUtils
{
namespace fs = std::experimental::filesystem;

/********************************Extractors****************************/
inline std::optional<typename FileExtractor::Container::value_type>
    FileExtractor::operator() (const fs::directory_entry &entry)
{
    if(fs::is_regular_file(entry.path()))
    {
        return std::make_optional(typename FileExtractor::Container::value_type(entry.path().filename()) );
    }
    else
        return {};
}


inline std::optional<typename FileSizeExtractor::Container::value_type>
    FileSizeExtractor::operator() (const fs::directory_entry &entry)
{
    if(fs::is_regular_file(entry.path()))
    {
        return std::make_optional(typename FileSizeExtractor::Container::value_type(fs::file_size(entry.path()), entry.path()) );
    }
    else
        return {};
}



inline bool FileMd5Extractor::calcMD5(const std::string &filename, FileMd5Extractor::ExtractedType &md5)
{
    FILE *inFile = fopen (filename.c_str(), "rb");
    if (inFile == nullptr)
    {
        std::string error("File can't be opened to calculate MD5: ");
        error +=  filename;
        throw std::runtime_error(error);
    }
    MD5_CTX mdContext;
    int bytes;

    static const size_t BLOCK_SIZE = 4096;
    unsigned char data[BLOCK_SIZE];


    MD5_Init (&mdContext);
    while ((bytes = fread (data, 1, BLOCK_SIZE, inFile)) != 0)
    {
        MD5_Update (&mdContext, data, bytes);
    }
    MD5_Final (md5.data(), &mdContext);
    fclose (inFile);
    return true;
}

inline std::optional<typename FileMd5Extractor::Container::value_type>
    FileMd5Extractor::operator() (const fs::directory_entry &entry)
{
    if(fs::is_regular_file(entry.path()))
    {
        FileMd5Extractor::ExtractedType md5{};
        calcMD5(entry.path().string(), md5);
        return std::make_optional(typename FileMd5Extractor::Container::value_type(md5, entry.path()) );
    }
    else
        return {};
}
///helper
template<class Cont>
struct Merge{};
template<class T>
struct Merge<std::vector<T>>
{
    static void merge(std::vector<T> &to, std::vector<T> &&from)
    {
        if(&to == &from) return;
        to.insert(to.end(), from.begin(), from.end());
    }
    static void push(std::vector<T> &to,typename std::vector<T>::value_type &&val)
    {
        to.push_back(std::move(val));
    }
};

template<class T, class U>
struct Merge<std::multimap<T,U>>
{
    static void merge(std::multimap<T,U> &to, std::multimap<T,U> &&from)
    {
        if(&to == &from) return;
        to.merge(std::move(from));
    }
    static void push(std::multimap<T,U> &to,typename std::multimap<T,U>::value_type &&val)
    {
        to.insert(std::move(val));
    }
};

/**********************************************************************/
template <class Collector, class Observer>
typename Collector::Container collectEntriesFromDir(const std::string &directoryPath,
                                                    Collector collector,
                                                    Observer &observer,
                                                    bool isResursive/* = false*/)
{
    typename Collector::Container retFiles;
    try
    {
        for(const auto& p : fs::directory_iterator(directoryPath))
        {
            if(observer.isInterrupted()) break;
            if(isResursive and fs::is_directory(p))
            {
                Merge<typename Collector::Container>::merge(retFiles, collectEntriesFromDir(p.path().string(), collector, observer, isResursive));
            }

            try
            {
                if(auto optional = collector(p); optional)
                {
                    auto val = optional.value();
                    Merge<typename Collector::Container>::push(retFiles, std::move(val));
                }
            }
            catch(std::exception &ex)
            {
                observer.notify(ex.what());
            }
        }
    }
    catch(std::exception &ex)
    {
        observer.notify(ex.what());
    }
    return retFiles;
}


bool checkFilesEqual(std::ifstream& in1, std::ifstream& in2)
{
    using namespace std;
    if(!in1.is_open() or !in2.is_open())
    {
        return false;
    }
    ifstream::pos_type size1, size2;

    size1 = in1.seekg(0, ifstream::end).tellg();
    in1.seekg(0, ifstream::beg);

    size2 = in2.seekg(0, ifstream::end).tellg();
    in2.seekg(0, ifstream::beg);

    if(size1 != size2)
    {
        return false;
    }

    static const size_t BLOCK_SIZE = 4096;
    size_t remainingBytes = size1;

    while(remainingBytes)
    {
        char buffer1[BLOCK_SIZE], buffer2[BLOCK_SIZE];
        size_t size = std::min(BLOCK_SIZE, remainingBytes);

        in1.read(buffer1, size);
        in2.read(buffer2, size);

        if(memcmp(buffer1, buffer2, size))
        {
            return false;
        }

        remainingBytes -= size;
    }

    return true;
}

}
#endif //FILE_SYSTEM_UTILS_HPP
