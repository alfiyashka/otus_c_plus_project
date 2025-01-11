#include "Job.hpp"

bool Job::whereClauseCompareTo(std::shared_ptr<IBasicDBWhereObject> whereData, BasicDBObject::pointer_t value) const
{
    if (!whereData->nameWhere().empty()
        && !whereData->dataWhere().get()
        && whereData->typeWhere() == Datatype::UNDEFINED)
    {
        // find by name
        if (whereData->nameWhere() == value->nameRedo())
        {
            return true;
        }
        return false;
    }
    else if (whereData->typeWhere() == Datatype::COMPOSITE)
    {
        if (!whereData->dataWhere().get()
           && whereData->nameWhere().empty())
        {
            if (whereData->typeWhere() == value->typeRedo())
            {
                return true;
            }
            return false;
        }
        else if (!whereData->dataWhere().get()
           && !whereData->nameWhere().empty())
        {
            // check name and type
            if (whereData->typeWhere() == value->typeRedo()
               && whereData->nameWhere() == value->nameRedo())
            {
                return true;
            }
            return false;
        }
        else if (whereData->dataWhere().get()
            && whereData->nameWhere().empty())
        {
            // check name and type and data
            if (whereData->typeWhere() == value->typeRedo()
               && whereData->nameWhere() == value->nameRedo())
            {
                ComplexDBObject::pointer_t whereComplex
                    = *static_cast<ComplexDBObject::pointer_t*>(whereData->dataWhere().get());
                ComplexDBObject::pointer_t valueComplex 
                    = std::dynamic_pointer_cast<ComplexDBObject>(value);
                
                if (whereComplex->getChildrens().size() != valueComplex->getChildrens().size())
                {
                    return false;
                }
                for (auto child : whereComplex->getChildrens())
                {
                    auto findValueChild = valueComplex->getChildrens().find(child.first);
                    if (findValueChild == valueComplex->getChildrens().end())
                    {
                        return false;
                    }
                    BasicDBObject::pointer_t valueChild = findValueChild->second;
                    std::shared_ptr<IBasicDBWhereObject> whereChild 
                        = std::dynamic_pointer_cast<IBasicDBWhereObject>(child.second);
                    return whereClauseCompareTo(whereChild, valueChild);
                }

            }
            return false;
        }
        return true;
    }
    else if (!whereData->nameWhere().empty() 
        && !whereData->dataWhere().get() 
        && whereData->typeWhere() != Datatype::UNDEFINED)
    {
        // find by name and type
        if (whereData->nameWhere() == value->nameRedo() && whereData->typeWhere() == value->typeRedo())
        {
            return true;
        }
    }
    else if (whereData->nameWhere().empty() && !whereData->dataWhere().get() && whereData->typeWhere() != Datatype::UNDEFINED)
    {
        // find by only type
        if (whereData->typeWhere() == value->typeRedo())
        {
            return true;
        }
    }
    else if (whereData->nameWhere().empty() && whereData->dataWhere().get() && whereData->typeWhere() != Datatype::UNDEFINED)
    {
        // find by data
        if (whereData->typeWhere() == value->typeRedo() && DataComparator::isPlainDataEqual(std::tuple(whereData->typeWhere(), whereData->dataWhere()),
                                                                                  std::tuple(value->typeRedo(), value->dataRedo())))
        {
            return true;
        }
    }
    else if (!whereData->nameWhere().empty() && whereData->dataWhere().get() && whereData->typeWhere() != Datatype::UNDEFINED)
    {
        if (whereData->nameWhere() == value->nameRedo() && whereData->typeWhere() == value->typeRedo() && DataComparator::isPlainDataEqual(std::tuple(whereData->typeWhere(), whereData->dataWhere()), std::tuple(value->typeRedo(), value->dataRedo())))
        {
            return true;
        }
    }
    return false;
}

void InsertJob::run()
{
    BasicDBObject::pointer_t data;
    if (m_insertData->typeRedo() == Datatype::COMPOSITE)
    {
        data = BasicDBObject::pointer_t(new ComplexDBObject(
            m_insertData->nameRedo(), m_insertData->dataRedo()));
        data->parent(m_insertData->parentRedo());        
    }
    else
    {
        data = BasicDBObject::pointer_t(new BasicDBObject(
            m_insertData->typeRedo(), m_insertData->nameRedo(), m_insertData->dataRedo()));
    }
    std::list<BasicDBObject::pointer_t> tempFindRes;
    tempFindRes.push_back(data);
    m_tempDataStore.insert(
        tempDbStoreType::value_type(m_sequence,
                                    std::pair(m_type, tempFindRes)));

    std::cout << "Inserted data \n";
}

bool DeleteJob::compareToDelete(BasicDBObject::pointer_t value) const
{
    return whereClauseCompareTo(m_whereData, value);
}

void DeleteJob::run()
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
                                  if (value.second.first == JOB_TYPE::DELETE)
                                  {
                                      for (auto data : value.second.second)
                                      {
                                          findRes.erase(data->id());
                                      }
                                      return findRes;
                                  }
                                  for (auto data : value.second.second)
                                  {
                                      if (compareToDelete(data))
                                      {
                                          if (findRes.find(data->id()) != findRes.end())
                                          {
                                              findRes.erase(data->id());
                                          }
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
    if (!tempFindRes.empty())
    {
        m_tempDataStore.insert(
            tempDbStoreType::value_type(m_sequence, std::pair(m_type, tempFindRes)));
    }

    std::cout << "Deleted data :" << tempFindRes.size() << " records" << std::endl;
}

void UpdateJob::updateData(BasicDBObject::pointer_t data) const
{
    if (!m_updateData->nameRedo().empty())
    {
        data->updateName(m_updateData->nameRedo());
    }
    if (m_updateData->dataRedo().get())
    {
        if (m_updateData->typeRedo() == Datatype::COMPOSITE)
        {
            if (data->typeRedo() != Datatype::COMPOSITE)
            {
                std::cerr << "Cannot convert plain data to composite data. Id is '" << data->id() << "' \n";
                return;
            }
            try
            {
                ComplexDBObject::pointer_t composite = std::dynamic_pointer_cast<ComplexDBObject>(data);
                ComplexDBObject* compositeUpdate = (reinterpret_cast<ComplexDBObject*>(m_updateData->dataRedo().get()));
                
                composite->updateData(compositeUpdate->dataRedo());
                composite->updateChilds(compositeUpdate->getChildrens());
            }
            catch(const std::bad_alloc& e)
            {
                std::cerr << "Cannot update composite data. Incorrect input redo. Id is '" << data->id() << "' \n";
            }
        }
        else
        {
            data->updateData(m_updateData->dataRedo());
            if (m_updateData->typeRedo() != Datatype::UNDEFINED)
            {
                data->updateType(m_updateData->typeRedo());
            }            
        }

    }
}

void UpdateJob::run()
{
    if (m_dataStore.empty() && m_tempDataStore.empty())
    {
        std::cout << "No data to update. Database is empty" << std::endl;
        return;
    }

    BasicDBObject::dataStore_t findRes;

    findRes = std::reduce(std::execution::par,
                          m_dataStore.select().begin(), m_dataStore.select().end(), findRes,
                          [this](BasicDBObject::dataStore_t &findRes, BasicDBObject::dataStore_t::value_type value)
                          {
                              if (whereClauseCompareTo(std::dynamic_pointer_cast<IBasicDBWhereObject>(m_updateData), value.second))
                              {
                                  auto data = value.second;
                                  updateData(data);
                                  findRes.insert(BasicDBObject::dataStore_t::value_type(data->id(), data));
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
                                  switch (value.second.first)
                                  {
                                  case JOB_TYPE::DELETE:
                                  {
                                      for (auto data : value.second.second)
                                      {
                                          findRes.erase(data->id());
                                      }
                                      return findRes;
                                  }
                                  case JOB_TYPE::UPDATE:
                                  {
                                      for (auto data : value.second.second)
                                      {
                                          findRes.erase(data->id());
                                          if (whereClauseCompareTo(std::dynamic_pointer_cast<IBasicDBWhereObject>(m_updateData), data))
                                          {
                                              updateData(data);
                                              findRes.insert(BasicDBObject::dataStore_t::value_type(data->id(), data));
                                          }
                                      }
                                      return findRes;
                                  }
                                  default:
                                  {
                                      for (auto data : value.second.second)
                                      {
                                          if (whereClauseCompareTo(std::dynamic_pointer_cast<IBasicDBWhereObject>(m_updateData), data))
                                          {
                                              updateData(data);
                                              findRes.insert(BasicDBObject::dataStore_t::value_type(data->id(), data));
                                          }
                                      }
                                      return findRes;
                                  }
                                  }
                              });
    }

    tempDbStoreType::value_type::second_type::second_type tempFindRes;
    for (auto res : findRes)
    {
        tempFindRes.push_back(res.second);
    }
    if (!tempFindRes.empty())
    {
        m_tempDataStore.insert(
            tempDbStoreType::value_type(m_sequence,
                                        std::pair(m_type, tempFindRes)));
    }

    std::cout << "Updated data :" << tempFindRes.size() << " records" << std::endl;
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
            && !m_whereData.get()->dataWhere().get()
            && m_whereData->typeWhere() == Datatype::UNDEFINED
            && m_whereData->nameWhere().empty()) // select all if where clause is empty
    {
        return selectAll();
    }
    BasicDBObject::pointer_t data;
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
                              if (whereClauseCompareTo(m_whereData, value.second))
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
                                  switch (value.second.first)
                                  {
                                  case JOB_TYPE::DELETE:
                                  {
                                      for (auto data : value.second.second)
                                      {
                                          findRes.erase(data->id());
                                      }
                                      return findRes;
                                  }
                                  case JOB_TYPE::UPDATE:
                                  {
                                      for (auto data : value.second.second)
                                      {
                                          findRes.erase(data->id());
                                          if (whereClauseCompareTo(m_whereData, data))
                                          {
                                              findRes.insert(BasicDBObject::dataStore_t::value_type(data->id(), data));
                                          }
                                      }
                                      return findRes;
                                  }
                                  default:
                                  {
                                      for (auto data : value.second.second)
                                      {
                                          if (whereClauseCompareTo(m_whereData, data))
                                          {
                                              findRes.insert(BasicDBObject::dataStore_t::value_type(data->id(), data));
                                          }
                                      }
                                      return findRes;
                                  }
                                  }
                              });
    }

    for (const auto data : findRes)
    {
        m_selectRes.insert(data.second);
    }

    std::cout << "Selected data :" << m_selectRes.size() << " records" << std::endl;
}