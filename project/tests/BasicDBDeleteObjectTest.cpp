#include <gtest/gtest.h>
#include "../src/model/BasicDBInputObject.hpp"
#include "../src/db/DataComparator.hpp"

TEST(basicdbdeleteobject_test_case, test1)
{
    const Datatype type = Datatype::INT;
    const std::string name = "myData";
    const Data_t data = Data_t(new int (90));
    BasicDBDeleteObject obj(type, name, data);


    EXPECT_EQ(obj.dataUndo(), data);
    EXPECT_EQ(obj.dataWhere(), data);
    EXPECT_EQ(obj.data(), data);
    EXPECT_TRUE(obj.parentUndo().get() == nullptr);
    EXPECT_TRUE(obj.parent().get() == nullptr);
    EXPECT_EQ(obj.typeUndo(), type);
    EXPECT_EQ(obj.type(), type);
    EXPECT_EQ(obj.typeWhere(), type);
    EXPECT_EQ(obj.nameUndo(), name);
    EXPECT_EQ(obj.nameWhere(), name);
    EXPECT_EQ(obj.name(), name);
    EXPECT_TRUE(DataComparator::isPlainDataEqual(DataWithType(type, data),
         DataWithType(obj.typeUndo(), obj.dataUndo())));      
}


