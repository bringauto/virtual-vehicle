#include "gtest/gtest.h"
//todo initialize logger only once
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}