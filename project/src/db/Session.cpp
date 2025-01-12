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
    Transaction::pointer_t tran(new Transaction(m_dataStore));
    m_tran = tran;
    m_transactionManager.add(tran);
    m_tran.value()->begin();
}

void Session::addTransaction(Transaction::pointer_t tr)
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

bool Session::insert(InsertJob::insertData_t insertData)
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

bool Session::update(UpdateJob::updateData_t updateData)
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

bool Session::deleteExec(DeleteJob::whereData_t deleteData)
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

BasicDBObject::dataList_t Session::select(SelectJob::whereData_t whereData)
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
        m_transactionManager.remove(tr->xid());
        m_tran.reset();
    }
}

void Session::rollback()
{
    if (m_tran.has_value())
    {
        auto tr = m_tran.value();
        tr->rollback();
        m_transactionManager.remove(tr->xid());
        m_tran.reset();
    }
}
