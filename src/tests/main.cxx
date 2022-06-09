#include "byteray.hxx"
#include <iostream>
#include <filesystem>
#include <fstream>

#define WBUF_SZ 1000
#define WBUF_VAL 'a'
#define READ_INCR 14

void create_write_read(int64_t size, const char *wbuf, int64_t writeSize) {
    std::cout << "Trying " << size << std::endl;

    ByteRay bray(size);

    for (int i = 0; i < size; i += writeSize) {
        int64_t amount = std::min(writeSize, size - i);
        bray.write(wbuf, amount, i);
    }

    for (int i = 0; i < size; i += READ_INCR) {
        if (bray.at(i) != WBUF_VAL) {
            std::cerr << "ByteRay at " << i << " != " << WBUF_VAL;
            return;
        }
    }

    std::cout << "Done" << std::endl;
}


int main() {

    char writeBuf[WBUF_SZ];
    for (int i = 0; i < WBUF_SZ; i++) {
        writeBuf[i] = WBUF_VAL;
    }

    create_write_read(10, writeBuf, 1);
    create_write_read(1000, writeBuf, 100);
    create_write_read(WBUF_SZ, writeBuf, WBUF_SZ);
    create_write_read(10000, writeBuf, WBUF_SZ);
    create_write_read(1000 * 1000, writeBuf, WBUF_SZ);
    create_write_read(4 * 1000 * 1000, writeBuf, 100);
    create_write_read(8 * 1000 * 1000, writeBuf, WBUF_SZ);
    create_write_read(16 * 1000 * 1000, writeBuf, 100);
    create_write_read(100 * 1000 * 1000, writeBuf, 100);
    //create_write_read(1000 * 1000 * 1000, writeBuf, WBUF_SZ);

    return 0;
}