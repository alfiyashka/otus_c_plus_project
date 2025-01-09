#ifndef _NOSQL_DB_HPP_
#define _NOSQL_DB_HPP_

#include "SessionManager.hpp"
#include "AuthManager.hpp"
#include "TransactionManager.hpp"
#include "Transaction.hpp"
#include "../src/model/BasicDBInputObject.hpp"
#include "../src/model/ComplexDBObject.hpp"
#include "DBStore.hpp"

#include <string>
#include <optional>


class NoSqlDB
{
    TransactionManager m_transactionManager;
    SessionManager m_sessionManager;
    AuthManager m_authManager;
    
    DBStore m_dataStore;
   

public:

    NoSqlDB(): m_sessionManager(m_transactionManager)
    {
        
    }

    using SessionHandler = std::optional<std::size_t>;

    bool registerUser(const std::string& username, const std::string& pwd);

    SessionHandler login(const std::string& username, const std::string& pwd);
    

    bool beginTransaction(const SessionHandler& sessionHandle);

    bool execInsert(const SessionHandler& sessionHandle,
        std::shared_ptr<IBasicDBInputObject> insertData);

    std::vector<std::shared_ptr<BasicDBObject>> select(const SessionHandler& sessionHandle, 
        std::shared_ptr<IBasicDBInputObject> whereData);

    bool execUpdate(const SessionHandler& sessionHandle,
        std::shared_ptr<IBasicDBInputObject> updateData);

    bool execDelete(const SessionHandler& sessionHandle,
        std::shared_ptr<IBasicDBInputObject> deleteData);

    void commitTransaction(const SessionHandler& sessionHandle);

    void exit(const SessionHandler& sessionHandle);

};

#endif