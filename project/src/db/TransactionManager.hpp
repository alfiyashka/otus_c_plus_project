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
    using transactions_t = std::map<std::size_t, Transaction::pointer_t>;

    int add(Transaction::pointer_t tr);

    void remove(const std::size_t xid);

    Transaction::pointer_t getTransaction(const std::size_t xid) const;

    const transactions_t& all_transactions() const { return m_transMap; }

private:
    transactions_t m_transMap;
    std::size_t m_xidGenerator;
    std::mutex m_mutex;
};


#endif