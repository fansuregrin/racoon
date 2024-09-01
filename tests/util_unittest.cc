/**
 * @file util_unittest.cc
 * @author Fansure Grin
 * @date 2024-08-31
 * @brief Unit test for util module.
 */
#include <gtest/gtest.h>
#include "../src/racoon/util/util.h"

TEST(UtilTest, GetPid) {
    EXPECT_EQ(racoon::GetPid(), racoon::GetPid());
}