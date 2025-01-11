#include "../src/db/Job.hpp"

#include <gtest/gtest.h>

TEST(db_insert_job_test_case, test1)
{
    tempDbStoreType tempDBStore;
    
    InsertJob::insertData_t insertdata(
        new BasicDBInsertObject(Datatype::STRING, "data_name",
            Data_t(new std::string("data"))));
    InsertJob job(1, tempDBStore, insertdata);

    EXPECT_EQ(tempDBStore.size(), 0);

    job.run();

    EXPECT_EQ(tempDBStore.size(), 1);
}

