#include "Utils.hpp"
#include <string>

void insertDB2Rows(DBStore& dbStore)
{
    tempDbStoreType tempDBStore;
    tempDbStoreType::value_type::second_type::second_type tempInsertRes;
    auto dataToStore = BasicDBObject::pointer_t(new BasicDBObject(Datatype::INT, "myInt", Data_t(new int(30))));
    auto dataToStore1 = BasicDBObject::pointer_t(new BasicDBObject(Datatype::STRING, "myString", Data_t(new std::string("data"))));

    tempInsertRes.push_back(dataToStore);
    tempInsertRes.push_back(dataToStore1);
    std::size_t id = 1;
    tempDBStore.insert(
            tempDbStoreType::value_type(id, std::pair(JOB_TYPE::INSERT, tempInsertRes)));

    dbStore.insert(tempDBStore);
}