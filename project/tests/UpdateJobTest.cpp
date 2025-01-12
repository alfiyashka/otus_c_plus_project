#include "../src/db/Job.hpp"

#include <gtest/gtest.h>
#include "Utils.hpp"


TEST(db_update_job_test_case, nothing_to_update_empty_db)
{
    tempDbStoreType tempDBStore;
    DBStore dbStore;
    
    UpdateJob::updateData_t updateData(
        new BasicDBUpdateObject(
            Datatype::STRING, "data_name", Data_t(new std::string("data")),
            Datatype::STRING, "old_data_name", Data_t(new std::string("old_data"))));
    UpdateJob job(1, tempDBStore, dbStore, updateData);

    job.run();

    EXPECT_EQ(tempDBStore.size(), 0);
}



TEST(db_update_job_test_case, nothing_to_update)
{
    DBStore dbStore;
    insertDB2Rows(dbStore);
    tempDbStoreType tempDBStore;
    
    std::size_t updateJobId = 2;
    UpdateJob::updateData_t updateData(
        new BasicDBUpdateObject(
            Datatype::STRING, "data_name", Data_t(new std::string("data")),
            Datatype::STRING, "old_data_name", Data_t(new std::string("old_data"))));
    UpdateJob job(updateJobId, tempDBStore, dbStore, updateData);

    job.run();

    EXPECT_EQ(tempDBStore.size(), 0);
    EXPECT_EQ(dbStore.select().size(), 2);
}

void checkResultUpdate(const tempDbStoreType& tempDBStore, 
    const std::size_t updateJobId,
    UpdateJob::updateData_t updatedata,
    const int expectedUpdateDataCount)
{
    auto findUpdated = tempDBStore.find(updateJobId);
    ASSERT_TRUE(findUpdated != tempDBStore.end());

    if (findUpdated != tempDBStore.end())
    {
        auto updatedDataList = findUpdated->second.second;
        auto findUpdatedDataCount = std::count_if(updatedDataList.begin(), updatedDataList.end(),
                [&updatedata](const tempDbStoreType::value_type::second_type::second_type::value_type &data)
                {
                    return (DataComparator::isPlainDataEqual(DataWithType(data->typeRedo(), data->dataRedo()),
                    DataWithType(updatedata->typeRedo(), updatedata->dataRedo()))) 
                    && updatedata->nameRedo() == data->nameRedo();
               });
        ASSERT_EQ(findUpdatedDataCount, expectedUpdateDataCount);
    }
}


TEST(db_update_job_test_case, update_one_by_type)
{
    tempDbStoreType tempDBStore;
    DBStore dbStore;
    insertDB2Rows(dbStore);
    EXPECT_EQ(dbStore.select().size(), 2);
  
    const std::size_t updateJobId = 1;
    UpdateJob::updateData_t updatedata(
        new BasicDBUpdateObject(
            Datatype::STRING, "updated", Data_t(new std::string("updated")),
            Datatype::STRING, "", Data_t(nullptr)));
    UpdateJob job(updateJobId, tempDBStore, dbStore, updatedata);

    job.run();
    EXPECT_EQ(tempDBStore.size(), 1); 

    checkResultUpdate(tempDBStore, updateJobId, updatedata, 1);   
}




TEST(db_update_job_test_case, update_one_by_data)
{
    tempDbStoreType tempDBStore;
    DBStore dbStore;
    insertDB2Rows(dbStore);
    EXPECT_EQ(dbStore.select().size(), 2);

    //1) test1 found 
    const std::size_t updateJobId = 1;
    UpdateJob::updateData_t updatedata(
        new BasicDBUpdateObject(
            Datatype::STRING, "updated", Data_t(new std::string("updated")),
            Datatype::STRING, "", Data_t(new std::string("data"))));
    UpdateJob job(updateJobId, tempDBStore, dbStore, updatedata);

    job.run();

    EXPECT_EQ(tempDBStore.size(), 1);

    checkResultUpdate(tempDBStore, updateJobId, updatedata, 1);


    //1) test1 not found 
    tempDBStore.clear();

    UpdateJob::updateData_t updatedata2(
        new BasicDBUpdateObject(
            Datatype::STRING, "updated2", Data_t(new std::string("updated2")),
            Datatype::STRING, "", Data_t(new std::string("another_data"))));
    UpdateJob job2(updateJobId, tempDBStore, dbStore, updatedata2);

    job2.run();

    EXPECT_EQ(tempDBStore.size(), 0);
 
}


TEST(db_update_job_test_case, update_one_by_data_and_name)
{
    tempDbStoreType tempDBStore;
    DBStore dbStore;
    insertDB2Rows(dbStore);
    EXPECT_EQ(dbStore.select().size(), 2);

    tempDBStore.clear();

    const std::size_t updateJobId = 1;
    UpdateJob::updateData_t updatedata(
        new BasicDBUpdateObject(
            Datatype::STRING, "updated", Data_t(new std::string("updated")),
            Datatype::STRING, "myString", Data_t(new std::string("data"))));
    UpdateJob job(updateJobId, tempDBStore, dbStore, updatedata);

    job.run();
    EXPECT_EQ(tempDBStore.size(), 1);

    checkResultUpdate(tempDBStore, updateJobId, updatedata, 1);
}

TEST(db_update_job_test_case, delete_one_by_only_name)
{
    tempDbStoreType tempDBStore;
    DBStore dbStore;
    insertDB2Rows(dbStore);
    EXPECT_EQ(dbStore.select().size(), 2);

    tempDBStore.clear();

    const std::size_t updateJobId = 1;
    UpdateJob::updateData_t updatedata(
        new BasicDBUpdateObject(
            Datatype::STRING, "updated", Data_t(new std::string("updated")),
            Datatype::UNDEFINED, "myString", Data_t(nullptr))
        );
    UpdateJob job(updateJobId, tempDBStore, dbStore, updatedata);

    job.run();

    EXPECT_EQ(tempDBStore.size(), 1);

    checkResultUpdate(tempDBStore, updateJobId, updatedata, 1);
}


TEST(db_update_job_test_case, delete_complex)
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

    EXPECT_EQ(tempDBStore.size(), 3);

    const std::size_t updateJobId = 4;
    UpdateJob::updateData_t updatedata(
        new BasicDBUpdateObject(
            Datatype::STRING, "updated", Data_t(new std::string("updated")),
            Datatype::STRING, "", Data_t(nullptr)));
    UpdateJob job(updateJobId, tempDBStore, dbStore, updatedata);

    job.run();

    EXPECT_EQ(tempDBStore.size(), 4);

    checkResultUpdate(tempDBStore, updateJobId, updatedata, 2);
}


TEST(db_update_job_test_case, update_one_by_type_composite_type)
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
    
    const std::size_t updateJobId = 1;
    
    auto dataToStoreRedo = BasicDBObject::pointer_t(new BasicDBObject(Datatype::INT, "myInt", Data_t(new int(30))));
    auto compositeDataRedo = ComplexDBObject::pointer_t(new ComplexDBObject("myComplexUpdate"));
    compositeDataRedo->addChild(dataToStoreRedo);

    UpdateJob::updateData_t updatedata(
        new BasicDBUpdateObject(
            Datatype::COMPOSITE, "", Data_t(std::move(compositeDataRedo)),
            Datatype::COMPOSITE, "", Data_t(nullptr)));    
    
    UpdateJob job(updateJobId, tempDBStore, dbStore, updatedata);

    job.run();

    EXPECT_EQ(tempDBStore.size(), 1);

}

