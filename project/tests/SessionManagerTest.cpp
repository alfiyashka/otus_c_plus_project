#include <gtest/gtest.h>
#include "../db/SessionManager.hpp"
#include <memory>


TEST(session_manager_test_case, test_create_session)
{
    TransactionManager trManager;
    DBStore dbStore;
    SessionManager sessionManager(trManager, dbStore);
    
    IAuth::pointer_t auth(new Auth("user", "pwd"));
    const auto sessionHandler = sessionManager.createSession(auth);

    EXPECT_TRUE(sessionHandler > 0);
    EXPECT_TRUE(sessionManager.getSession(sessionHandler).has_value());
}

TEST(session_manager_test_case, test_remove_session)
{
    TransactionManager trManager;
    DBStore dbStore;
    SessionManager sessionManager(trManager, dbStore);
    
    IAuth::pointer_t auth(new Auth("user", "pwd"));
    const auto sessionHandler = sessionManager.createSession(auth);

    sessionManager.closeSession(sessionHandler);

    EXPECT_FALSE(sessionManager.getSession(sessionHandler).has_value());
}

TEST(session_manager_test_case, test_create_session_threads)
{
    TransactionManager trManager;
    DBStore dbStore;
    SessionManager sessionManager(trManager, dbStore);
    
    auto sessionCreaterWork = [&sessionManager] () 
    {
        IAuth::pointer_t auth(new Auth("user", "pwd"));
        sessionManager.createSession(auth);
    };

    std::vector<std::thread> threads;
    const int threadsCount = 100;
    for (int i = 0; i < threadsCount; i++)
    {
        threads.emplace_back(std::thread(sessionCreaterWork));
    }

    for (int i = 0; i < threadsCount; i++)
    {
        if (threads[i].joinable())
        {
            threads[i].join();
        }        
    }

    EXPECT_TRUE(sessionManager.all_sessions().size() == threadsCount);
}

