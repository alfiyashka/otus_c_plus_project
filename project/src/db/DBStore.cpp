#include "DBStore.hpp"

void DBStore::insert(const tempDbStoreType &tempDataStore)
{
    if (tempDataStore.size() == 0)
    {
        return;
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
                m_dataStore.insert(BasicDBObject::dataStore_t::value_type(insert->id(), insert));
            }
        }
        case JOB_TYPE::UPDATE:
        {
            // delete + insert
            for (const auto update : tempData.second.second)
            {
                m_dataStore.erase(update->id());
                m_dataStore.insert(BasicDBObject::dataStore_t::value_type(update->id(), update));
            }
        }
        case JOB_TYPE::DELETE:
        {
            for (const auto deleteItem : tempData.second.second)
            {
                m_dataStore.erase(deleteItem->id());
            }
        }
        }
    }
}

const BasicDBObject::dataStore_t &DBStore::select() const
{
    return m_dataStore;
}

bool DBStore::empty() const
{
    return m_dataStore.empty();
}
