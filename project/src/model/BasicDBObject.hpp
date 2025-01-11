#ifndef _BASIC_DB_OBJECT_HPP_
#define _BASIC_DB_OBJECT_HPP_

#include "Datatype.hpp"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "../db/IdGenerator.hpp"
#include "BasicDBInputObject.hpp"

class BasicDBObject: public BasicDBInsertObject
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

    void parent(BasicDBObject::parent_t parent) { m_parent = parent; }

    std::size_t id() const { return m_id; }


    void updateName(const std::string& newName)
    {
      m_name = newName;
    }

    void updateData(Data_t newData)
    {
      m_data = newData;
    }

    void updateType(const Datatype newType)
    {
      m_type = newType;
    }
    
};

#endif