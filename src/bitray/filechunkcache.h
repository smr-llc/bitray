#pragma once

#include <mutex>
#include <shared_mutex>
#include <fstream>

class FileChunkCache {
public:
    FileChunkCache(std::fstream *filestream, std::mutex *fileMutex, int64_t offset, int64_t length);
    ~FileChunkCache();

    int64_t size() const;
    int64_t offset() const;
    
    char at(int64_t idx) const;
    void read(char *buffer, int64_t bufferLength, int64_t readOffset = 0) const;
    void write(const char *data, int64_t dataLength, int64_t writeOffset = 0);

    void syncToFile();

private:
    char* m_data;
    int64_t m_size;
    int64_t m_offset;
    std::fstream *m_filestream;
    std::mutex *m_fileMutex;
    mutable std::shared_mutex m_cacheMutex;
};