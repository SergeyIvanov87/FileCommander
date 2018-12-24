#ifndef FILE_SYSTEM_UTILS_H
#define FILE_SYSTEM_UTILS_H
#include <list>
#include <array>
#include <string>
#include <optional>
#include <experimental/filesystem>
#include <stdio.h>
#include <openssl/md5.h>
#include "Interfaces/WatchConsumer.h"


namespace FileSystemUtils
{
namespace fs = std::experimental::filesystem;


struct DummyObserver
{
    template <class Last, class ...Args>
    void notify(Last &&last, Args &&...args){}

    bool isInterrupted() const { return false; };
};

struct FileExtractor
{
    using ExtractedType = fs::path;
    using Container = std::vector<ExtractedType>;
    inline std::optional<typename Container::value_type> operator() (const fs::directory_entry &entry);
};

struct FileSizeExtractor
{
    using ExtractedType = std::uintmax_t;
    using Container = std::multimap<ExtractedType, fs::path>;
    inline std::optional<typename Container::value_type> operator() (const fs::directory_entry &entry);
};

struct FileMd5Extractor
{
    using ExtractedType = std::array<unsigned char, MD5_DIGEST_LENGTH>;

    inline bool calcMD5(const std::string &filePath, ExtractedType &md5);
    using Container = std::multimap<ExtractedType, fs::path>;
    inline std::optional<typename Container::value_type> operator() (const fs::directory_entry &entry);
};

using Files = std::list<fs::path>;

template <class Collector, class Observer = DummyObserver>
typename Collector::Container collectEntriesFromDir(const std::string &directoryPath,
                                                    Collector collector,
                                                    Observer &observer,
                                                    bool isResursive = false);

inline bool checkFilesEqual(std::ifstream& in1, std::ifstream& in2);
}
#endif //FILE_SYSTEM_UTILS_H
