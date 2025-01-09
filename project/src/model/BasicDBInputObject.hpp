#ifndef _BASIC_DB_INSERT_OBJECT_HPP_
#define _BASIC_DB_INSERT_OBJECT_HPP_

#include "Datatype.hpp"

#include <string>
#include <vector>
#include <memory>

class IBasicDBInputObject
{
public:

    virtual IBasicDBInputObject* parentRedo() const = 0;
    virtual Datatype typeRedo() const = 0;
    virtual std::string nameRedo() const = 0;
    virtual std::shared_ptr<void> dataRedo() const = 0;

    virtual IBasicDBInputObject* parentUndo() const { return nullptr; };
    virtual Datatype typeUndo() const { return Datatype::UNDEFINED; };
    virtual std::string nameUndo() const { return ""; };
    virtual std::shared_ptr<void> dataUndo() const { return std::shared_ptr<void>(nullptr); } 

};

class BasicDBInsertObject: public IBasicDBInputObject
{
    
protected:
    std::shared_ptr<void> m_data;
    // metadata
    Datatype m_type;
    std::string m_name;

    BasicDBInsertObject* m_parent;
public:
    BasicDBInsertObject(const Datatype type,
     const std::string& name,
     std::shared_ptr<void> data)
    : m_type(type),
      m_name(name),
      m_data(data)
    {

    }

    BasicDBInsertObject(const Datatype type,
     std::shared_ptr<void> data)
    : m_type(type),
      m_name(""),
      m_data(data)
    {
    }

    BasicDBInsertObject(const BasicDBInsertObject& obj)
    : m_type(obj.m_type),
      m_name(obj.m_name),
      m_data(obj.m_data)
    {

    }

    BasicDBInsertObject &operator=(const BasicDBInsertObject &obj) = delete;
    BasicDBInsertObject(BasicDBInsertObject&& obj) = delete;
    BasicDBInsertObject &operator=(BasicDBInsertObject && obj) = delete;

    IBasicDBInputObject* parentRedo() const override { return m_parent; }
    void parentRedo(BasicDBInsertObject* parent) { m_parent = parent; }


    Datatype typeRedo() const override { return m_type; }
    std::string nameRedo() const override { return m_name; }

    std::shared_ptr<void> dataRedo() const override { return m_data; }


    
};


class BasicDBUpdateObject: public BasicDBInsertObject
{
    
protected:
    std::shared_ptr<void> m_dataUndo;
    // metadata
    const Datatype m_typeUndo;
    const std::string m_nameUndo;
    
    BasicDBUpdateObject* m_parentUndo;
public:
    BasicDBUpdateObject(const Datatype type,
     const std::string& name,
     std::shared_ptr<void> data,
     const Datatype typeUndo,
     const std::string& nameUndo,
     std::shared_ptr<void> dataUndo)
    : BasicDBInsertObject(type, name, data),
      m_dataUndo(dataUndo),
      m_nameUndo(nameUndo),
      m_typeUndo(typeUndo)
    {

    }

    BasicDBUpdateObject(const BasicDBUpdateObject& obj)
    : BasicDBInsertObject(obj.m_type, obj.m_name, obj.m_data),
      m_typeUndo(obj.m_typeUndo),
      m_nameUndo(obj.m_nameUndo),
      m_dataUndo(obj.m_dataUndo)
    {

    }

    BasicDBUpdateObject &operator=(const BasicDBUpdateObject &obj) = delete;
    BasicDBUpdateObject(BasicDBUpdateObject&& obj) = delete;
    BasicDBUpdateObject &operator=(BasicDBUpdateObject && obj) = delete;

    IBasicDBInputObject* parentUndo() const override { return m_parentUndo; }
    void parentUndo(BasicDBUpdateObject* parent) { m_parentUndo = parent; }


    Datatype typeUndo() const override { return m_typeUndo; }
    std::string nameUndo() const override { return m_nameUndo; }

    std::shared_ptr<void> dataUndo() const override { return m_dataUndo; }
    
};


class BasicDBDeleteObject: public BasicDBInsertObject
{
    bool m_cascade;
public:
    BasicDBDeleteObject(const Datatype type,
     const std::string& name,
     std::shared_ptr<void> data,
     const bool cascade)
    : BasicDBInsertObject(type, name, data), 
      m_cascade(cascade)
    {

    }

    BasicDBDeleteObject(const Datatype type,
     std::shared_ptr<void> data,
     const bool cascade = false)
    : BasicDBInsertObject(type, data),
      m_cascade(cascade)
    {
    }

    BasicDBDeleteObject(const BasicDBDeleteObject& obj)
    : BasicDBInsertObject(obj),
      m_cascade(obj.m_cascade)
    {

    }

    BasicDBDeleteObject &operator=(const BasicDBDeleteObject &obj) = delete;
    BasicDBDeleteObject(BasicDBDeleteObject&& obj) = delete;
    BasicDBDeleteObject &operator=(BasicDBDeleteObject && obj) = delete;

    Datatype typeUndo() const override { return typeRedo(); }
    std::string nameUndo() const override { return nameRedo(); }
    bool cascade() const { return m_cascade; }

};

#endif