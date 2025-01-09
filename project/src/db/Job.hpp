#ifndef _JOB_HPP_
#define _JOB_HPP_

#include "../model/BasicDBObject.hpp"
#include "../model/ComplexDBObject.hpp"
#include "../model/BasicDBInputObject.hpp"
#include "DBStore.hpp"
#include "DataComparator.hpp"
#include <memory>
#include <iostream>
#include <mutex>
#include <execution>
#include <list>
#include <numeric>
#include <set>

/*
class DBTempStore
{
public:
    void insert(std::shared_ptr<BasicDBObject> data, const std::size_t sequence)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::list<std::shared_ptr<BasicDBObject>> tempDataList;
        tempDataList.push_back(data);
        m_dataStore.insert(tempDbStoreType::value_type(sequence,
                                                       std::pair(JOB_TYPE::INSERT, tempDataList)));
    }
    void deleteData(std::list<std::shared_ptr<BasicDBObject>> data, const std::size_t sequence)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_dataStore.insert(tempDbStoreType::value_type(sequence,
                                                       std::pair(JOB_TYPE::DELETE, data)));
    }

    void update(std::list<std::shared_ptr<BasicDBObject>> data, const std::size_t sequence)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_dataStore.insert(tempDbStoreType::value_type(sequence,
                                                       std::pair(JOB_TYPE::UPDATE, data)));
    }

    const tempDbStoreType &select() const
    {
        return m_dataStore;
    }

private:
    tempDbStoreType m_dataStore;

    std::mutex m_mutex;
};
*/

class Job
{
protected:
    bool whereClouseCompareTo(std::shared_ptr<IBasicDBInputObject> whereData, std::shared_ptr<BasicDBObject> value) const;
    const JOB_TYPE m_type;
    const int m_sequence;
public:
    Job(const JOB_TYPE type, const int sequence): m_type(type), m_sequence(sequence) {}
    virtual ~Job() {}
    virtual void run() = 0;
    JOB_TYPE type() const noexcept { return m_type; }
    int sequence() const noexcept {return m_sequence; }
};

class InsertJob : public Job
{
     
public:
    InsertJob(const int sequence,
              tempDbStoreType &tempDataStore,
              std::shared_ptr<IBasicDBInputObject> insertData)
        : Job(JOB_TYPE::INSERT, sequence),
         m_tempDataStore(tempDataStore), m_insertData(insertData)
    {
    }

    void run() override;


private:
    tempDbStoreType &m_tempDataStore;
    std::shared_ptr<IBasicDBInputObject> m_insertData;
};

class DeleteJob : public Job
{
public:
    DeleteJob(const int sequence,
              tempDbStoreType &tempDataStore,
              const DBStore &dataStore,
              std::shared_ptr<IBasicDBInputObject> whereData)
        : Job(JOB_TYPE::DELETE, sequence), m_tempDataStore(tempDataStore), m_dataStore(dataStore), m_deleteData(whereData)
    {
    }

    bool compareToDelete(std::shared_ptr<BasicDBObject> value) const;

    void run() override;

private:
    const DBStore &m_dataStore;
    tempDbStoreType &m_tempDataStore;
    std::shared_ptr<IBasicDBInputObject> m_deleteData;
};

class UpdateJob : public Job
{
public:
    UpdateJob(const int sequence,
              tempDbStoreType &tempDataStore,
              const DBStore &dataStore,
              std::shared_ptr<IBasicDBInputObject> updateData)
        : Job(JOB_TYPE::UPDATE, sequence), m_tempDataStore(tempDataStore), m_dataStore(dataStore), m_updateData(updateData)
    {
    }

    void run() override;

private:
    const DBStore &m_dataStore;
    tempDbStoreType &m_tempDataStore;
    std::shared_ptr<IBasicDBInputObject> m_updateData;
};

class CommitJob : public Job
{
public:
    CommitJob(const int sequence,
            tempDbStoreType &tempDataStore,
            DBStore &dataStore,
            const std::size_t xid)
        : Job(JOB_TYPE::COMMIT, sequence), m_tempDataStore(tempDataStore), m_dataStore(dataStore), m_xid(xid)
    {
    }

    void run() override;

private:
    DBStore &m_dataStore;
    tempDbStoreType &m_tempDataStore;
    const std::size_t m_xid;
    std::list<std::shared_ptr<BasicDBObject>> m_selectRes;
};

struct CompareData
{
    bool operator() (const std::shared_ptr<BasicDBObject> r, const std::shared_ptr<BasicDBObject> l) const
    {
        return *r.get() < *l.get();
    }
};

class SelectJob : public Job
{
public:
    using SelectResult = std::set<std::shared_ptr<BasicDBObject>, CompareData>;
    SelectJob(const int sequence,
              tempDbStoreType &tempDataStore,
              const DBStore &dataStore,
              std::shared_ptr<IBasicDBInputObject> whereData)
        : Job(JOB_TYPE::SELECT, sequence), m_tempDataStore(tempDataStore), m_dataStore(dataStore), m_whereData(whereData)
    {
    }

    void run() override;

    const SelectResult& getResult() const { return m_selectRes; }

private:
    void selectAll();
 
    const DBStore &m_dataStore;
    tempDbStoreType &m_tempDataStore;
    std::shared_ptr<IBasicDBInputObject> m_whereData;
    SelectResult m_selectRes;
};

#endif