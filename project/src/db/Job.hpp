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

/**
 * abstract class of nosql db operations
 */
class Job
{
protected:
    /**
     * helps to compare where clause data with a value from db
     * @param whereData - where clause data
     * @param value - a value from db
     * @return true if a value from db appropriate to where clause data, else - false
     */
    bool whereClauseCompareTo(std::shared_ptr<IBasicDBWhereObject> whereData, BasicDBObject::pointer_t value) const;
    const JOB_TYPE m_type;  // job type
    const int m_sequence;  // job sequence
public:
    Job(const JOB_TYPE type, const int sequence): m_type(type), m_sequence(sequence) {}
    virtual ~Job() {}

    /**
     * noSql db operation job
     */
    virtual void run() = 0;

    JOB_TYPE type() const noexcept { return m_type; }
    int sequence() const noexcept {return m_sequence; }
};

/**
 * defines insert job operation. Collects data into temp data store
 */
class InsertJob : public Job
{
     
public:
    using insertData_t = std::shared_ptr<IBasicDBRedoObject>;
    InsertJob(const int sequence,
              tempDbStoreType &tempDataStore,
              insertData_t insertData)
        : Job(JOB_TYPE::INSERT, sequence),
         m_tempDataStore(tempDataStore), m_insertData(insertData)
    {
    }

    /**
     * Collects data into temp data store
     */
    void run() override;


private:
    tempDbStoreType &m_tempDataStore; //temp data store for a transaction 
    insertData_t m_insertData; // insert data
};

/**
 * defines delete job operation. Collects data into temp data store
 */
class DeleteJob : public Job
{
public:
    using whereData_t = std::shared_ptr<IBasicDBUndoObject>;
    DeleteJob(const int sequence,
              tempDbStoreType &tempDataStore,
              const DBStore &dataStore,
              whereData_t whereData)
        : Job(JOB_TYPE::DELETE, sequence),
         m_tempDataStore(tempDataStore),
         m_dataStore(dataStore),
         m_whereData(whereData)
    {
    }

    /**
     * Collects data to delete into temp data store
     */
    void run() override;

private:

    /**
     * helps to compare where clause data with a value from db
     * @param value - a value from db
     * @return true if a value from db appropriate to where clause data, else - false
     */
    bool compareToDelete(BasicDBObject::pointer_t value) const;

    const DBStore &m_dataStore; // db store
    tempDbStoreType &m_tempDataStore; //temp data store for a transaction 
    whereData_t m_whereData; // where data
};

/**
 * defines update job operation.
 */
class UpdateJob : public Job
{
public:
    using updateData_t = std::shared_ptr<IBasicDBRedoUndoObject>;
    UpdateJob(const int sequence,
              tempDbStoreType &tempDataStore,
              const DBStore &dataStore,
              updateData_t updateData)
        : Job(JOB_TYPE::UPDATE, sequence), m_tempDataStore(tempDataStore), m_dataStore(dataStore), m_updateData(updateData)
    {
    }

    /**
     * Collects data to delete into temp data store
     */
    void run() override;

private:
    /**
     * updates data object by input m_updateData
     * @param data - data object
     */
    void updateData(BasicDBObject::pointer_t data) const;


    const DBStore &m_dataStore; // db store
    tempDbStoreType &m_tempDataStore; //temp data store for a transaction
    updateData_t m_updateData; // data need to update (has redo and undo)
};

class CommitJob : public Job
{
public:
    CommitJob(const int sequence,
            tempDbStoreType &tempDataStore,
            DBStore &dataStore,
            const std::size_t xid)
        : Job(JOB_TYPE::COMMIT, sequence), 
        m_tempDataStore(tempDataStore),
        m_dataStore(dataStore),
        m_xid(xid)
    {
    }

    void run() override;

private:
    DBStore &m_dataStore;
    tempDbStoreType &m_tempDataStore;
    const std::size_t m_xid;
};

struct CompareData
{
    bool operator() (const BasicDBObject::pointer_t r, const BasicDBObject::pointer_t l) const
    {
        return *r.get() < *l.get();
    }
};

/**
 * defines select job operation.
 */
class SelectJob : public Job
{
public:
    using whereData_t = std::shared_ptr<IBasicDBWhereObject>;
    using selectData_t = std::set<BasicDBObject::pointer_t, CompareData>;
    SelectJob(const int sequence,
              tempDbStoreType &tempDataStore,
              const DBStore &dataStore,
              whereData_t whereData)
        : Job(JOB_TYPE::SELECT, sequence), m_tempDataStore(tempDataStore), m_dataStore(dataStore), m_whereData(whereData)
    {
    }
    /**
     * collects data that appropriates to whereData into m_selectRes structure
     */
    void run() override;

    /**
     * returs selected dataMap
     */
    const selectData_t& getResult() const { return m_selectRes; }

private:
    /**
     * selects data from dbStore and temp data store without condition
     */
    void selectAll();
 
    const DBStore &m_dataStore;  // db store
    tempDbStoreType &m_tempDataStore;  // temp data store for one transaction
    whereData_t m_whereData; // where clause data
    selectData_t m_selectRes; // selected result data
};

#endif