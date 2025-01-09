#include "Session.hpp"

Session::~Session()
{
    // TODO terminate transaction!

    terminate();
    if (m_tran.has_value())
    {
        m_tran.value()->terminate();
    }
}

void Session::createTransaction()
{
    std::shared_ptr<Transaction> tran(new Transaction(m_dataStore));
    m_tran = tran;
    m_transactionManager.add(tran);
    m_tran.value()->begin();
}

void Session::addTransaction(std::shared_ptr<Transaction> tr)
{
    m_tran = tr;
    tr->begin();
}

void Session::terminate()
{
    m_terminate = true;
    if (m_tran.has_value())
    {
        m_tran.value()->terminate();
    }
    m_tran.reset();
}

void Session::closeSession()
{
    m_isOpen = false;
    terminate();
    m_tran.reset();
}

bool Session::insert(std::shared_ptr<IBasicDBRedoObject> insertData)
{
    if (m_tran.has_value())
    {
        auto tr = m_tran.value();
        tr->insert(insertData);
    }
    else
    {
        createTransaction(); // like auto commit
        auto tr = m_tran.value();
        tr->insert(insertData);
        tr->commit();
    }
    return true;
}

bool Session::update(std::shared_ptr<IBasicDBRedoUndoObject> updateData)
{
    if (m_tran.has_value())
    {
        auto tr = m_tran.value();
        tr->update(updateData);
    }
    else
    {
        createTransaction(); // like auto commit
        auto tr = m_tran.value();
        tr->update(updateData);
        tr->commit();
    }
    return true;
}

bool Session::deleteExec(std::shared_ptr<IBasicDBUndoObject> deleteData)
{
    if (m_tran.has_value())
    {
        auto tr = m_tran.value();
        tr->deleteExec(deleteData);
    }
    else
    {
        createTransaction(); // like auto commit
        auto tr = m_tran.value();
        tr->deleteExec(deleteData);
        tr->commit();
    }
    return true;
}

BasicDBObject::dataList_t Session::select(std::shared_ptr<IBasicDBWhereObject> whereData)
{
    BasicDBObject::dataList_t data;
    if (m_tran.has_value())
    {
        auto tr = m_tran.value();
        data = tr->select(whereData);
    }
    else
    {
        createTransaction(); // like auto commit
        auto tr = m_tran.value();
        data = tr->select(whereData);
        tr->commit();
    }
    return data;
}

void Session::commit()
{
    if (m_tran.has_value())
    {
        auto tr = m_tran.value();
        tr->commit();
        m_tran.reset();
    }
}