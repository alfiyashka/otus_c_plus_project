#include "../src/db/Job.hpp"

#include <gtest/gtest.h>
#include "Utils.hpp"


TEST(db_select_job_test_case, nothing_to_select_empty_db)
{
    tempDbStoreType tempDBStore;
    DBStore dbStore;
    
    SelectJob::whereData_t wheredata(
        new BasicDBSelectObject(
            Datatype::STRING, "data_name", Data_t(new std::string("data"))));
    SelectJob job(1, tempDBStore, dbStore, wheredata);

    job.run();

    EXPECT_EQ(job.getResult().size(), 0);
}



TEST(db_select_job_test_case, nothing_to_select)
{
    DBStore dbStore;
    insertDB2Rows(dbStore);
    tempDbStoreType tempDBStore;
    
    std::size_t selectJobId = 2;
    SelectJob::whereData_t wheredata(
        new BasicDBSelectObject(
            Datatype::STRING, "data_name", Data_t(new std::string("old_data"))));
    SelectJob job(selectJobId, tempDBStore, dbStore, wheredata);

    job.run();

    EXPECT_EQ(tempDBStore.size(), 0);
    EXPECT_EQ(dbStore.select().size(), 2);
    EXPECT_EQ(job.getResult().size(), 0);
}


TEST(db_select_job_test_case, select_one_by_type)
{
    tempDbStoreType tempDBStore;
    DBStore dbStore;
    insertDB2Rows(dbStore);
    EXPECT_EQ(dbStore.select().size(), 2);
  
    const std::size_t selectJobId = 1;
    SelectJob::whereData_t wheredata(
        new BasicDBSelectObject(
            Datatype::STRING, Data_t(nullptr)));
    SelectJob job(selectJobId, tempDBStore, dbStore, wheredata);

    job.run();
    auto selectRes = job.getResult();
    EXPECT_EQ(selectRes.size(), 1);

    auto findSelected = std::find_if(selectRes.begin(), selectRes.end(),
      [](const SelectJob::selectData_t::value_type& data)
     {
        return data->typeRedo() == Datatype::STRING;
     });

    EXPECT_TRUE(findSelected != selectRes.end());
}


TEST(db_select_job_test_case, select_one_by_data)
{
    tempDbStoreType tempDBStore;
    DBStore dbStore;
    insertDB2Rows(dbStore);
    EXPECT_EQ(dbStore.select().size(), 2);

    //1) test1 found 
    const std::size_t selectJobId = 1;
    SelectJob::whereData_t wheredata(
        new BasicDBSelectObject(
            Datatype::STRING, Data_t(new std::string("data"))));
    SelectJob job(selectJobId, tempDBStore, dbStore, wheredata);

    job.run();

    auto selectRes = job.getResult();
    EXPECT_EQ(selectRes.size(), 1);

    auto findSelected = std::find_if(selectRes.begin(), selectRes.end(),
      [&wheredata](const SelectJob::selectData_t::value_type& data)
     {
        return DataComparator::isPlainDataEqual(DataWithType(data->typeRedo(), data->dataRedo()),
          DataWithType(wheredata->typeWhere(), wheredata->dataWhere()));
     });

    EXPECT_TRUE(findSelected != selectRes.end());

    //1) test1 not found 
    tempDBStore.clear();

    SelectJob::whereData_t wheredata2(
        new BasicDBSelectObject(
            Datatype::STRING, Data_t(new std::string("another_data"))));
    SelectJob job2(selectJobId, tempDBStore, dbStore, wheredata2);

    job2.run();

    EXPECT_EQ(job2.getResult().size(), 0);
}


TEST(db_select_job_test_case, select_one_by_data_and_name)
{
    tempDbStoreType tempDBStore;
    DBStore dbStore;
    insertDB2Rows(dbStore);
    EXPECT_EQ(dbStore.select().size(), 2);

    tempDBStore.clear();

    const std::size_t selectJobId = 1;
    SelectJob::whereData_t wheredata(
        new BasicDBSelectObject(
            Datatype::STRING, "myString", Data_t(new std::string("data"))));
    SelectJob job(selectJobId, tempDBStore, dbStore, wheredata);

    job.run();
    auto selectRes = job.getResult();
    EXPECT_EQ(selectRes.size(), 1);

    auto findSelected = std::find_if(selectRes.begin(), selectRes.end(),
      [&wheredata](const SelectJob::selectData_t::value_type& data)
     {
        return DataComparator::isPlainDataEqual(DataWithType(data->typeRedo(), data->dataRedo()),
          DataWithType(wheredata->typeWhere(), wheredata->dataWhere()))
           && data->nameRedo() == wheredata->nameWhere();
     });

}

TEST(db_select_job_test_case, select_one_by_only_name)
{
    tempDbStoreType tempDBStore;
    DBStore dbStore;
    insertDB2Rows(dbStore);
    EXPECT_EQ(dbStore.select().size(), 2);

    tempDBStore.clear();

    const std::size_t selectJobId = 1;
    SelectJob::whereData_t wheredata(new BasicDBSelectObject("myString"));
    SelectJob job(selectJobId, tempDBStore, dbStore, wheredata);

    job.run();

    auto selectRes = job.getResult();
    EXPECT_EQ(selectRes.size(), 1);

    auto findSelected = std::find_if(selectRes.begin(), selectRes.end(),
      [&wheredata](const SelectJob::selectData_t::value_type& data)
     {
        return data->nameRedo() == wheredata->nameWhere();
     });

}


TEST(db_select_job_test_case, select_complex)
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

    const std::size_t selectJobId = 4;
    SelectJob::whereData_t wheredata(
        new BasicDBSelectObject(
            Datatype::STRING, Data_t(nullptr)));
    SelectJob job(selectJobId, tempDBStore, dbStore, wheredata);

    job.run();

    EXPECT_EQ(tempDBStore.size(), 3);
    auto selectRes = job.getResult();
    EXPECT_EQ(selectRes.size(), 2);


    for (auto selectItem : selectRes)
    {
        if (selectItem->id() == updatedDataToStore2->id())
        {
            // is updated ?
            EXPECT_TRUE(DataComparator::isPlainDataEqual(DataWithType(selectItem->typeRedo(), selectItem->dataRedo()),
                DataWithType(updatedDataToStore2->typeRedo(), updatedDataToStore2->dataRedo())));
            break;
        }
    }
}


TEST(db_select_job_test_case, select_one_by_type_composite_type)
{
    tempDbStoreType tempDBStore;
    DBStore dbStore;
    
    tempDbStoreType::value_type::second_type::second_type tempInsertRes;
    auto dataToStore = BasicDBObject::pointer_t(new BasicDBObject(Datatype::INT, "myInt", Data_t(new int(30))));
    auto compositeData = ComplexDBObject::pointer_t(new ComplexDBObject("myComplex", Data_t(new std::string("data"))));
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
    
    const std::size_t selectJobId = 1;
    SelectJob::whereData_t wheredata(
        new BasicDBSelectObject(
            Datatype::COMPOSITE, Data_t(nullptr)));
    SelectJob job(selectJobId, tempDBStore, dbStore, wheredata);

    job.run();

    auto selectRes = job.getResult();
    EXPECT_EQ(selectRes.size(), 1);

    auto findSelected = std::find_if(selectRes.begin(), selectRes.end(),
      [&wheredata](const SelectJob::selectData_t::value_type& data)
     {
        return data->typeRedo() == Datatype::COMPOSITE;
     });
}

