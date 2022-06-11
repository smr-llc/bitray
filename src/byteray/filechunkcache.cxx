#include "filechunkcache.hxx"
#include <cstring>
#include <iostream>

FileChunkCache::FileChunkCache(std::fstream *filestream, std::mutex *fileMutex, int64_t offset, int64_t size) :
    m_size(size),
    m_offset(offset),
    m_filestream(filestream),
    m_fileMutex(fileMutex)
{
    m_data = (char*)malloc(m_size);
    {
        std::scoped_lock lock(*m_fileMutex);
        m_filestream->seekg(m_offset);
        m_filestream->read(m_data, m_size);
    }
}

FileChunkCache::~FileChunkCache() {
    this->syncToFile();
    free(m_data);
}

int64_t FileChunkCache::size() const {
    return m_size;
}

int64_t FileChunkCache::offset() const {
    return m_offset;
}

char FileChunkCache::at(int64_t idx) const {
    std::scoped_lock lock(m_cacheMutex);
    return m_data[idx];
}

void FileChunkCache::read(char *buffer, int64_t bufferLength, int64_t readOffset) const {
    std::scoped_lock lock(m_cacheMutex);
    memcpy(buffer, m_data + readOffset, bufferLength);
}

void FileChunkCache::write(const char *data, int64_t dataLength, int64_t writeOffset) {
    std::scoped_lock lock(m_cacheMutex);
    memcpy(m_data + writeOffset, data, dataLength);
}

void FileChunkCache::syncToFile() {
    std::scoped_lock lock(*m_fileMutex, m_cacheMutex);
    m_filestream->seekp(m_offset);
    m_filestream->write(m_data, m_size);
}