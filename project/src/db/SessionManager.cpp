#include "SessionManager.hpp"
#include "yelloger.h"

int SessionManager::createSession(IAuth::pointer_t auth)
{
    const auto newId = generateId();
    m_sessions.insert(
        sessions_t::value_type(newId, std::shared_ptr<Session>(new Session(newId, auth, m_transactionManager, m_dataStore))));
    return newId;
}

void SessionManager::closeSession(const std::size_t sessionId)
{
    m_sessions.erase(sessionId);
}

std::optional<std::shared_ptr<Session>> SessionManager::getSession(std::size_t sessionId) const
{
    const auto &sessionIter = m_sessions.find(sessionId);
    if (sessionIter != m_sessions.end())
    {
        return std::optional{sessionIter->second};
    }
    else
    {
        Yellog::Error("Internal Logic Error. This session '%d'", sessionId);
        return std::nullopt;
    }
}
