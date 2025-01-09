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
    std::shared_ptr<void> m_data;
protected:
    // metadata
    const std::size_t m_id;


public:
    BasicDBObject(const Datatype type,
     const std::string& name,
     std::shared_ptr<void> data)
    : BasicDBInsertObject(type, name, data),
      m_id(IDGenerator::getGenerator()->generateId())
    {

    }

    BasicDBObject(const BasicDBObject& obj)
    : BasicDBInsertObject(obj),
      m_id(IDGenerator::getGenerator()->generateId()) {

    }
    bool operator<(const BasicDBObject &obj) const
    {
      return id() < obj.id();
    }
    
    BasicDBObject &operator=(const BasicDBObject &obj) = delete;
    BasicDBObject(BasicDBObject&& obj) = delete;
    BasicDBObject &operator=(BasicDBObject && obj) = delete;

    void parent(BasicDBObject* parent) { m_parent = parent; }

    std::size_t id() const { return m_id; }

    using dataStore_t = std::unordered_map<int, std::shared_ptr<BasicDBObject>>;

    void updateName(const std::string& newName)
    {
      m_name = newName;
    }

    void updateData(std::shared_ptr<void> newData)
    {
      m_data = newData;
    }

    void updateType(const Datatype newType)
    {
      m_type = newType;
    }
    
};

#endif