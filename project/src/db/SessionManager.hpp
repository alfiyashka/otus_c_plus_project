#ifndef _SESSION_MANAGER_HPP_
#define _SESSION_MANAGER_HPP_

#include <map>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <optional>
#include <iostream>

#include "Session.hpp"



class SessionManager
{
public:
    
    using sessions_t = std::map<std::size_t, std::shared_ptr<Session>>;

    SessionManager(TransactionManager& transactionManager): m_sessionIdGenerator(0),
        m_transactionManager(transactionManager)
    {

    }

    int createSession(std::shared_ptr<IAuth> auth, DBStore& dataStore);

    void closeSession(const std::size_t sessionId);

    std::optional<std::shared_ptr<Session>> getSession(std::size_t sessionId) const;


private:
    std::size_t generateId() 
    {
        return ++m_sessionIdGenerator;
    }

    sessions_t m_sessions;
    std::size_t m_sessionIdGenerator;
    TransactionManager& m_transactionManager;

};


#endif