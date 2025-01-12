#include <gtest/gtest.h>
#include "../db/TransactionManager.hpp"
#include <memory>
#include <stdexcept>


TEST(transaction_manager_test_case, test)
{
    TransactionManager trManager;
    DBStore dataStore;
    Transaction::pointer_t tr(new Transaction(dataStore));
    const auto res = trManager.add(tr);
    
    EXPECT_TRUE(res > 0);
    
    const auto find = trManager.getTransaction(tr->xid());

    EXPECT_EQ(find->xid(), tr->xid());

    tr->commit();

    trManager.remove(tr->xid());
    
    EXPECT_THROW(trManager.getTransaction(tr->xid()), std::runtime_error);
}

TEST(transaction_manager_test_case, test_create_session_threads)
{
    TransactionManager trManager;
    DBStore dataStore;

    auto transactionCreaterWork = [&trManager, &dataStore]()
    {
        Transaction::pointer_t tr(new Transaction(dataStore));
        const auto res = trManager.add(tr);

        EXPECT_TRUE(res > 0);

        const auto find = trManager.getTransaction(tr->xid());

        EXPECT_EQ(find->xid(), tr->xid());

        tr->commit();
    };

    std::vector<std::thread> threads;
    const int threadsCount = 100;
    for (int i = 0; i < threadsCount; i++)
    {
        threads.emplace_back(std::thread(transactionCreaterWork));
    }

    for (int i = 0; i < threadsCount; i++)
    {
        if (threads[i].joinable())
        {
            threads[i].join();
        }        
    }

    EXPECT_TRUE(trManager.all_transactions().size() == threadsCount);
}
