#ifndef _TRANSACTION_MANAGER_HPP_
#define _TRANSACTION_MANAGER_HPP_

#include <map>
#include <memory>
#include <stdexcept>
#include <mutex>

#include "Transaction.hpp"


class TransactionManager
{
public:
    using transactions_t = std::map<std::size_t, std::shared_ptr<Transaction>>;

    int add(std::shared_ptr<Transaction> tr);

    void commit(const std::size_t xid);

    auto getTransaction(const std::size_t xid) const;

private:
    transactions_t m_transMap;
    std::size_t m_xidGenerator;
    std::mutex m_mutex;
};


#endif