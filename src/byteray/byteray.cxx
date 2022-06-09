#include <stdexcept>
#include <algorithm>
#include <string>
#include <memory>

#include "byteray.hxx"

#define CHUNK_SIZE (10L * 1000L * 1000L)
#define MAX_CACHE_CHUNKS 5L

std::string random_string( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

ByteRay::ByteRay() :
    m_size(0)
{
    m_dataFilePath = fs::temp_directory_path() / ("byteray" + random_string(8));
    while (fs::exists(m_dataFilePath)) {
        m_dataFilePath = fs::temp_directory_path() / ("byteray" + random_string(8));
    }
    m_dataFile.open(m_dataFilePath, std::fstream::out);
    m_dataFile.write("byteray", 7);
    m_dataFile.close();
    m_dataFile.open(m_dataFilePath, std::fstream::in | std::fstream::out | std::fstream::binary);
}

ByteRay::ByteRay(int64_t size) :
    ByteRay()
{
    reinitializeCache(size);
}

ByteRay::ByteRay(const char *data, int64_t size) :
    ByteRay(size)
{
    this->write(data, size);
}

ByteRay::ByteRay(fs::path filepath, int64_t size) :
    ByteRay()
{
    if (size < 0) {
        size = fs::file_size(filepath) * 8;
    }
    
    std::ifstream fs(filepath, std::fstream::in | std::fstream::binary);
    initFromStream(fs, size);
}

ByteRay::ByteRay(std::istream &dataStream, int64_t size) :
    ByteRay()
{
    initFromStream(dataStream, size);
}

ByteRay::ByteRay(const ByteRay &other) :
    ByteRay()
{
    initFromOther(other, other.size());
}

ByteRay::ByteRay(const ByteRay *other) :
    ByteRay()
{
    initFromOther(*other, other->size());
}

ByteRay::~ByteRay()
{
    m_dataFile.close();
    fs::remove(m_dataFilePath);
}

ByteRay& ByteRay::operator=(const ByteRay &other)
{
    initFromOther(other, other.size());
    return *this;
}

void ByteRay::initFromOther(const ByteRay &other, int64_t size)
{
    if (size > other.size()) {
        throw std::invalid_argument("The provided source ByteRay has fewer than the requested " + std::to_string(size) + " bytes");
    }
    m_size = size;
    int64_t bytesToRead = m_size;
    int64_t readOffset = 0;
    auto byteBuffer = std::make_unique<char>(CHUNK_SIZE);
    while (bytesToRead > 0) {
        int64_t byteChunkSize = std::min(bytesToRead, int64_t(CHUNK_SIZE));
        other.read(byteBuffer.get(), byteChunkSize, readOffset);
        m_dataFile.write(byteBuffer.get(), byteChunkSize);
        bytesToRead -= byteChunkSize;
        readOffset += byteChunkSize;
    }
    reinitializeCache();
}

void ByteRay::initFromStream(std::istream &dataStream, int64_t size)
{
    m_size = size;
    int64_t bytesToRead = m_size;
    auto byteBuffer = std::make_unique<char>(CHUNK_SIZE);
    while (bytesToRead > 0) {
        int64_t actualBytes = std::min(bytesToRead, int64_t(CHUNK_SIZE));
        int64_t bytesRead = dataStream.readsome(byteBuffer.get(), actualBytes);
        m_dataFile.write(byteBuffer.get(), bytesRead);
        bytesToRead -= bytesRead;

        if (bytesToRead > 0 && bytesRead < 1) {
            throw std::invalid_argument("'dataStream' provided to ByteRay constructor had fewer than " + std::to_string(size) + " bytes");
        }
    }

    reinitializeCache();
}

void ByteRay::reinitializeCache(int64_t size)
{
    std::scoped_lock lock(m_cacheMutex, m_mutex);
    m_dataCaches.clear();
    m_recentCacheAccess.clear();
    if (size >= 0) {
        m_dataFile.close();
        m_size = size;
        fs::resize_file(m_dataFilePath, size);
        m_dataFile.open(m_dataFilePath, std::fstream::in | std::fstream::out | std::fstream::binary);
    }
}

std::shared_ptr<FileChunkCache> ByteRay::loadCacheAt(int64_t i) const
{
    std::scoped_lock lock(m_cacheMutex);
    int64_t cacheIdx = i / CHUNK_SIZE;
    auto it = m_dataCaches.find(cacheIdx);
    if (it != m_dataCaches.end()) {
        return it->second;
    }

    int64_t byteIdx = cacheIdx * CHUNK_SIZE;

    auto chunk = std::shared_ptr<FileChunkCache>(new FileChunkCache(&m_dataFile, &m_dataFileMutex, byteIdx, CHUNK_SIZE));
    m_dataCaches.emplace(cacheIdx, chunk);
    m_recentCacheAccess.push_back(cacheIdx);

    if (m_recentCacheAccess.size() > MAX_CACHE_CHUNKS) {
        int64_t removedCacheIndex = m_recentCacheAccess.front();
        m_recentCacheAccess.pop_front();
        m_dataCaches.erase(removedCacheIndex);
    }

    return chunk;
}

char ByteRay::at(int64_t i) const
{
    auto cacheChunk = loadCacheAt(i);
    return cacheChunk->at(i - cacheChunk->offset());
}

int64_t ByteRay::size() const
{
    return m_size;
}

void ByteRay::resize(int64_t size)
{
    std::scoped_lock lock(m_mutex);
    reinitializeCache(size);
}

void ByteRay::read(char *buffer, int64_t bufferLength, int64_t readOffset) const
{
    int64_t bytesToRead = bufferLength;
    int64_t bufferOffset = 0;
    while (bytesToRead > 0) {
        auto cacheChunk = loadCacheAt(readOffset);
        int64_t chunkOffset = readOffset - cacheChunk->offset();
        int64_t chunkToRead = std::min(bytesToRead, cacheChunk->size() - chunkOffset);

        cacheChunk->read(buffer + bufferOffset, chunkToRead, chunkOffset);

        readOffset += chunkToRead;
        bytesToRead -= chunkToRead;
        bufferOffset += chunkToRead;
    }
}

void ByteRay::write(const char *data, int64_t dataLength, int64_t writeOffset)
{
    std::scoped_lock lock(m_mutex);
    int64_t bytesToWrite = dataLength;
    int64_t bufferOffset = 0;
    while (bytesToWrite > 0) {
        auto cacheChunk = loadCacheAt(writeOffset);
        int64_t chunkOffset = writeOffset - cacheChunk->offset();
        int64_t chunkToWrite = std::min(bytesToWrite, cacheChunk->size() - chunkOffset);

        cacheChunk->write(data + bufferOffset, chunkToWrite, chunkOffset);

        writeOffset += chunkToWrite;
        bytesToWrite -= chunkToWrite;
        bufferOffset += chunkToWrite;
    }
}