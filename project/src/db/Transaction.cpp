#include "Transaction.hpp"
#include "yelloger.h"

Transaction::~Transaction()
{
    if (m_thread.joinable())
        m_thread.join();
}

void Transaction::transactionWork()
{
    while (m_isOpen || m_jobs.size() > 0)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condVar.wait(lock, [this]
                       { return m_failed || !m_isOpen || !m_jobs.empty(); });

        if (m_failed)
        { // failed in the middle of transaction
            m_isOpen = false;
            break;
        }

        const auto job = m_jobs.front();
        job->run();
        if (job->type() == JOB_TYPE::COMMIT)
        {
            m_isOpen = false;
            break;
        }
        m_lastProcessedSequence = job->sequence();
        m_jobs.pop();
    }
}

void Transaction::begin()
{
    m_isOpen = true;
    Yellog::Info("Begin transaction with id '%d'", m_xid);
}

void Transaction::commit()
{
    std::shared_ptr<Job> commitJob(new CommitJob(m_jobSequence++, m_tempDataStore, m_dataStore, m_xid));
    m_jobs.push(commitJob);
    m_condVar.notify_one();
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

bool Transaction::insert(InsertJob::insertData_t insertData)
{
    Yellog::Info("Inserting data by xid '%d'", m_xid);
    if (!insertData.get())
    {
        m_failed = false;
        m_condVar.notify_one();
        Yellog::Error("Insert failed");
        return false;
    }

    {
        std::shared_ptr<Job> insertJob(new InsertJob(m_jobSequence++, m_tempDataStore, insertData));
        m_jobs.push(insertJob);
        m_condVar.notify_one();
    }

    return true;
}

bool Transaction::update(UpdateJob::updateData_t updateData)
{
    Yellog::Info("Updating data by xid '%d'", m_xid);
    if (!updateData.get())
    {
        m_failed = false;
        m_condVar.notify_one();
        Yellog::Error("Updating failed");
        return false;
    }

    {
        std::shared_ptr<Job> updateJob(new UpdateJob(m_jobSequence++, m_tempDataStore, m_dataStore, updateData));
        m_jobs.push(updateJob);
        m_condVar.notify_one();
    }

    return true;
}

bool Transaction::deleteExec(DeleteJob::whereData_t deleteData)
{
    Yellog::Info("Deleting data by xid '%d'", m_xid);
    if (!deleteData.get())
    {
        m_failed = false;
        m_condVar.notify_one();
        Yellog::Error("Delete failed");
        return false;
    }

    {
        std::shared_ptr<Job> deleteJob(new DeleteJob(m_jobSequence++, m_tempDataStore, m_dataStore, deleteData));
        m_jobs.push(deleteJob);
        m_condVar.notify_one();
        return true;
    }
}

BasicDBObject::selectList_t Transaction::select(SelectJob::whereData_t whereData)
{
    while (m_isOpen && !m_failed)
    {
        if (m_lastProcessedSequence >= m_jobSequence - 1 && m_jobs.empty())
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            break;
        }
    }
    Yellog::Info("Selecting data by xid '%d'", m_xid);
    std::shared_ptr<SelectJob> selectJob(new SelectJob(m_jobSequence++, m_tempDataStore, m_dataStore, whereData));
    selectJob->run();
    m_lastProcessedSequence = selectJob->sequence();
    const auto resFromJob = selectJob->getResult();
    BasicDBObject::selectList_t res { resFromJob.begin(), resFromJob.end() };

    return res;
}

void Transaction::rollback()
{
    m_failed = true;
    m_condVar.notify_one();
    Yellog::Info("Rollback data by xid '%d'", m_xid);
    if (m_thread.joinable())
    {
        m_thread.join();
    }      
}

void Transaction::terminate()
{
    Yellog::Info("Terminating data by xid '%d'", m_xid);
    rollback();
}
