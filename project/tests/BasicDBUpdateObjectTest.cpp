#include <gtest/gtest.h>
#include "../src/model/BasicDBInputObject.hpp"
#include "../src/db/DataComparator.hpp"

TEST(basicdbupdateobject_test_case, test1)
{
    const Datatype typeRedo = Datatype::INT;
    const std::string nameRedo = "myDataRedo";
    const Data_t dataRedo = Data_t(new int (90));

    const Datatype typeUndo = Datatype::INT;
    const std::string nameUndo = "myDataUndo";
    const Data_t dataUndo = Data_t(new int (90));
    BasicDBUpdateObject obj(typeRedo, nameRedo, dataRedo,
        typeUndo, nameUndo, dataUndo);


    EXPECT_EQ(obj.dataRedo(), dataRedo);
    EXPECT_EQ(obj.data(), dataRedo);
    EXPECT_EQ(obj.dataWhere(), dataUndo);
    EXPECT_EQ(obj.dataUndo(), dataUndo);

    EXPECT_EQ(obj.typeRedo(), typeRedo);
    EXPECT_EQ(obj.type(), typeRedo);
    EXPECT_EQ(obj.typeWhere(), typeUndo);
    EXPECT_EQ(obj.typeUndo(), typeUndo);
    
    EXPECT_EQ(obj.nameRedo(), nameRedo);
    EXPECT_EQ(obj.name(), nameRedo);
    EXPECT_EQ(obj.nameWhere(), nameUndo);
    EXPECT_EQ(obj.nameUndo(), nameUndo);
    
    EXPECT_TRUE(obj.parentRedo().get() == nullptr);
    EXPECT_TRUE(obj.parent().get() == nullptr);

    EXPECT_TRUE(DataComparator::isPlainDataEqual(DataWithType(typeRedo, dataRedo),
         DataWithType(obj.typeRedo(), obj.dataRedo())));

    EXPECT_TRUE(DataComparator::isPlainDataEqual(DataWithType(typeUndo, dataUndo),
         DataWithType(obj.typeUndo(), obj.dataUndo())));
}



