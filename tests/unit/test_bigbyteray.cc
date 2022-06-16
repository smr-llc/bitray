#include <gtest/gtest.h>
#include <tuple>
#include <memory>
#include <cstring>
#include "byteray.h"

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
    for (int64_t i = 0; i < size; i += step) {
        bray.write(byteBuffer.get(), 1, i);
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
                std::make_tuple(1L<<34, (1L<<28L) + (1L<<10L) + 13L), // 16 GiB
                std::make_tuple(1L<<35, (1L<<29L) + (1L<<11L) + 13L), // 32 GiB
                std::make_tuple(1L<<36, (1L<<30L) + (1L<<12L) + 13L), // 64 GiB
                std::make_tuple(1L<<37, (1L<<31L) + (1L<<13L) + 13L) // 128 GiB
        )
);

}  // namespace