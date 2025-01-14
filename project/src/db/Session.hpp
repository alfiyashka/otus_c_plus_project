#ifndef _SESSION_HPP_
#define _SESSION_HPP_

#include <thread>
#include <mutex>
#include <memory>
#include <stdexcept>

#include <condition_variable>
#include <atomic>
#include <optional>

#include "Transaction.hpp"
#include "TransactionManager.hpp"
#include "../model/ComplexDBObject.hpp"
#include "DBStore.hpp"

#include "AuthInfo.hpp"

class Session 
{
    const IAuth::pointer_t m_auth;
    const std::size_t m_id;
    std::optional<Transaction::pointer_t> m_tran;
    bool m_terminate;
    bool m_isOpen;

    TransactionManager& m_transactionManager;
    DBStore& m_dataStore;
    
public:
    Session(const std::size_t id,
        IAuth::pointer_t auth,
        TransactionManager& transactionManager,
        DBStore& dataStore)
    : m_id(id),
      m_terminate(false),
      m_auth(auth),
      m_isOpen(false),
      m_tran(std::nullopt),
      m_transactionManager(transactionManager),
      m_dataStore(dataStore)
    {
        
    }

    ~Session();

    void createTransaction();


    void addTransaction(Transaction::pointer_t tr);

    void terminate();

    void closeSession();

    bool insert(InsertJob::insertData_t insertData);

    bool update(UpdateJob::updateData_t updateData);

    bool deleteExec(DeleteJob::whereData_t deleteData);

    BasicDBObject::selectList_t select(SelectJob::whereData_t whereData);

    void commit();

    void rollback();

    std::size_t getId() const { return m_id; }
};


#endif