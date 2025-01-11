#include "../src/db/DBStore.hpp"
#include "../src/db/DataComparator.hpp"
#include <gtest/gtest.h>
#include <string>

TEST(db_store_test_case, is_empty)
{
    DBStore dbStore;
    EXPECT_TRUE(dbStore.empty());

    //insert nothing. temDbStore is empty
    tempDbStoreType tempDBStore;
    dbStore.insert(tempDBStore);
    EXPECT_TRUE(dbStore.empty());

    tempDbStoreType::value_type::second_type::second_type tempInsertRes;
    std::size_t id = 1;
    tempDBStore.insert(
            tempDbStoreType::value_type(id, std::pair(JOB_TYPE::INSERT, tempInsertRes)));

    dbStore.insert(tempDBStore);
    EXPECT_TRUE(dbStore.empty()); // temp data list is empty
}

TEST(db_store_test_case, is_not_empty)
{
    DBStore dbStore;
    EXPECT_TRUE(dbStore.empty());

    tempDbStoreType temDbStore;
    tempDbStoreType::value_type::second_type::second_type tempInsertRes;
    auto dataToStore = BasicDBObject::pointer_t(new BasicDBObject(Datatype::INT, "myInt", Data_t(new int(30))));
    tempInsertRes.push_back(dataToStore);
    std::size_t id = 1;
    temDbStore.insert(
            tempDbStoreType::value_type(id, std::pair(JOB_TYPE::INSERT, tempInsertRes)));

    dbStore.insert(temDbStore);
    EXPECT_FALSE(dbStore.empty());

    const auto selectRes = dbStore.select();
    EXPECT_EQ(selectRes.size(), 1);
}


TEST(db_store_test_case, complex_test)
{
    DBStore dbStore;
    EXPECT_TRUE(dbStore.empty());

    tempDbStoreType tempDbStore;
    tempDbStoreType::value_type::second_type::second_type tempDMLOperationRes;


    //1)insert data 1
    //2)insert data 2
    //3)insert data 3
    //4)update data 1, 2
    //5)delete data 2
    //6)insert data 4
    //7)delete data 4
    //8)select 1(updated), 3(same)

    //1)insert data 1
    auto dataToStore1 = BasicDBObject::pointer_t(
        new BasicDBObject(Datatype::INT, "myInt1", Data_t(new int(30))));

    tempDMLOperationRes.push_back(dataToStore1);
    std::size_t id = 1;
    tempDbStore.insert(
            tempDbStoreType::value_type(id,
            std::pair(JOB_TYPE::INSERT, tempDMLOperationRes)));
    tempDMLOperationRes.clear();

    //2)insert data 2
    auto dataToStore2 = BasicDBObject::pointer_t(
        new BasicDBObject(Datatype::INT, "myInt2", Data_t(new int(31))));

    tempDMLOperationRes.push_back(dataToStore2);
    id = 2;
    tempDbStore.insert(
            tempDbStoreType::value_type(id,
            std::pair(JOB_TYPE::INSERT, tempDMLOperationRes)));
    tempDMLOperationRes.clear();

    //3)insert data 3
    auto dataToStore3 = BasicDBObject::pointer_t(
        new BasicDBObject(Datatype::SMALLINT, "mySmallInt", Data_t(new int16_t(31))));
    id = 3;
    tempDMLOperationRes.push_back(dataToStore3);
    tempDbStore.insert(
            tempDbStoreType::value_type(id,
            std::pair(JOB_TYPE::INSERT, tempDMLOperationRes)));
    tempDMLOperationRes.clear();

    //4)update data 1, 2
    id = 4;
    auto updatedDataToStore1 = BasicDBObject::pointer_t(
        new BasicDBObject(*dataToStore1.get()));
    updatedDataToStore1->updateData(Data_t(new int(200)));
    updatedDataToStore1->updateName("updated1");
    auto updatedDataToStore2 = BasicDBObject::pointer_t(
        new BasicDBObject(*dataToStore2.get()));   
    updatedDataToStore2->updateData(Data_t(new int(200)));
    updatedDataToStore2->updateName("updated2");
    tempDMLOperationRes.push_back(updatedDataToStore1);
    tempDMLOperationRes.push_back(updatedDataToStore2);
    tempDbStore.insert(
            tempDbStoreType::value_type(id,
            std::pair(JOB_TYPE::UPDATE, tempDMLOperationRes)));
    tempDMLOperationRes.clear();

    //5)delete data 2
    id = 5;
    auto deleteDataToStore2 = BasicDBObject::pointer_t(
        new BasicDBObject(*dataToStore2.get()));
    tempDMLOperationRes.push_back(deleteDataToStore2);
    tempDbStore.insert(
            tempDbStoreType::value_type(id,
            std::pair(JOB_TYPE::DELETE, tempDMLOperationRes)));
    tempDMLOperationRes.clear();

    //6)insert data 4
    auto dataToStore4 = BasicDBObject::pointer_t(
        new BasicDBObject(Datatype::INT, "myInt4", Data_t(new int(31))));
    tempDMLOperationRes.push_back(dataToStore4);
    id = 6;
    tempDbStore.insert(
            tempDbStoreType::value_type(id,
            std::pair(JOB_TYPE::INSERT, tempDMLOperationRes)));
    tempDMLOperationRes.clear();

    //7)delete data 4
    id = 7;
    auto deleteDataToStore4 = BasicDBObject::pointer_t(
        new BasicDBObject(*dataToStore4.get()));
    tempDMLOperationRes.push_back(deleteDataToStore4);
    tempDbStore.insert(
            tempDbStoreType::value_type(id,
            std::pair(JOB_TYPE::DELETE, tempDMLOperationRes)));
    tempDMLOperationRes.clear();

    EXPECT_TRUE(dbStore.insert(tempDbStore));
    EXPECT_FALSE(dbStore.empty());

    //8)select 1(updated), 3(same)
    const auto selectRes = dbStore.select();

    EXPECT_EQ(selectRes.size(), 2);

    for (auto selectItem : selectRes)
    {
        if (selectItem.first == dataToStore1->id())
        {
            // is updated ?
            auto data = selectItem.second;
            EXPECT_TRUE(DataComparator::isPlainDataEqual(DataWithType(data->typeRedo(), data->dataRedo()),
                DataWithType(updatedDataToStore1->typeRedo(), updatedDataToStore1->dataRedo())));
            continue;
        }
        if (selectItem.first == dataToStore4->id())
        {
            auto data = selectItem.second;
            EXPECT_TRUE(DataComparator::isPlainDataEqual(DataWithType(data->typeRedo(), data->dataRedo()),
                DataWithType(dataToStore4->typeRedo(), dataToStore4->dataRedo())));            
        }
    }
}

TEST(db_store_test_case, test_try_add_with_same_id)
{
    DBStore dbStore;
    EXPECT_TRUE(dbStore.empty());

    tempDbStoreType tempDbStore;
    tempDbStoreType::value_type::second_type::second_type tempDMLOperationRes;

    auto dataToStore1 = BasicDBObject::pointer_t(
        new BasicDBObject(Datatype::INT, "myInt1", Data_t(new int(30))));

    tempDMLOperationRes.push_back(dataToStore1);
    std::size_t id = 1;
    tempDbStore.insert(
            tempDbStoreType::value_type(id,
            std::pair(JOB_TYPE::INSERT, tempDMLOperationRes)));
    tempDMLOperationRes.clear();

    id = 2;
    auto dublicateDataToStore1 = BasicDBObject::pointer_t(
        new BasicDBObject(*dataToStore1.get()));

    tempDMLOperationRes.push_back(dublicateDataToStore1);

    tempDbStore.insert(
            tempDbStoreType::value_type(id,
            std::pair(JOB_TYPE::INSERT, tempDMLOperationRes)));

    EXPECT_FALSE(dbStore.insert(tempDbStore));

}


TEST(db_store_test_case, test_try_update_non_exist_data)
{
    DBStore dbStore;
    EXPECT_TRUE(dbStore.empty());

    tempDbStoreType tempDbStore;
    tempDbStoreType::value_type::second_type::second_type tempDMLOperationRes;

    auto dataToStore1 = BasicDBObject::pointer_t(
        new BasicDBObject(Datatype::INT, "myInt1", Data_t(new int(30))));

    tempDMLOperationRes.push_back(dataToStore1);
    std::size_t id = 1;
    tempDbStore.insert(
            tempDbStoreType::value_type(id,
            std::pair(JOB_TYPE::UPDATE, tempDMLOperationRes)));

    EXPECT_TRUE(dbStore.insert(tempDbStore));
    EXPECT_TRUE(dbStore.empty());
}


TEST(db_store_test_case, test_threads_try_add_same)
{
    DBStore dbStore;
    EXPECT_TRUE(dbStore.empty());

    auto dataToStore1 = BasicDBObject::pointer_t(
        new BasicDBObject(Datatype::INT, "myInt1", Data_t(new int(30))));

    auto dublicateDataToStore1 = BasicDBObject::pointer_t(
        new BasicDBObject(*dataToStore1.get()));

    auto threadWork = [&dbStore] (BasicDBObject::pointer_t data) {
        tempDbStoreType tempDbStore;
        tempDbStoreType::value_type::second_type::second_type tempDMLOperationRes;

        tempDMLOperationRes.push_back(data);
        std::size_t id = 1;
        tempDbStore.insert(
                tempDbStoreType::value_type(id,
                std::pair(JOB_TYPE::INSERT, tempDMLOperationRes)));
        tempDMLOperationRes.clear();

        dbStore.insert(tempDbStore);
    };

    auto th1 = std::thread(threadWork, dataToStore1);
    auto th2 = std::thread(threadWork, dublicateDataToStore1);\

    if (th1.joinable())
    {
        th1.join();
    }

    if (th2.joinable())
    {
        th2.join();
    }

    EXPECT_EQ(dbStore.select().size(), 1);
}
