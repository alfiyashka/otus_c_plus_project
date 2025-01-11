#include "../src/db/Job.hpp"

#include <gtest/gtest.h>
#include "Utils.hpp"

TEST(db_commit_job_test_case, test1)
{
    tempDbStoreType tempDBStore;
    DBStore dbStore;
    insertDB2Rows(dbStore);
    EXPECT_EQ(dbStore.select().size(), 2);

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

    EXPECT_EQ(tempDBStore.size(), 3);

    
    CommitJob job(1, tempDBStore, dbStore, 1);

    job.run();

    EXPECT_EQ(dbStore.select().size(), 3);

    auto findUpdated = dbStore.select().find(updatedDataToStore2->id());

    ASSERT_TRUE(findUpdated != dbStore.select().end());

    // is updated?
    EXPECT_TRUE(DataComparator::isPlainDataEqual(
        DataWithType(findUpdated->second->typeRedo(), findUpdated->second->dataRedo()),
        DataWithType(updatedDataToStore2->typeRedo(), updatedDataToStore2->dataRedo())));
}