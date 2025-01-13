#ifndef _COMPLEX_DB_OBJECT_HPP_
#define _COMPLEX_DB_OBJECT_HPP_

#include "BasicDBObject.hpp"

#include <string>
#include <vector>
#include <memory>
#include <map>

class ComplexDBObject : public BasicDBObject, public std::enable_shared_from_this<ComplexDBObject>
{

public:
    using pointer_t = std::shared_ptr<ComplexDBObject>;
    ComplexDBObject(const std::string &name)
         : BasicDBObject(Datatype::COMPOSITE, name, Data_t(nullptr))
    {
    }

    ComplexDBObject(const ComplexDBObject& obj)
         : BasicDBObject((const BasicDBObject)obj),
         m_childrens(obj.getChildrens())
    {
    }

    ComplexDBObject &operator=(const ComplexDBObject &obj) = delete;
    ComplexDBObject(ComplexDBObject &&obj) = delete;
    ComplexDBObject &operator=(ComplexDBObject &&obj) = delete;
    
    void addChild(BasicDBObject::pointer_t child);

    void removeChild(BasicDBObject::pointer_t child);

    void removeChild(std::size_t id);

    BasicDBAbstractObject::pointer_t getChild(std::size_t id);

    BasicDBAbstractObject::childs_t childsRedo() const override { return getChildrens(); }
    BasicDBAbstractObject::childs_t getChilds() const { return getChildrens(); }
    
    const BasicDBAbstractObject::childs_t &getChildrens() const;

    void updateChilds(const BasicDBAbstractObject::childs_t& childrens);

    bool operator==(const ComplexDBObject &obj) const;


private:
    BasicDBAbstractObject::childs_t m_childrens;

    Data_t dataWhere() const override { return dataRedo(); }
    Data_t dataRedo() const override { return m_data; }

    void updateData(Data_t newData) override { }


};

#endif