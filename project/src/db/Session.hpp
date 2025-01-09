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
    const std::shared_ptr<Auth> m_auth;
    const std::size_t m_id;
    std::optional<std::shared_ptr<Transaction>> m_tran;
    bool m_terminate;
    bool m_isOpen;

    TransactionManager& m_transactionManager;
    DBStore& m_dataStore;
    
public:
    Session(const std::size_t id,
        std::shared_ptr<Auth> auth,
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


    void addTransaction(std::shared_ptr<Transaction> tr);

    void terminate();

    void closeSession();

    bool insert(std::shared_ptr<IBasicDBInputObject> insertData);

    bool update(std::shared_ptr<IBasicDBInputObject> updateData);

    bool deleteExec(std::shared_ptr<IBasicDBInputObject> deleteData);

    std::vector<std::shared_ptr<BasicDBObject>> select(std::shared_ptr<IBasicDBInputObject> whereData);

    void commit();

    std::size_t getId() const { return m_id; }
};


#endif