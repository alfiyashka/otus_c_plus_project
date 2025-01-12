#ifndef _SESSION_MANAGER_HPP_
#define _SESSION_MANAGER_HPP_

#include <map>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <optional>
#include <iostream>
#include <atomic>

#include "Session.hpp"



class SessionManager
{
public:
    
    using sessions_t = std::map<std::size_t, std::shared_ptr<Session>>;

    SessionManager(TransactionManager& transactionManager, DBStore& dataStore)
       : m_sessionIdGenerator(0)
        , m_transactionManager(transactionManager)
        , m_dataStore(dataStore)
    {

    }

    int createSession(std::shared_ptr<IAuth> auth);

    void closeSession(const std::size_t sessionId);

    std::optional<std::shared_ptr<Session>> getSession(std::size_t sessionId) const;

    const sessions_t& all_sessions() const { return m_sessions; }

private:
    std::size_t generateId() 
    {
        return ++m_sessionIdGenerator;
    }

    sessions_t m_sessions;
    std::atomic<std::size_t> m_sessionIdGenerator;
    TransactionManager& m_transactionManager;
    DBStore& m_dataStore;

};


#endif