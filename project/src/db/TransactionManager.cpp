#include "TransactionManager.hpp"

int TransactionManager::add(std::shared_ptr<Transaction> tr)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_transMap.insert(transactions_t::value_type(tr->xid(), tr));
    return tr->xid();
}

void TransactionManager::commit(const std::size_t xid)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_transMap.erase(xid);
}

auto TransactionManager::getTransaction(const std::size_t xid) const 
{
    const auto &sessionIter = m_transMap.find(xid);
    if (sessionIter != m_transMap.end())
    {
        sessionIter->second;
    }
    else
    {
        std::string error = "Internal Logic Error. This session " + std::to_string(xid);
        throw std::runtime_error(error.c_str());
    }
}