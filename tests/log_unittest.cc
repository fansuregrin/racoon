/**
 * @file log_unittest.cc
 * @author Fansure Grin
 * @date 2024-08-31
 * @brief Unit test for log module.
 */
#include <future>
#include <gtest/gtest.h>
#include "../src/racoon/log/log.h"

TEST(LogTest, LogLevelToString) {
    EXPECT_EQ(racoon::LogLevelToString(racoon::UNKNOWN), "UNKNOWN");
    EXPECT_EQ(racoon::LogLevelToString(racoon::DEBUG), "DEBUG");
    EXPECT_EQ(racoon::LogLevelToString(racoon::INFO), "INFO");
    EXPECT_EQ(racoon::LogLevelToString(racoon::WARN), "WARN");
    EXPECT_EQ(racoon::LogLevelToString(racoon::ERROR), "ERROR");
}

TEST(LogTest, StringToLogLevel) {
    EXPECT_EQ(racoon::StringToLogLevel("ABCD"), racoon::UNKNOWN);
    EXPECT_EQ(racoon::StringToLogLevel("DEBUG"), racoon::DEBUG);
    EXPECT_EQ(racoon::StringToLogLevel("INFO"), racoon::INFO);
    EXPECT_EQ(racoon::StringToLogLevel("WARN"), racoon::WARN);
    EXPECT_EQ(racoon::StringToLogLevel("ERROR"), racoon::ERROR);
}


class LogEventTest : public testing::Test {};

TEST_F(LogEventTest, ToString) {
    racoon::LogEvent log_event_debug(racoon::DEBUG, __FILE__, __LINE__);
    racoon::LogEvent log_event_info(racoon::INFO, __FILE__, __LINE__);
    racoon::LogEvent log_event_warn(racoon::WARN, __FILE__, __LINE__);
    racoon::LogEvent log_event_error(racoon::ERROR, __FILE__, __LINE__);
    printf("%s\n", log_event_debug.ToString().c_str());
    printf("%s\n", log_event_info.ToString().c_str());
    printf("%s\n", log_event_warn.ToString().c_str());
    printf("%s\n", log_event_error.ToString().c_str());
}

TEST_F(LogEventTest, GetLogLevel) {
    racoon::LogEvent log_event_debug(racoon::DEBUG, __FILE__, __LINE__);
    racoon::LogEvent log_event_info(racoon::INFO, __FILE__, __LINE__);
    racoon::LogEvent log_event_warn(racoon::WARN, __FILE__, __LINE__);
    racoon::LogEvent log_event_error(racoon::ERROR, __FILE__, __LINE__);
    EXPECT_EQ(log_event_debug.GetLogLevel(), racoon::DEBUG);
    EXPECT_EQ(log_event_info.GetLogLevel(), racoon::INFO);
    EXPECT_EQ(log_event_warn.GetLogLevel(), racoon::WARN);
    EXPECT_EQ(log_event_error.GetLogLevel(), racoon::ERROR);
}

TEST_F(LogEventTest, GetFilename) {
    racoon::LogEvent log_event_debug(racoon::DEBUG, "debug", __LINE__);
    racoon::LogEvent log_event_info(racoon::INFO, "info", __LINE__);
    racoon::LogEvent log_event_warn(racoon::WARN, "warn", __LINE__);
    racoon::LogEvent log_event_error(racoon::ERROR, "error", __LINE__);
    EXPECT_EQ(log_event_debug.GetFilename(), "debug");
    EXPECT_EQ(log_event_info.GetFilename(), "info");
    EXPECT_EQ(log_event_warn.GetFilename(), "warn");
    EXPECT_EQ(log_event_error.GetFilename(), "error");
}

class AsyncLoggerTest : public testing::Test {};

TEST_F(AsyncLoggerTest, GetInstance) {
    auto logger1_ft = std::async(std::launch::async, racoon::AsyncLogger::GetInstance);
    auto logger2_ft = std::async(std::launch::async, racoon::AsyncLogger::GetInstance);
    auto logger3_ft = std::async(std::launch::async, racoon::AsyncLogger::GetInstance);
    racoon::AsyncLogger &logger1 = logger1_ft.get();
    racoon::AsyncLogger &logger2 = logger2_ft.get();
    racoon::AsyncLogger &logger3 = logger3_ft.get();
    EXPECT_EQ(&logger1, &logger2);
    EXPECT_EQ(&logger1, &logger3);
}

TEST_F(AsyncLoggerTest, Init) {
    auto &logger = racoon::AsyncLogger::GetInstance();
    logger.Init(racoon::AsyncLogger::LOG_TYPE_STDOUT_FILE,
        "./logs", "log_test", 1<<20, racoon::DEBUG);
    logger.CloseLogger();
}

TEST(LogTest, Log) {
    auto &logger = racoon::AsyncLogger::GetInstance();
    logger.Init(racoon::AsyncLogger::LOG_TYPE_STDOUT_FILE,
        "./logs", "log_test", 1<<20, racoon::DEBUG);
    LOG_DEBUG("this is a debug log");
    LOG_DEBUG("%d dollar", 10);
    LOG_INFO("this is a info log");
    LOG_INFO("I have %.2f yuan", 105.87);
    LOG_WARN("this is a warn log");
    LOG_ERROR("this is a error log");
    logger.CloseLogger();
}