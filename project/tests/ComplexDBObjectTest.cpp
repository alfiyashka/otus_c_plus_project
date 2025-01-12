#include <gtest/gtest.h>
#include "../src/model/ComplexDBObject.hpp"
#include "../src/db/DataComparator.hpp"

TEST(coomplexdbobject_test_case, test1)
{
    const Datatype type = Datatype::INT;
    const std::string name = "myData";
    const Data_t data = Data_t(new int (90));
    BasicDBObject::pointer_t obj(new BasicDBObject(type, name, data));

    const std::string complexName = "myComplexData";
    ComplexDBObject::pointer_t complexObj(new ComplexDBObject(complexName));
    complexObj->addChild(obj);


    EXPECT_EQ(complexObj->nameRedo(), complexName);
    EXPECT_EQ(complexObj->name(), complexName);
    EXPECT_EQ(complexObj->getChildrens().size(), 1);
    EXPECT_EQ(complexObj->childsRedo().size(), 1);
    EXPECT_EQ(complexObj->type(), Datatype::COMPOSITE);
    EXPECT_EQ(complexObj->typeRedo(), Datatype::COMPOSITE);

    ASSERT_TRUE(complexObj->getChild(obj->id()).get() != nullptr);
    ASSERT_TRUE(complexObj->getChild(obj->id()).get() == obj.get());

    complexObj->removeChild(obj->id());
    EXPECT_EQ(complexObj->getChildrens().size(), 0);

}





