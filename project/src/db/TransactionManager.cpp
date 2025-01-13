#include "TransactionManager.hpp"
#include "yelloger.h"

int TransactionManager::add(Transaction::pointer_t tr)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_transMap.insert(transactions_t::value_type(tr->xid(), tr));
    return tr->xid();
}

void TransactionManager::remove(const std::size_t xid)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    auto tr = m_transMap.find(xid);
    if (tr != m_transMap.end())
    {
        if (tr->second->isOpen())
        {
            Yellog::Error("Transaction with xid '%s' has been terminated", xid);
            tr->second->terminate();
        }
        m_transMap.erase(xid);
    }    
}

Transaction::pointer_t TransactionManager::getTransaction(const std::size_t xid) const 
{
    const auto &trIter = m_transMap.find(xid);
    if (trIter != m_transMap.end())
    {
        return trIter->second;
    }
    else
    {
        std::string error = "Internal Logic Error. This transaction " + std::to_string(xid) + "does not exist";
        throw std::runtime_error(error.c_str());
    }
}