#include "../src/db/Job.hpp"

#include <gtest/gtest.h>
#include "Utils.hpp"

TEST(db_delete_job_test_case, nothing_to_delete_empty_db)
{
    tempDbStoreType tempDBStore;
    DBStore dbStore;
    
    DeleteJob::whereData_t wheredata(
        new BasicDBDeleteObject(
            Datatype::STRING, "data_name", Data_t(new std::string("data"))));
    DeleteJob job(1, tempDBStore, dbStore, wheredata);

    EXPECT_EQ(tempDBStore.size(), 0);

    job.run();

    EXPECT_EQ(tempDBStore.size(), 0);
}

TEST(db_delete_job_test_case, nothing_to_delete)
{
    DBStore dbStore;
    insertDB2Rows(dbStore);
    tempDbStoreType tempDBStore;
    
    std::size_t deleteJobId = 2;
    DeleteJob::whereData_t wheredata(
        new BasicDBDeleteObject(
            Datatype::STRING, "data_name", Data_t(new std::string("old_data"))));
    DeleteJob job(deleteJobId, tempDBStore, dbStore, wheredata);

    EXPECT_EQ(tempDBStore.size(), 0);

    job.run();

    EXPECT_EQ(tempDBStore.size(), 0);
    EXPECT_EQ(dbStore.select().size(), 2);
    EXPECT_TRUE(tempDBStore.find(deleteJobId) == tempDBStore.end());
}

TEST(db_delete_job_test_case, delete_one_by_type)
{
    tempDbStoreType tempDBStore;
    DBStore dbStore;
    insertDB2Rows(dbStore);
    EXPECT_EQ(dbStore.select().size(), 2);
  
    const std::size_t deleteJobId = 1;
    DeleteJob::whereData_t wheredata(
        new BasicDBDeleteObject(
            Datatype::STRING, Data_t(nullptr)));
    DeleteJob job(deleteJobId, tempDBStore, dbStore, wheredata);

    EXPECT_EQ(tempDBStore.size(), 0);

    job.run();

    EXPECT_EQ(tempDBStore.size(), 1);
    EXPECT_TRUE(tempDBStore.find(deleteJobId) != tempDBStore.end());
}

TEST(db_delete_job_test_case, delete_one_by_data)
{
    tempDbStoreType tempDBStore;
    DBStore dbStore;
    insertDB2Rows(dbStore);
    EXPECT_EQ(dbStore.select().size(), 2);

    tempDBStore.clear();
    
    //1) test1 found 
    const std::size_t deleteJobId = 1;
    DeleteJob::whereData_t wheredata(
        new BasicDBDeleteObject(
            Datatype::STRING, Data_t(new std::string("data"))));
    DeleteJob job(deleteJobId, tempDBStore, dbStore, wheredata);

    EXPECT_EQ(tempDBStore.size(), 0);

    job.run();

    EXPECT_EQ(tempDBStore.size(), 1);
    EXPECT_TRUE(tempDBStore.find(deleteJobId) != tempDBStore.end());

    //1) test1 not found 
    tempDBStore.clear();

    DeleteJob::whereData_t wheredata2(
        new BasicDBDeleteObject(
            Datatype::STRING, Data_t(new std::string("another_data"))));
    DeleteJob job2(deleteJobId, tempDBStore, dbStore, wheredata2);

    EXPECT_EQ(tempDBStore.size(), 0);

    job2.run();

    EXPECT_EQ(tempDBStore.size(), 0);
}

TEST(db_delete_job_test_case, delete_one_by_data_name)
{
    tempDbStoreType tempDBStore;
    DBStore dbStore;
    insertDB2Rows(dbStore);
    EXPECT_EQ(dbStore.select().size(), 2);

    tempDBStore.clear();

    const std::size_t deleteJobId = 1;
    DeleteJob::whereData_t wheredata(
        new BasicDBDeleteObject(
            Datatype::STRING, "myString", Data_t(new std::string("data"))));
    DeleteJob job(deleteJobId, tempDBStore, dbStore, wheredata);

    EXPECT_EQ(tempDBStore.size(), 0);

    job.run();

    EXPECT_EQ(tempDBStore.size(), 1);
    EXPECT_TRUE(tempDBStore.find(deleteJobId) != tempDBStore.end());
}

TEST(db_delete_job_test_case, delete_one_by_only_name)
{
    tempDbStoreType tempDBStore;
    DBStore dbStore;
    insertDB2Rows(dbStore);
    EXPECT_EQ(dbStore.select().size(), 2);

    tempDBStore.clear();

    const std::size_t deleteJobId = 1;
    DeleteJob::whereData_t wheredata(new BasicDBDeleteObject("myString"));
    DeleteJob job(deleteJobId, tempDBStore, dbStore, wheredata);

    EXPECT_EQ(tempDBStore.size(), 0);

    job.run();

    EXPECT_EQ(tempDBStore.size(), 1);
    EXPECT_TRUE(tempDBStore.find(deleteJobId) != tempDBStore.end());
}


TEST(db_delete_job_test_case, delete_complex)
{
    // we are in the long transaction, temp data list is not empty
    tempDbStoreType tempDBStore;

    auto dataToStore = BasicDBObject::pointer_t(
        new BasicDBObject(Datatype::STRING, "myStr", Data_t(new std::string("31"))));
    auto dataToStore2 = BasicDBObject::pointer_t(
        new BasicDBObject(Datatype::STRING, "myStr2", Data_t(new std::string("data"))));
    
    tempDbStoreType::value_type::second_type::second_type tempDMLOperationRes;
    tempDMLOperationRes.push_back(dataToStore);
    tempDMLOperationRes.push_back(dataToStore2);
    std::size_t id = 1;
    tempDBStore.insert(
            tempDbStoreType::value_type(id,
            std::pair(JOB_TYPE::INSERT, tempDMLOperationRes)));
    tempDMLOperationRes.clear();


    id = 2;
    auto deleteDataToStore = BasicDBObject::pointer_t(
        new BasicDBObject(*dataToStore.get()));
    tempDMLOperationRes.push_back(deleteDataToStore);
    tempDBStore.insert(
            tempDbStoreType::value_type(id,
            std::pair(JOB_TYPE::DELETE, tempDMLOperationRes)));
    tempDMLOperationRes.clear();

    id = 3;
    auto updatedDataToStore2 = BasicDBObject::pointer_t(
        new BasicDBObject(*dataToStore2.get()));
    updatedDataToStore2->updateData(Data_t(new std::string("updated")));
    updatedDataToStore2->updateName("updatedName");
    tempDMLOperationRes.push_back(updatedDataToStore2);
    tempDBStore.insert(
            tempDbStoreType::value_type(id,
            std::pair(JOB_TYPE::UPDATE, tempDMLOperationRes)));
    tempDMLOperationRes.clear();


    DBStore dbStore;
    insertDB2Rows(dbStore);
    EXPECT_EQ(dbStore.select().size(), 2);

    
    const std::size_t deleteJobId = 4;
    DeleteJob::whereData_t wheredata(
        new BasicDBDeleteObject(
            Datatype::STRING, Data_t(nullptr)));
    DeleteJob job(deleteJobId, tempDBStore, dbStore, wheredata);

    job.run();

    EXPECT_EQ(tempDBStore.size(), 4);
    auto findDeleteResult = tempDBStore.find(deleteJobId);
    EXPECT_TRUE(findDeleteResult != tempDBStore.end());
    EXPECT_EQ(findDeleteResult->second.first, JOB_TYPE::DELETE);
    EXPECT_EQ(findDeleteResult->second.second.size(), 2);

    for (auto deleteItem : findDeleteResult->second.second)
    {
        if (deleteItem->id() == updatedDataToStore2->id())
        {
            // is updated ?
            EXPECT_TRUE(DataComparator::isPlainDataEqual(DataWithType(deleteItem->typeRedo(), deleteItem->dataRedo()),
                DataWithType(updatedDataToStore2->typeRedo(), updatedDataToStore2->dataRedo())));
            break;
        }
    }
}

TEST(db_delete_job_test_case, delete_one_by_type_composite_type)
{
    tempDbStoreType tempDBStore;
    DBStore dbStore;
    
    tempDbStoreType::value_type::second_type::second_type tempInsertRes;
    auto dataToStore = BasicDBObject::pointer_t(new BasicDBObject(Datatype::INT, "myInt", Data_t(new int(30))));
    auto compositeData = ComplexDBObject::pointer_t(new ComplexDBObject("myComplex"));
    compositeData->addChild(dataToStore);
    
    BasicDBObject::pointer_t compositeDbObject = 
        std::static_pointer_cast<BasicDBObject>(compositeData);
    
    tempInsertRes.push_back(compositeDbObject);
    tempInsertRes.push_back(dataToStore);
    std::size_t id = 1;
    tempDBStore.insert(
            tempDbStoreType::value_type(id, std::pair(JOB_TYPE::INSERT, tempInsertRes)));

    dbStore.insert(tempDBStore);

    
    EXPECT_EQ(dbStore.select().size(), 2);

    tempDBStore.clear();
    
    const std::size_t deleteJobId = 1;
    DeleteJob::whereData_t wheredata(
        new BasicDBDeleteObject(
            Datatype::COMPOSITE, Data_t(nullptr)));
    DeleteJob job(deleteJobId, tempDBStore, dbStore, wheredata);

    EXPECT_EQ(tempDBStore.size(), 0);

    job.run();

    EXPECT_EQ(tempDBStore.size(), 1);
    EXPECT_TRUE(tempDBStore.find(deleteJobId) != tempDBStore.end());
}

