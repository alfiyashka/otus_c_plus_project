#include "Transaction.hpp"

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
            rollback();
            m_isOpen = false;
            while (m_jobs.empty())
            {
                m_jobs.pop();
            }
            break;
        }

        const auto job = m_jobs.front();
        job->run();
        if (job->type() == JOB_TYPE::COMMIT)
        {
            m_isOpen = false;
        }
        m_lastProcessedSequence = job->sequence();
        m_jobs.pop();
    }
}

void Transaction::begin()
{
    std::cout << "Begin transaction with id '" << m_xid << "'\n";
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
    std::cout << "Inserting data by xid '" << m_xid << "'\n";
    if (!insertData.get())
    {
        m_failed = false;
        m_condVar.notify_one();
        std::cout << "Insert failed \n";
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
    std::cout << "Updating data by xid '" << m_xid << "'\n";
    if (!updateData.get())
    {
        m_failed = false;
        m_condVar.notify_one();
        std::cout << "Updating failed \n";
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
    std::cout << "Deleting data by xid '" << m_xid << "'\n";
    if (!deleteData.get())
    {
        m_failed = false;
        m_condVar.notify_one();
        std::cout << "Delete failed \n";
        return false;
    }

    {
        std::shared_ptr<Job> deleteJob(new DeleteJob(m_jobSequence++, m_tempDataStore, m_dataStore, deleteData));
        m_jobs.push(deleteJob);
        m_condVar.notify_one();
        return true;
    }
}

BasicDBObject::dataList_t Transaction::select(SelectJob::whereData_t whereData)
{
    while (m_isOpen && !m_failed)
    {
        if (m_lastProcessedSequence >= m_jobSequence - 1 && m_jobs.empty())
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            break;
        }
    }
    std::cout << "Selecting data by xid '" << m_xid << "'\n";
    std::shared_ptr<SelectJob> selectJob(new SelectJob(m_jobSequence++, m_tempDataStore, m_dataStore, whereData));
    selectJob->run();
    m_lastProcessedSequence = selectJob->sequence();
    const auto resFromJob = selectJob->getResult();
    BasicDBObject::dataList_t res { resFromJob.begin(), resFromJob.end() };

    return res;
}

void Transaction::rollback()
{
    m_tempDataStore.clear();
    std::cout << "Rollback data by xid '" << m_xid << "'\n";
    if (m_thread.joinable())
    {
        m_thread.join();
    }      
}

void Transaction::terminate()
{
    this->m_failed = true;
    m_condVar.notify_one();
    std::cout << "Terminate data by xid '" << m_xid << "'\n";
}
