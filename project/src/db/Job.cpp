#include "Job.hpp"

bool Job::whereClouseCompareTo(std::shared_ptr<IBasicDBInputObject> whereData, std::shared_ptr<BasicDBObject> value) const
{
    if (!whereData->nameUndo().empty() && !whereData->dataUndo().get() && whereData->typeUndo() == Datatype::UNDEFINED)
    {
        // find by name
        if (whereData->nameUndo() == value->nameRedo())
        {
            return true;
        }
    }
    if (!whereData->nameUndo().empty() && !whereData->dataUndo().get() && whereData->typeUndo() != Datatype::UNDEFINED)
    {
        // find by name and type
        if (whereData->nameUndo() == value->nameRedo() && whereData->typeUndo() == value->typeRedo())
        {
            return true;
        }
    }
    else if (whereData->nameUndo().empty() && !whereData->dataUndo().get() && whereData->typeUndo() != Datatype::UNDEFINED)
    {
        // find by only type
        if (whereData->typeUndo() == value->typeRedo())
        {
            return true;
        }
    }
    else if (whereData->nameUndo().empty() && whereData->dataUndo().get() && whereData->typeUndo() != Datatype::UNDEFINED)
    {
        // find by data
        if (whereData->typeUndo() == value->typeRedo() && DataComparator::compare(std::tuple(whereData->typeUndo(), whereData->dataUndo()),
                                                                                  std::tuple(value->typeRedo(), value->dataRedo())))
        {
            return true;
        }
    }
    else if (!whereData->nameUndo().empty() && whereData->dataUndo().get() && whereData->typeUndo() != Datatype::UNDEFINED)
    {
        if (whereData->nameUndo() == value->nameRedo() && whereData->typeUndo() == value->typeRedo() && DataComparator::compare(std::tuple(whereData->typeUndo(), whereData->dataUndo()), std::tuple(value->typeRedo(), value->dataRedo())))
        {
            return true;
        }
    }
    return false;
}

void InsertJob::run()
{
    std::shared_ptr<BasicDBObject> data;
    if (m_insertData->typeRedo() == Datatype::COMPOSITE)
    {
        data = std::shared_ptr<BasicDBObject>(new ComplexDBObject(
            m_insertData->nameRedo(), m_insertData->dataRedo()));
    }
    else
    {
        data = std::shared_ptr<BasicDBObject>(new BasicDBObject(
            m_insertData->typeRedo(), m_insertData->nameRedo(), m_insertData->dataRedo()));
    }
    std::list<std::shared_ptr<BasicDBObject>> tempFindRes;
    tempFindRes.push_back(data);
    m_tempDataStore.insert(
        tempDbStoreType::value_type(m_sequence,
                                    std::pair(m_type, tempFindRes)));

    std::cout << "Inserted data \n";
}

bool DeleteJob::compareToDelete(std::shared_ptr<BasicDBObject> value) const
{
    return whereClouseCompareTo(m_deleteData, value);
}

void DeleteJob::run()
{
    std::shared_ptr<BasicDBObject> data;
    if (m_deleteData->typeRedo() == Datatype::COMPOSITE)
    {
        data = std::shared_ptr<BasicDBObject>(new ComplexDBObject(
            m_deleteData->nameRedo(), m_deleteData->dataRedo()));
    }
    else
    {
        if (m_dataStore.empty() && m_tempDataStore.empty())
        {
            std::cout << "No data to delete. Database is empty" << std::endl;
            return;
        }

        BasicDBObject::dataStore_t findRes;

        findRes = std::reduce(std::execution::par,
                              m_dataStore.select().begin(), m_dataStore.select().end(), findRes,
                              [this](BasicDBObject::dataStore_t &findRes, BasicDBObject::dataStore_t::value_type value)
                              {
                                  if (compareToDelete(value.second))
                                  {
                                      findRes.insert(value);
                                  }

                                  return findRes;
                              });

        if (!m_tempDataStore.empty())
        {
            findRes = std::reduce(std::execution::par,
                                  m_tempDataStore.begin(), m_tempDataStore.end(), findRes,
                                  [this](BasicDBObject::dataStore_t &findRes,
                                         const tempDbStoreType::value_type &value)
                                  {
                                      for (auto data : value.second.second)
                                      {
                                          if (compareToDelete(data))
                                          {
                                              findRes.insert(BasicDBObject::dataStore_t::value_type(data->id(), data));
                                          }
                                      }
                                      return findRes;
                                  });
        }

        tempDbStoreType::value_type::second_type::second_type tempFindRes;
        for (auto res : findRes)
        {
            tempFindRes.push_back(res.second);
        }

        m_tempDataStore.insert(
            tempDbStoreType::value_type(m_sequence, std::pair(m_type, tempFindRes)));

        std::cout << "Deleted data :" << tempFindRes.size() << " records" << std::endl;
    }
}

void UpdateJob::run()
{
    if (m_updateData->typeRedo() == Datatype::COMPOSITE)
    {
        std::shared_ptr<BasicDBObject> data = std::shared_ptr<BasicDBObject>(new ComplexDBObject(
            m_updateData->nameRedo(), m_updateData->dataRedo()));
    }
    else
    {
        if (m_dataStore.empty() && m_tempDataStore.empty())
        {
            std::cout << "No data to delete. Database is empty" << std::endl;
            return;
        }

        BasicDBObject::dataStore_t findRes;

        findRes = std::reduce(std::execution::par,
                              m_dataStore.select().begin(), m_dataStore.select().end(), findRes,
                              [this](BasicDBObject::dataStore_t &findRes, BasicDBObject::dataStore_t::value_type value)
                              {
                                  if (whereClouseCompareTo(m_updateData, value.second))
                                  {
                                      findRes.insert(value);
                                  }

                                  return findRes;
                              });
        std::list<std::shared_ptr<BasicDBObject>> tempFindRes;
        if (!m_tempDataStore.empty())
        {
            tempFindRes = std::reduce(std::execution::par,
                                      m_tempDataStore.begin(), m_tempDataStore.end(), tempFindRes,
                                      [this](std::list<std::shared_ptr<BasicDBObject>> &tempFindRes,
                                             const tempDbStoreType::value_type &value)
                                      {
                                          for (auto data : value.second.second)
                                          {
                                              if (whereClouseCompareTo(m_updateData, data))
                                              {
                                                  tempFindRes.push_back(data);
                                              }
                                          }
                                          return tempFindRes;
                                      });
        }

        // try delete dublicates by id
        if (!tempFindRes.empty() && !findRes.empty())
        {
            for (auto tempRes : tempFindRes)
            {
                findRes.erase(tempRes->id());
            }
        }
        for (auto res : findRes)
        {
            tempFindRes.push_back(res.second);
        }
        // update temp data
        for (auto res : tempFindRes)
        {
            if (!m_updateData->nameRedo().empty())
            {
                res->updateName(m_updateData->nameRedo());
            }
            if (m_updateData->dataRedo().get())
            {
                res->updateData(m_updateData->dataRedo());
                if (m_updateData->typeRedo() != Datatype::UNDEFINED)
                {
                    res->updateType(m_updateData->typeRedo());
                }
            }
        }

        m_tempDataStore.insert(
            tempDbStoreType::value_type(m_sequence,
                                        std::pair(m_type, tempFindRes)));
        std::cout << "Updated data :" << tempFindRes.size() << " records" << std::endl;
    }
}

void CommitJob::run()
{
    m_dataStore.insert(m_tempDataStore);
    m_tempDataStore.clear();
    std::cout << "End transaction with id '" << m_xid << "'\n";
}

void SelectJob::selectAll()
{
    std::set<int> deletedIds;
    for (auto tempData = m_tempDataStore.rbegin();
         tempData != m_tempDataStore.rend();
         tempData++)
    {
        if (tempData->second.first == JOB_TYPE::DELETE)
        {
            for (const auto data : tempData->second.second)
            {
                deletedIds.insert(data->id());
            }
        }
        for (const auto data : tempData->second.second)
        {
            if (deletedIds.find(data->id()) == deletedIds.end())
            {
                m_selectRes.insert(data);
            }            
        }
    }

    for (auto storedDataIter = m_dataStore.select().begin();
         storedDataIter != m_dataStore.select().end();
         storedDataIter++)
    {
        m_selectRes.insert(storedDataIter->second);
    }
    std::cout << "Selected data :" << m_selectRes.size() << " records" << std::endl;
}

void SelectJob::run()
{
    if (!m_whereData.get()
        || m_whereData.get()
            && !m_whereData.get()->dataUndo().get()
            && m_whereData->typeUndo() == Datatype::UNDEFINED
            && m_whereData->nameUndo().empty()) // select all if where clause is empty
    {
        return selectAll();
    }
    std::shared_ptr<BasicDBObject> data;
    if (m_whereData->typeRedo() == Datatype::COMPOSITE)
    {
        data = std::shared_ptr<BasicDBObject>(new ComplexDBObject(
            m_whereData->nameRedo(), m_whereData->dataRedo()));
    }
    else
    {
        if (m_dataStore.empty() && m_tempDataStore.empty())
        {
            std::cout << "No data to select. Database is empty" << std::endl;
            return;
        }

        BasicDBObject::dataStore_t findRes;

        findRes = std::reduce(std::execution::par,
                              m_dataStore.select().begin(), m_dataStore.select().end(), findRes,
                              [this](BasicDBObject::dataStore_t &findRes, BasicDBObject::dataStore_t::value_type value)
                              {
                                  if (whereClouseCompareTo(m_whereData, value.second))
                                  {
                                      findRes.insert(value);
                                  }

                                  return findRes;
                              });

        if (!m_tempDataStore.empty())
        {
            findRes = std::reduce(std::execution::par,
                                  m_tempDataStore.begin(), m_tempDataStore.end(), findRes,
                                  [this](BasicDBObject::dataStore_t &findRes,
                                         const tempDbStoreType::value_type &value)
                                  {
                                      for (auto data : value.second.second)
                                      {
                                          if (whereClouseCompareTo(m_whereData, data))
                                          {
                                              findRes.insert(BasicDBObject::dataStore_t::value_type(data->id(), data));
                                          }
                                      }
                                      return findRes;
                                  });
        }

        for (const auto data : findRes)
        {
            m_selectRes.insert(data.second);
        }

        std::cout << "Selected data :" << m_selectRes.size() << " records" << std::endl;
    }
}