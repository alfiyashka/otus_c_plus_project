#include "SessionManager.hpp"

int SessionManager::createSession(std::shared_ptr<IAuth> auth)
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
        std::cerr << "Internal Logic Error. This session " << sessionId << std::endl;
        return std::nullopt;
    }
}
