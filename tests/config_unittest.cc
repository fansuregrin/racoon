/**
 * @file config_unittest.cc
 * @author Fansure Grin
 * @date 2024-08-31
 * @brief Unit test for class `Config`
 */
#include <thread>
#include <future>
#include <gtest/gtest.h>
#include "../src/racoon/config/config.h"

class ConfigTest : public testing::Test {};


// 测试多线程下单例模式是否正常
TEST_F(ConfigTest, GetInstance) {
    auto cfg1_ft = std::async(std::launch::async,
        racoon::Config::GetInstance, "./racoon.xml");
    auto cfg2_ft = std::async(std::launch::async,
        racoon::Config::GetInstance, "./racoon.xml");
    auto cfg3_ft = std::async(std::launch::async,
        racoon::Config::GetInstance, "./racoon.xml");
    
    auto &cfg1 = cfg1_ft.get();
    auto &cfg2 = cfg2_ft.get();
    auto &cfg3 = cfg3_ft.get();
    EXPECT_EQ(&cfg1, &cfg2);
    EXPECT_EQ(&cfg1, &cfg3);
}

// 测试配置项
TEST_F(ConfigTest, ConfigItems) {
    racoon::Config &cfg = racoon::Config::GetInstance("./racoon.xml");
    EXPECT_EQ(cfg.port, 8967);
    EXPECT_EQ(cfg.io_threads, 2);
    EXPECT_EQ(cfg.log_level, "DEBUG");
    EXPECT_EQ(cfg.log_dir, "../logs/");
    EXPECT_EQ(cfg.log_filename, "test_rpc_server");
    EXPECT_EQ(cfg.log_max_file_size, 4194304);
}