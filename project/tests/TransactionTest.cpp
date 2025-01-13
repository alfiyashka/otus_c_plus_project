#include <gtest/gtest.h>
#include <../src/db/Transaction.hpp>
#include "Utils.hpp"


class TransactionTest : public testing::Test
{
public:
  DBStore m_dbStore;

  void SetUp()
  {
    insertDB2Rows(m_dbStore);
  }
  void TearDown() { }

};

TEST_F(TransactionTest, test_empty)
{
    Transaction::pointer_t tr(new Transaction(m_dbStore));
    tr->begin();
    EXPECT_TRUE(tr->isOpen());
    tr->commit();
    EXPECT_FALSE(tr->isOpen());
}

TEST_F(TransactionTest, test_empty_select_from_db)
{
    Transaction::pointer_t tr(new Transaction(m_dbStore));
    tr->begin();
    EXPECT_TRUE(tr->isOpen());
    SelectJob::whereData_t wheredata(
        new BasicDBSelectObject(
            Datatype::STRING, "myString", Data_t(new std::string("data"))));
    const auto selectRes = tr->select(wheredata);
    EXPECT_EQ(selectRes.size(), 1);
    EXPECT_TRUE(tr->isOpen());
    tr->commit();
    EXPECT_FALSE(tr->isOpen());
}


TEST_F(TransactionTest, test_complex)
{
    Transaction::pointer_t tr(new Transaction(m_dbStore));
    tr->begin();
    //1 insert
    EXPECT_TRUE(tr->isOpen());
    InsertJob::insertData_t insertdata(
        new BasicDBInsertObject(Datatype::STRING, "data_name",
            Data_t(new std::string("data"))));
    tr->insert(insertdata);

    //2 select
    EXPECT_TRUE(tr->isOpen());
    SelectJob::whereData_t wheredata(
        new BasicDBSelectObject(
            Datatype::STRING, "myString", Data_t(new std::string("data"))));
    const auto selectRes = tr->select(wheredata);
    EXPECT_EQ(selectRes.size(), 1);

    //3 update first insert
    EXPECT_TRUE(tr->isOpen());
    UpdateJob::updateData_t updatedata(
        new BasicDBUpdateObject(
            Datatype::STRING, "updated", Data_t(new std::string("updated")),
            Datatype::STRING, "data_name", Data_t(new std::string("data"))));
    tr->update(updatedata);

     //4 select last updated (3 step)
    SelectJob::whereData_t wheredataUpdated(
        new BasicDBSelectObject(
            Datatype::STRING, "updated", Data_t(nullptr)));
    const auto selectResUpdated = tr->select(wheredataUpdated);
    EXPECT_EQ(selectResUpdated.size(), 1);

    //5 insert 2
    EXPECT_TRUE(tr->isOpen());
    InsertJob::insertData_t insertdata2(
        new BasicDBInsertObject(Datatype::STRING, "data_name",
            Data_t(new std::string("data"))));
    tr->insert(insertdata2);

    //5 delete last inserted 2
    DeleteJob::whereData_t deletedata(
        new BasicDBDeleteObject(
            Datatype::STRING, "data_name", Data_t(new std::string("data"))));
    tr->deleteExec(deletedata);

    //6 select last deleted 2
    SelectJob::whereData_t selectdataDeleted(
        new BasicDBSelectObject(
            Datatype::STRING, "data_name", Data_t(nullptr)));
    const auto selectResDeleted = tr->select(selectdataDeleted);
    EXPECT_EQ(selectResDeleted.size(), 0);

    //6 select last all string
    SelectJob::whereData_t wheredataAllString(
        new BasicDBSelectObject(
            Datatype::STRING, "", Data_t(nullptr)));
    const auto selectResAllString = tr->select(wheredataAllString);
    EXPECT_EQ(selectResAllString.size(), 2);

    EXPECT_TRUE(tr->isOpen());
    tr->commit();
    EXPECT_FALSE(tr->isOpen());

    EXPECT_EQ(m_dbStore.select().size(), 3); // 1 inserted 

}

TEST_F(TransactionTest, test_insert_dublicate_non_error_case)
{
    Transaction::pointer_t tr(new Transaction(m_dbStore));
    tr->begin();
    //1 insert
    EXPECT_TRUE(tr->isOpen());
    InsertJob::insertData_t insertdata(
        new BasicDBInsertObject(Datatype::STRING, "data_name",
            Data_t(new std::string("data"))));
    tr->insert(insertdata);

    //insert same, added new id 
    tr->insert(insertdata);

    EXPECT_TRUE(tr->isOpen());
    tr->commit();
    EXPECT_FALSE(tr->isOpen());

    EXPECT_EQ(m_dbStore.select().size(), 4);

}

TEST_F(TransactionTest, test_terminate)
{
    Transaction::pointer_t tr(new Transaction(m_dbStore));
    tr->begin();
    //1 insert
    EXPECT_TRUE(tr->isOpen());
    InsertJob::insertData_t insertdata(
        new BasicDBInsertObject(Datatype::STRING, "data_name",
            Data_t(new std::string("data"))));
    tr->insert(insertdata);

    tr->terminate();
    EXPECT_FALSE(tr->isOpen());

    EXPECT_EQ(m_dbStore.select().size(), 2);

}

TEST_F(TransactionTest, test_rollback)
{
    Transaction::pointer_t tr(new Transaction(m_dbStore));
    tr->begin();
    //1 insert
    EXPECT_TRUE(tr->isOpen());
    InsertJob::insertData_t insertdata(
        new BasicDBInsertObject(Datatype::STRING, "data_name",
            Data_t(new std::string("data"))));
    tr->insert(insertdata);

    tr->rollback();
    EXPECT_FALSE(tr->isOpen());

    EXPECT_EQ(m_dbStore.select().size(), 2);

}


