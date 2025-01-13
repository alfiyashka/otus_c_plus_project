#ifndef _BASIC_DB_OBJECT_HPP_
#define _BASIC_DB_OBJECT_HPP_

#include "Datatype.hpp"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "../db/IdGenerator.hpp"
#include "BasicDBInputObject.hpp"


class IBasicDBSelectObject
{
public:
    using pointer_t = std::shared_ptr<IBasicDBSelectObject>;
    using selectList_t = std::vector<IBasicDBSelectObject::pointer_t>;
    virtual ~IBasicDBSelectObject() {}

    virtual Datatype getType() const = 0;
    virtual std::string getName() const = 0;
    virtual Data_t getData() const = 0;
    virtual std::size_t getId() const = 0;
    virtual BasicDBAbstractObject::childs_t getChilds() const { return BasicDBAbstractObject::childs_t(); }
    virtual BasicDBAbstractObject::parent_t getParent() const = 0;

    bool operator<(const IBasicDBSelectObject &obj) const
    {
      return getId() < obj.getId();
    }
    
};

class BasicDBObject: public BasicDBInsertObject, public IBasicDBSelectObject
{
protected:
    // metadata
    const std::size_t m_id;
public:
    using pointer_t = std::shared_ptr<BasicDBObject>;
    using dataStore_t = std::unordered_map<int, pointer_t>;
    using dataList_t = std::vector<pointer_t>;


    BasicDBObject(const Datatype type,
     const std::string& name,
     Data_t data)
    : BasicDBInsertObject(type, name, data),
      m_id(IDGenerator::getGenerator()->generateId())
    {

    }

    BasicDBObject(const BasicDBObject& obj)
    : BasicDBInsertObject(obj),
      m_id(obj.id()) {

    }
    bool operator<(const BasicDBObject &obj) const
    {
      return id() < obj.id();
    }

    BasicDBObject &operator=(const BasicDBObject &obj) = delete;
    BasicDBObject(BasicDBObject&& obj) = delete;
    BasicDBObject &operator=(BasicDBObject && obj) = delete;

    void setParent(BasicDBObject::parent_t parent)
    { 
      m_parent = parent;
    }

    std::size_t id() const { return m_id; }


    void updateName(const std::string& newName)
    {
      m_name = newName;
    }

    virtual void updateData(Data_t newData)
    {
      m_data = newData;
    }

    void updateType(const Datatype newType)
    {
      m_type = newType;
    }


    Datatype getType() const override { return type();}
    std::string getName() const override { return name();}
    Data_t getData() const override { return data();}
    std::size_t getId() const override { return id();}
    BasicDBAbstractObject::parent_t getParent() const override { return parent();}

private:
   
    // inaccessible forcely
    Datatype typeWhere() const override { return typeRedo(); }
    std::string nameWhere() const override { return nameRedo(); }
    Data_t dataWhere() const override { return dataRedo(); }

    BasicDBAbstractObject::childs_t childsRedo() const override { return BasicDBAbstractObject::childs_t(); };


};

#endif