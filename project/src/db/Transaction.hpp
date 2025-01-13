#ifndef _TRANSACTION_HPP_
#define _TRANSACTION_HPP_

#include "IdGenerator.hpp"
#include "../model/BasicDBInputObject.hpp"
#include "../model/ComplexDBObject.hpp"
#include "Job.hpp"
#include "DBStore.hpp"

#include "Utils.hpp"


#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <vector>


/**
 * defines transaction class
 */
class Transaction
{
public:
    using pointer_t = std::shared_ptr<Transaction>;
    Transaction(DBStore& dataStore)
    : m_xid(XIDGenerator::getGenerator()->generateId()),
      m_dataStore(dataStore),
      m_failed(false),
      m_isOpen(true),
      m_jobSequence(0),
      m_lastProcessedSequence(0)
    {
        m_thread = std::thread(&Transaction::transactionWork, this);
    }

    ~Transaction();
    
    /**
     * performs transaction jobs sequentially
     */
    void transactionWork();

    /**
     * open transaction
     */
    void begin();

    void commit();

    bool insert(InsertJob::insertData_t insertData);

    bool update(UpdateJob::updateData_t updateData);

    bool deleteExec(DeleteJob::whereData_t deleteData);

    BasicDBObject::selectList_t select(SelectJob::whereData_t whereData);

    void rollback();

    void terminate();
    
    bool isOpen() const { return m_isOpen; }
    std::size_t xid() const { return m_xid; }

private:  
    std::atomic<bool> m_isOpen;
    std::size_t m_xid;

    DBStore& m_dataStore;

    tempDbStoreType m_tempDataStore;

    std::mutex m_mutex;

    std::queue<std::shared_ptr<Job>> m_jobs;

    std::condition_variable m_condVar;

    bool m_failed;

    std::thread m_thread;
    std::atomic<int> m_lastProcessedSequence;

    std::atomic<int> m_jobSequence;

    
};

#endif