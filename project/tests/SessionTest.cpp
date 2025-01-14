#include <gtest/gtest.h>
#include "../src/db/Session.hpp"
#include "Utils.hpp"

#include <memory>



class SessionTest : public testing::Test
{
public:
  DBStore m_dbStore;
  TransactionManager m_transactionManager;
  IAuth::pointer_t m_auth;

  void SetUp()
  {
    m_auth = IAuth::pointer_t(new Auth("user", "pwd"));
    insertDB2Rows(m_dbStore);
  }
  void TearDown() { }

};

TEST_F(SessionTest, test_empty)
{
    Session session(1, m_auth, m_transactionManager, m_dbStore);
    session.createTransaction();
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 1);
    session.commit();
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 0);
}

TEST_F(SessionTest, test_empty_rollback)
{
    Session session(1, m_auth, m_transactionManager, m_dbStore);
    session.createTransaction();
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 1);
    session.rollback();
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 0);
}


TEST_F(SessionTest, test_empty_select_from_db)
{
    Session session(1, m_auth, m_transactionManager, m_dbStore);
    session.createTransaction();
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 1);
    SelectJob::whereData_t wheredata(
        new BasicDBSelectObject(
            Datatype::STRING, "myString", Data_t(new std::string("data"))));
    const auto selectRes = session.select(wheredata);
    EXPECT_EQ(selectRes.size(), 1);
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 1);
    session.commit();
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 0);
}


TEST_F(SessionTest, test_empty_select_auto_commit)
{
    Session session(1, m_auth, m_transactionManager, m_dbStore);
    SelectJob::whereData_t wheredata(
        new BasicDBSelectObject(
            Datatype::STRING, "myString", Data_t(new std::string("data"))));
    const auto selectRes = session.select(wheredata);
    EXPECT_EQ(selectRes.size(), 1);
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 0);
}


TEST_F(SessionTest, test_insert_from_db)
{
    Session session(1, m_auth, m_transactionManager, m_dbStore);
    session.createTransaction();
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 1);
    InsertJob::insertData_t insertdata(
        new BasicDBInsertObject(Datatype::STRING, "data_name",
            Data_t(new std::string("data"))));
    session.insert(insertdata);
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 1);
    session.commit();
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 0);
    EXPECT_EQ(m_dbStore.select().size(), 3);
}


TEST_F(SessionTest, test_insert_auto_commit)
{
    Session session(1, m_auth, m_transactionManager, m_dbStore);
    InsertJob::insertData_t insertdata(
        new BasicDBInsertObject(Datatype::STRING, "data_name",
            Data_t(new std::string("data"))));
    session.insert(insertdata);
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 0);
    EXPECT_EQ(m_dbStore.select().size(), 3);
}


TEST_F(SessionTest, test_update_from_db)
{
    Session session(1, m_auth, m_transactionManager, m_dbStore);
    session.createTransaction();
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 1);
    UpdateJob::updateData_t updatedata(
        new BasicDBUpdateObject(
            Datatype::STRING, "updated", Data_t(new std::string("updated")),
            Datatype::STRING, "myString", Data_t(new std::string("data"))));
    session.update(updatedata);
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 1);
    session.commit();
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 0);
    EXPECT_EQ(m_dbStore.select().size(), 2);
}


TEST_F(SessionTest, test_update_auto_commit)
{
    Session session(1, m_auth, m_transactionManager, m_dbStore);
    UpdateJob::updateData_t updatedata(
        new BasicDBUpdateObject(
            Datatype::STRING, "updated", Data_t(new std::string("updated")),
            Datatype::STRING, "myString", Data_t(new std::string("data"))));
    session.update(updatedata);
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 0);
    EXPECT_EQ(m_dbStore.select().size(), 2);
}

TEST_F(SessionTest, test_delete_from_db)
{
    Session session(1, m_auth, m_transactionManager, m_dbStore);
    session.createTransaction();
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 1);
    DeleteJob::whereData_t deletedata(
        new BasicDBDeleteObject(
            Datatype::STRING, "myString", Data_t(new std::string("data"))));
    session.deleteExec(deletedata);
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 1);
    session.commit();
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 0);
    EXPECT_EQ(m_dbStore.select().size(), 1);
}


TEST_F(SessionTest, test_delete_auto_commit)
{
    Session session(1, m_auth, m_transactionManager, m_dbStore);
    DeleteJob::whereData_t deletedata(
        new BasicDBDeleteObject(
            Datatype::STRING, "myString", Data_t(new std::string("data"))));
    session.deleteExec(deletedata);
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 0);
    EXPECT_EQ(m_dbStore.select().size(), 1);
}


TEST_F(SessionTest, test_complex)
{
    Session session(1, m_auth, m_transactionManager, m_dbStore);
    session.createTransaction();
    //1 insert
    InsertJob::insertData_t insertdata(
        new BasicDBInsertObject(Datatype::STRING, "data_name",
            Data_t(new std::string("data"))));
    session.insert(insertdata);

    //2 select
    SelectJob::whereData_t wheredata(
        new BasicDBSelectObject(
            Datatype::STRING, "myString", Data_t(new std::string("data"))));
    const auto selectRes = session.select(wheredata);
    EXPECT_EQ(selectRes.size(), 1);

    //3 update first insert
    UpdateJob::updateData_t updatedata(
        new BasicDBUpdateObject(
            Datatype::STRING, "updated", Data_t(new std::string("updated")),
            Datatype::STRING, "data_name", Data_t(new std::string("data"))));
    session.update(updatedata);

     //4 select last updated (3 step)
    SelectJob::whereData_t wheredataUpdated(
        new BasicDBSelectObject(
            Datatype::STRING, "updated", Data_t(nullptr)));
    const auto selectResUpdated = session.select(wheredataUpdated);
    EXPECT_EQ(selectResUpdated.size(), 1);

    //5 insert 2
    InsertJob::insertData_t insertdata2(
        new BasicDBInsertObject(Datatype::STRING, "data_name",
            Data_t(new std::string("data"))));
    session.insert(insertdata2);

    //5 delete last inserted 2
    DeleteJob::whereData_t deletedata(
        new BasicDBDeleteObject(
            Datatype::STRING, "data_name", Data_t(new std::string("data"))));
    session.deleteExec(deletedata);

    //6 select last deleted 2
    SelectJob::whereData_t selectdataDeleted(
        new BasicDBSelectObject(
            Datatype::STRING, "data_name", Data_t(nullptr)));
    const auto selectResDeleted = session.select(selectdataDeleted);
    EXPECT_EQ(selectResDeleted.size(), 0);

    //6 select last all string
    SelectJob::whereData_t wheredataAllString(
        new BasicDBSelectObject(
            Datatype::STRING, "", Data_t(nullptr)));
    const auto selectResAllString = session.select(wheredataAllString);
    EXPECT_EQ(selectResAllString.size(), 2);

    EXPECT_EQ(m_transactionManager.all_transactions().size(), 1);
    session.commit();
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 0);

    EXPECT_EQ(m_dbStore.select().size(), 3); // 1 inserted 

}


TEST_F(SessionTest, test_terminate)
{
    Session session(1, m_auth, m_transactionManager, m_dbStore);
    session.createTransaction();
    //1 insert
    InsertJob::insertData_t insertdata(
        new BasicDBInsertObject(Datatype::STRING, "data_name",
            Data_t(new std::string("data"))));
    session.insert(insertdata);

    session.terminate();
    EXPECT_EQ(m_transactionManager.all_transactions().size(), 0);
    EXPECT_EQ(m_dbStore.select().size(), 2);

}


TEST_F(SessionTest, test_insert_thread)
{
    Session session1(1, m_auth, m_transactionManager, m_dbStore);
    Session session2(1, m_auth, m_transactionManager, m_dbStore);
    const auto insertWork = [] (Session& session)
    {
        InsertJob::insertData_t insertdata(
            new BasicDBInsertObject(Datatype::STRING, "data_name",
                Data_t(new std::string("data"))));
        session.insert(insertdata);
    };

    std::thread thr1(insertWork, std::ref(session1));
    std::thread thr2(insertWork, std::ref(session2));
    if (thr1.joinable())
    {
        thr1.join();
    }
    if (thr2.joinable())
    {
        thr2.join();
    }


    EXPECT_EQ(m_transactionManager.all_transactions().size(), 0);


    EXPECT_EQ(m_dbStore.select().size(), 4);
}
