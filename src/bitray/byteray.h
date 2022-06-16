#pragma once

#include <cstdint>
#include <iostream>
#include <deque>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <map>

#include "filechunkcache.h"

namespace fs = std::filesystem;

/*!
 * \brief An array of bytes that can accomodate very large sizes
 * 
 */
class ByteRay {
public:

    ByteRay();
    ByteRay(int64_t size);
    ByteRay(const char *data, int64_t size);
    ByteRay(fs::path filepath, int64_t size = -1);
    ByteRay(std::istream &dataStream, int64_t size);

    ByteRay(const ByteRay &other, int64_t size);
    ByteRay(const ByteRay &other);
    ByteRay(const ByteRay *other);
    ByteRay& operator=(const ByteRay &other);

    ~ByteRay();

    /// get byte at ith position
    char at(int64_t i) const;

    /// get the size of the ByteRay
    int64_t size() const;

    /// resize the ByteRay
    void resize(int64_t size);

    void read(char *buffer, int64_t bufferLength, int64_t readOffset = 0) const;
    void write(const char *data, int64_t dataLength, int64_t writeOffset = 0);

protected:
    void initFromOther(const ByteRay &other, int64_t size);
    void initFromStream(std::istream &dataStream, int64_t size);
    void reinitializeCache(int64_t size = -1);
    std::shared_ptr<FileChunkCache> loadCacheAt(int64_t i) const;

private:
    int64_t m_size;
    mutable std::mutex m_mutex;
    mutable std::shared_mutex m_cacheMutex;

    fs::path m_dataFilePath;
    mutable std::fstream m_dataFile;
    mutable std::mutex m_dataFileMutex;

    mutable std::deque<int64_t> m_recentCacheAccess;
    mutable std::map<int64_t, std::shared_ptr<FileChunkCache>> m_dataCaches;
};