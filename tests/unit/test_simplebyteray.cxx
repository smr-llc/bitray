#include <gtest/gtest.h>
#include <tuple>
#include <memory>
#include <cstring>
#include "byteray.cxx"

namespace {

class CreateWriteReadFixture : public ::testing::TestWithParam<std::tuple<int64_t, int64_t>> {
};

TEST_P(CreateWriteReadFixture, CreateWriteReadByteRay) {
    size_t writeBlockSize = 1024;
    char fillChar = 'a';
    auto byteBuffer = std::make_unique<char[]>(writeBlockSize);
    memset(byteBuffer.get(), fillChar, writeBlockSize);

    auto [size, step] = GetParam();

    // Create the ByteRay
    ByteRay  bray(size);
    ASSERT_EQ(bray.size(), size);

    // Write to the ByteRay
    for (int64_t i = 0; i < size; i += writeBlockSize) {
        int64_t chunkSize = std::min(int64_t(writeBlockSize), size - i);
        bray.write(byteBuffer.get(), chunkSize, i);
    }

    // Read from the ByteRay
    for (int64_t i = 0; i < size; i += step) {
        SCOPED_TRACE("i = " + std::to_string(i));
        ASSERT_EQ(bray.at(i), fillChar);
    }
}

INSTANTIATE_TEST_CASE_P(
        CreateWriteReadByteRay,
        CreateWriteReadFixture,
        ::testing::Values(
                std::make_tuple(1L, 1),
                std::make_tuple(5L, 1),
                std::make_tuple(8L, 1),
                std::make_tuple(1L<<5L, 3L),
                std::make_tuple(1L<<10L, 7L),
                std::make_tuple(1L<<15L, 1L<<10 + 1L),
                std::make_tuple(1L<<17L, 1L<<12L),
                std::make_tuple(1L<<17L + 1L, 1L<<8L),
                std::make_tuple(1L<<17L - 1L, 1L<<8L),
                std::make_tuple(1L<<20, 1L<<15 + 3L),
                std::make_tuple(1L<<24, 8L),
                std::make_tuple(1L<<24, 1L<<19 + 3L),
                std::make_tuple(1L<<26, 1L<<10),
                std::make_tuple(1L<<26, 8L),
                std::make_tuple(1L<<28, 1L<<19 + 3L),
                std::make_tuple(1L<<32, 1L<<26 + 3L),
                std::make_tuple(1L<<34, 1L<<26 + 3L)
        )
);

}  // namespace