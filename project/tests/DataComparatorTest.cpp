#include "../db/DataComparator.hpp"
#include <gtest/gtest.h>
#include <string>
#include <tuple>

TEST(data_comparator_test_case_plain_data, compare_data_ints)
{
    const DataWithType data1 (Datatype::INT, Data_t(new int(90000)));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data1, data1));
    const DataWithType data2 (Datatype::INT, Data_t(new int(90001)));

    EXPECT_FALSE(DataComparator::isPlainDataEqual(data1, data2));

    const DataWithType data1same (Datatype::INT, Data_t(new int(90000)));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data1, data1same));

    const DataWithType data3null (Datatype::INT, Data_t(nullptr));

    EXPECT_FALSE(DataComparator::isPlainDataEqual(data1, data3null));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data3null, data3null));
}


TEST(data_comparator_test_case_plain_data, compare_data_bytes)
{
    const DataWithType data1 (Datatype::BYTE, Data_t(new int8_t(90)));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data1, data1));
    const DataWithType data2 (Datatype::BYTE, Data_t(new int8_t(91)));

    EXPECT_FALSE(DataComparator::isPlainDataEqual(data1, data2));

    const DataWithType data1same (Datatype::BYTE, Data_t(new int8_t(90)));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data1, data1same));

    const DataWithType data3null (Datatype::BYTE, Data_t(nullptr));

    EXPECT_FALSE(DataComparator::isPlainDataEqual(data1, data3null));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data3null, data3null));
}

TEST(data_comparator_test_case_plain_data, compare_data_smallints)
{
    const DataWithType data1 (Datatype::SMALLINT, Data_t(new int16_t(900)));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data1, data1));
    const DataWithType data2 (Datatype::SMALLINT, Data_t(new int16_t(901)));

    EXPECT_FALSE(DataComparator::isPlainDataEqual(data1, data2));

    const DataWithType data1same (Datatype::SMALLINT, Data_t(new int16_t(900)));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data1, data1same));

    const DataWithType data3null (Datatype::SMALLINT, Data_t(nullptr));

    EXPECT_FALSE(DataComparator::isPlainDataEqual(data1, data3null));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data3null, data3null));
}


TEST(data_comparator_test_case_plain_data, compare_data_bigints)
{
    const DataWithType data1 (Datatype::BIGINT, Data_t(new int64_t(900000000)));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data1, data1));
    const DataWithType data2 (Datatype::BIGINT, Data_t(new int64_t(900000001)));

    EXPECT_FALSE(DataComparator::isPlainDataEqual(data1, data2));

    const DataWithType data1same (Datatype::BIGINT, Data_t(new int64_t(900000000)));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data1, data1same));

    const DataWithType data3null (Datatype::BIGINT, Data_t(nullptr));

    EXPECT_FALSE(DataComparator::isPlainDataEqual(data1, data3null));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data3null, data3null));
}


TEST(data_comparator_test_case_plain_data, compare_data_floats)
{
    
    const DataWithType data1 (Datatype::FLOAT, Data_t(new float(123.123)));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data1, data1));
    const DataWithType data2 (Datatype::FLOAT, Data_t(new float(1230.125)));

    EXPECT_FALSE(DataComparator::isPlainDataEqual(data1, data2));

    const DataWithType data1same (Datatype::FLOAT, Data_t(new float(123.123)));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data1, data1same));

    const DataWithType data3null (Datatype::FLOAT, Data_t(nullptr));

    EXPECT_FALSE(DataComparator::isPlainDataEqual(data1, data3null));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data3null, data3null));
}

TEST(data_comparator_test_case_plain_data, compare_data_doubles)
{
    
    const DataWithType data1 (Datatype::DOUBLE, Data_t(new double(12300000000.123)));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data1, data1));
    const DataWithType data2 (Datatype::DOUBLE, Data_t(new double(123000000000.125)));

    EXPECT_FALSE(DataComparator::isPlainDataEqual(data1, data2));

    const DataWithType data1same (Datatype::DOUBLE, Data_t(new double(12300000000.123)));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data1, data1same));

    const DataWithType data3null (Datatype::DOUBLE, Data_t(nullptr));

    EXPECT_FALSE(DataComparator::isPlainDataEqual(data1, data3null));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data3null, data3null));
}

TEST(data_comparator_test_case_plain_data, compare_data_strings)
{
    const DataWithType data1 (Datatype::STRING, Data_t(new std::string("data")));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data1, data1));
    const DataWithType data2 (Datatype::STRING, Data_t(new std::string("another_data")));

    EXPECT_FALSE(DataComparator::isPlainDataEqual(data1, data2));

    const DataWithType data1same (Datatype::STRING, Data_t(new std::string("data")));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data1, data1same));

    const DataWithType data3null (Datatype::STRING, Data_t(nullptr));

    EXPECT_FALSE(DataComparator::isPlainDataEqual(data1, data3null));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(data3null, data3null));
}

TEST(data_comparator_test_case_plain_data, compare_data_different)
{
    const DataWithType data1 (Datatype::STRING, Data_t(new std::string("data")));
    const DataWithType data2 (Datatype::INT, Data_t(new int(12)));
    EXPECT_FALSE(DataComparator::isPlainDataEqual(data1, data2));

    const DataWithType data1null (Datatype::STRING, Data_t(nullptr));
    const DataWithType data2null (Datatype::INT, Data_t(nullptr));
    EXPECT_FALSE(DataComparator::isPlainDataEqual(data1null, data2null));   
}

