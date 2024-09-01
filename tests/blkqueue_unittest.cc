/**
 * @file blkqueue_unittest.cc
 * @author Fansure Grin
 * @date 2024-09-01
 * @brief Unit test for class `BlockingQueue`
 */
#include <algorithm>
#include <gtest/gtest.h>
#include <thread>
#include <future>
#include <set>
#include "../src/racoon/blocking_queue/blocking_queue.hpp"


TEST(BlockingQueueTest, PushAndPop) {
    racoon::BlockingQueue<int> queue;
    std::set<int> const s1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::set<int> const s2 = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    
    std::thread pro1([&] {
        for (const auto &i : s1) {
            queue.push(i);
            std::this_thread::yield();
        }
    });
    
    std::thread pro2([&] {
        for (const auto &i : s2) {
            queue.push(i);
            std::this_thread::yield();
        }
    });

    auto consume = [&]{
        std::set<int> res;
        int val;
        while (!queue.empty()) {
            queue.pop(val);
            res.insert(val);
            std::this_thread::yield();
        }
        return res;
    };

    auto res1_ft = std::async(std::launch::async, consume);
    auto res2_ft = std::async(std::launch::async, consume);
    auto res3_ft = std::async(std::launch::async, consume);

    pro1.join(), pro2.join();

    auto const res1 = res1_ft.get();
    auto const res2 = res2_ft.get();
    auto const res3 = res3_ft.get();

    std::vector<int> data_pushed;
    for (const auto &i : s1) { data_pushed.push_back(i); }
    for (const auto &i : s2) { data_pushed.push_back(i); }
    std::sort(data_pushed.begin(), data_pushed.end());

    std::vector<int> data_poped;
    for (const auto &i : res1) { data_poped.push_back(i); }
    for (const auto &i : res2) { data_poped.push_back(i); }
    for (const auto &i : res3) { data_poped.push_back(i); }
    std::sort(data_poped.begin(), data_poped.end());

    EXPECT_EQ(data_pushed, data_poped);
}