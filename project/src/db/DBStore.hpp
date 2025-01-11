#ifndef _DB_STORE_HPP_
#define _DB_STORE_HPP_

#include <map>
#include "../model/BasicDBObject.hpp"
#include "Utils.hpp"

#include <memory>
#include <mutex>

/**
 * Stores all db data. 
 * Insert data is thread safe
 */
class DBStore
{
public:
    /**
     * inserts data from temp data store
     * @param tempDataStore - temp data store (used to store temp changes in one transaction)
     * @return true if data inserted successfully
     */
    bool insert(const tempDbStoreType& tempDataStore);

    /**
     * selects data from data store
     * @return data from data store
     */
    const BasicDBObject::dataStore_t &select() const;

    /**
     * checks db is empty or not
     * @return true if db is empty, else false
     */
    bool empty() const;

private:
    /**
     * checks dublicates
     * @param id - data id
     * @return true, if dublicate exists, else false
     */
    bool dublicateExists(const std::size_t id) const;


    BasicDBObject::dataStore_t m_dataStore; // main data store

    std::mutex m_mutex;

};

#endif