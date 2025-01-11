#include "DBStore.hpp"
#include <iostream>

bool DBStore::insert(const tempDbStoreType &tempDataStore)
{
    if (tempDataStore.size() == 0)
    {
        return true;
    }
    // thread save commit
    std::unique_lock<std::mutex> lock(m_mutex);
    for (const auto &tempData : tempDataStore)
    {
        const JOB_TYPE jobType = tempData.second.first;
        switch (jobType)
        {
            case JOB_TYPE::INSERT:
            {
                for (const auto insert : tempData.second.second)
                {
                    if (dublicateExists(insert->id()))
                    {
                        std::cerr<<"Cannot insert data with id '" << insert->id()
                             << "'. Because the data with same id exists" << std::endl;
                        return false;
                    }
                    m_dataStore.insert(BasicDBObject::dataStore_t::value_type(insert->id(), insert));
                }
                break;
            }
            case JOB_TYPE::UPDATE:
            {
                // delete + insert
                for (const auto update : tempData.second.second)
                {
                    if (!dublicateExists(update->id()))
                    {
                        continue; //the data to update does not exist
                    }
                    m_dataStore.erase(update->id());
                    m_dataStore.insert(BasicDBObject::dataStore_t::value_type(update->id(), update));
                }
                break;
            }
            case JOB_TYPE::DELETE:
            {
                for (const auto deleteItem : tempData.second.second)
                {
                    m_dataStore.erase(deleteItem->id());
                }
                break;
            }
        }
    }
    return true;
}

const BasicDBObject::dataStore_t &DBStore::select() const
{
    return m_dataStore;
}

bool DBStore::empty() const
{
    return m_dataStore.empty();
}

bool DBStore::dublicateExists(const std::size_t id) const
{
    return m_dataStore.find(id) != m_dataStore.end();
}
