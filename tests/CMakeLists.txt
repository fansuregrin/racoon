cmake_minimum_required(VERSION 3.14)
project(racoon_test)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(GTest REQUIRED)
find_package(tinyxml2 REQUIRED)

enable_testing()

add_executable(config_unittest
  config_unittest.cc
  ../src/racoon/config/config.cc
) 
target_link_libraries(config_unittest
  GTest::gtest_main
  tinyxml2
)

add_executable(util_unittest
  util_unittest.cc
  ../src/racoon/util/util.cc
)
target_link_libraries(util_unittest
  GTest::gtest_main
)

add_executable(log_unittest
  log_unittest.cc
  ../src/racoon/util/util.cc
  ../src/racoon/log/log.cc
)
target_link_libraries(log_unittest
  GTest::gtest_main
)

add_executable(blkqueue_unittest
  blkqueue_unittest.cc
)
target_link_libraries(blkqueue_unittest
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(config_unittest)
gtest_discover_tests(util_unittest)
gtest_discover_tests(log_unittest)
gtest_discover_tests(blkqueue_unittest)

file(COPY ../conf/racoon.xml DESTINATION ${PROJECT_BINARY_DIR})