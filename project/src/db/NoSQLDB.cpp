#include "NoSQLDB.hpp"

bool NoSqlDB::registerUser(const std::string &username, const std::string &pwd)
{
    return m_authManager.registerUser(username, pwd);
}

NoSqlDB::SessionHandler NoSqlDB::login(const std::string &username, const std::string &pwd)
{
    const auto loginInfo = m_authManager.login(username, pwd);
    if (!loginInfo.has_value())
    {
        return std::nullopt;
    }
    const auto sessionId = m_sessionManager.createSession(loginInfo.value(), m_dataStore);
    return SessionHandler(sessionId);
}

bool NoSqlDB::beginTransaction(const NoSqlDB::SessionHandler &sessionHandle)
{
    const auto sessionRes = m_sessionManager.getSession(sessionHandle.value());
    if (!sessionRes.has_value())
    {
        std::cerr << "Cannot begin transaction. Auth error" << std::endl;
        ;
        return false;
    }
    auto session = sessionRes.value();
    session->createTransaction();
    return true;
}

bool NoSqlDB::execInsert(const NoSqlDB::SessionHandler &sessionHandle,
                         std::shared_ptr<IBasicDBInputObject> insertData)
{
    const auto sessionRes = m_sessionManager.getSession(sessionHandle.value());
    if (!sessionRes.has_value())
    {
        std::cerr << "Cannot Insert data. Auth error" << std::endl;
        ;
        return false;
    }
    auto session = sessionRes.value();
    return session->insert(insertData);
}

std::vector<std::shared_ptr<BasicDBObject>> NoSqlDB::select(const NoSqlDB::SessionHandler &sessionHandle,
                                 std::shared_ptr<IBasicDBInputObject> whereData)
{
    const auto sessionRes = m_sessionManager.getSession(sessionHandle.value());
    if (!sessionRes.has_value())
    {
        std::cerr << "Cannot Select data. Auth error" << std::endl;
        return std::vector<std::shared_ptr<BasicDBObject>>();
    }
    auto session = sessionRes.value();
    return session->select(whereData);
}

bool NoSqlDB::execUpdate(const NoSqlDB::SessionHandler &sessionHandle,
                         std::shared_ptr<IBasicDBInputObject> updateData)
{
    const auto sessionRes = m_sessionManager.getSession(sessionHandle.value());
    if (!sessionRes.has_value())
    {
        std::cerr << "Cannot Update data. Auth error" << std::endl;
        ;
        return false;
    }
    auto session = sessionRes.value();
    return session->update(updateData);
}

bool NoSqlDB::execDelete(const NoSqlDB::SessionHandler &sessionHandle,
                         std::shared_ptr<IBasicDBInputObject> deleteData)
{
    const auto sessionRes = m_sessionManager.getSession(sessionHandle.value());
    if (!sessionRes.has_value())
    {
        std::cerr << "Cannot Update data. Auth error" << std::endl;
        ;
        return false;
    }
    auto session = sessionRes.value();
    return session->deleteExec(deleteData);
}
// db.select();

void NoSqlDB::commitTransaction(const NoSqlDB::SessionHandler &sessionHandle)
{
    const auto sessionRes = m_sessionManager.getSession(sessionHandle.value());
    if (!sessionRes.has_value())
    {
        std::cerr << "Cannot begin transaction. Auth error" << std::endl;
        return;
    }

    auto session = sessionRes.value();
    return session->commit();
}

void NoSqlDB::exit(const NoSqlDB::SessionHandler &sessionHandle)
{
    const auto sessionRes = m_sessionManager.getSession(sessionHandle.value());
    if (!sessionRes.has_value())
    {
        std::cerr << "Cannot get close session. Session does not found(Auth error)" << std::endl;
        ;
        return;
    }

    m_sessionManager.closeSession(sessionHandle.value());
    std::cout << "End session with id '" << sessionHandle.value() << "'." << std::endl;
}