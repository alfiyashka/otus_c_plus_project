#include "../db/IdGenerator.hpp"
#include <gtest/gtest.h>
#include <string>
#include <mutex>
#include <thread>
#include <set>

TEST(id_generator_test_case, test1)
{
    auto id = IDGenerator::getGenerator()->generateId();
    auto id2 = IDGenerator::getGenerator()->generateId();
    EXPECT_EQ(id, id2 - 1);
}

TEST(id_generator_test_case, test2_threads)
{
    std::set<size_t> ids;
    std::mutex myMutex;
    auto threadWork = [&ids, &myMutex]() -> void
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto id = IDGenerator::getGenerator()->generateId();
        
        {
            std::lock_guard<std::mutex> lock(myMutex);
            ids.insert(id);
        }
    };
    const int threadsCount = 10;
    std::vector<std::thread> threads;
    for (int i = 0; i< threadsCount; i++)
    {
        threads.emplace_back(std::thread(threadWork));
    }

    for (int i = 0; i< threadsCount; i++)
    {
        if (threads[i].joinable())
            threads[i].join();
    }
    EXPECT_EQ(ids.size(), threadsCount);
    
}

TEST(xid_generator_test_case, test1)
{
    auto id = XIDGenerator::getGenerator()->generateId();
    auto id2 = XIDGenerator::getGenerator()->generateId();
    EXPECT_EQ(id, id2 - 1);
}


TEST(xid_generator_test_case, test2_threads)
{
    std::set<size_t> ids;
    std::mutex myMutex;
    auto threadWork = [&ids, &myMutex]() -> void
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto id = XIDGenerator::getGenerator()->generateId();
        
        {
            std::lock_guard<std::mutex> lock(myMutex);
            ids.insert(id);
        }
    };
    const int threadsCount = 10;
    std::vector<std::thread> threads;
    for (int i = 0; i< threadsCount; i++)
    {
        threads.emplace_back(std::thread(threadWork));
    }

    for (int i = 0; i< threadsCount; i++)
    {
        if (threads[i].joinable())
            threads[i].join();
    }
    EXPECT_EQ(ids.size(), threadsCount);
    
}
