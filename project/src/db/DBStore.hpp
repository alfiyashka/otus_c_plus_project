#ifndef _DB_STORE_HPP_
#define _DB_STORE_HPP_

#include <map>
#include "../model/BasicDBObject.hpp"
#include "Utils.hpp"

#include <memory>
#include <mutex>

class DBStore
{
public:
    void insert(const tempDbStoreType& tempDataStore);

    const BasicDBObject::dataStore_t &select() const;

    bool empty() const;

private:
    BasicDBObject::dataStore_t m_dataStore;

    std::mutex m_mutex;

};

#endif