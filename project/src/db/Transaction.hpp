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



class Transaction
{
public:   

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

    void transactionWork();


    void begin();

    void commit();

    bool insert(std::shared_ptr<IBasicDBRedoObject> insertData);

    bool update(std::shared_ptr<IBasicDBRedoUndoObject> updateData);

    bool deleteExec(std::shared_ptr<IBasicDBUndoObject> deleteData);

    BasicDBObject::dataList_t select(std::shared_ptr<IBasicDBWhereObject> whereData);

    void rollback();

    void terminate();
    
    bool isOpen() const { return m_isOpen; }
    std::size_t xid() const { return m_xid; }

private:  
    bool m_isOpen;
    std::size_t m_xid;

    DBStore& m_dataStore;

    tempDbStoreType m_tempDataStore;

    std::mutex m_mutex;
    std::mutex m_selectmutex;

    std::queue<std::shared_ptr<Job>> m_jobs;

    std::condition_variable m_condVar;
    std::condition_variable m_condVarSelect;

    bool m_failed;

    std::thread m_thread;
    std::atomic<int> m_lastProcessedSequence;

    std::atomic<int> m_jobSequence;

    
};

#endif