#ifndef _BASIC_DB_INSERT_OBJECT_HPP_
#define _BASIC_DB_INSERT_OBJECT_HPP_

#include "Datatype.hpp"

#include <string>
#include <vector>
#include <memory>


class BasicDBAbstractObject
{
protected:
    std::shared_ptr<void> m_data;
    // metadata
    Datatype m_type;
    std::string m_name;

    BasicDBAbstractObject* m_parent;
public:
    BasicDBAbstractObject(const Datatype type,
     const std::string& name,
     std::shared_ptr<void> data)
    : m_type(type),
      m_name(name),
      m_data(data)
    {

    }

    BasicDBAbstractObject(const Datatype type,
     std::shared_ptr<void> data)
    : m_type(type),
      m_name(""),
      m_data(data)
    {
    }

    BasicDBAbstractObject(const BasicDBAbstractObject& obj)
    : m_type(obj.m_type),
      m_name(obj.m_name),
      m_data(obj.m_data)
    {

    }

    BasicDBAbstractObject &operator=(const BasicDBAbstractObject &obj) = delete;
    BasicDBAbstractObject(BasicDBAbstractObject&& obj) = delete;
    BasicDBAbstractObject &operator=(BasicDBAbstractObject && obj) = delete;  
};

class IBasicDBWhereObject
{
public:
    virtual ~IBasicDBWhereObject() {}

    virtual BasicDBAbstractObject* parentWhere() const = 0;
    virtual Datatype typeWhere() const = 0;
    virtual std::string nameWhere() const = 0;
    virtual std::shared_ptr<void> dataWhere() const = 0;
};

class IBasicDBRedoObject
{
public:
    virtual ~IBasicDBRedoObject() {}

    virtual BasicDBAbstractObject* parentRedo() const = 0;
    virtual Datatype typeRedo() const = 0;
    virtual std::string nameRedo() const = 0;
    virtual std::shared_ptr<void> dataRedo() const = 0;
};



class IBasicDBUndoObject: public IBasicDBWhereObject
{
public:
    virtual ~IBasicDBUndoObject() {}

    virtual BasicDBAbstractObject* parentUndo() const = 0;
    virtual Datatype typeUndo() const = 0;
    virtual std::string nameUndo() const = 0;
    virtual std::shared_ptr<void> dataUndo() const = 0;

    BasicDBAbstractObject* parentWhere() const override { return parentUndo(); }
    Datatype typeWhere() const override { return typeUndo(); }
    std::string nameWhere() const override { return nameUndo(); }
    std::shared_ptr<void> dataWhere() const override { return dataUndo(); }
};


class IBasicDBRedoUndoObject: public IBasicDBRedoObject, public IBasicDBWhereObject
{
public:
    virtual ~IBasicDBRedoUndoObject() {}

    virtual BasicDBAbstractObject* parentUndo() const { return nullptr; };
    virtual Datatype typeUndo() const { return Datatype::UNDEFINED; };
    virtual std::string nameUndo() const { return ""; };
    virtual std::shared_ptr<void> dataUndo() const { return std::shared_ptr<void>(nullptr); }

    BasicDBAbstractObject* parentWhere() const override { return parentUndo(); }
    Datatype typeWhere() const override { return typeUndo(); }
    std::string nameWhere() const override { return nameUndo(); }
    std::shared_ptr<void> dataWhere() const override { return dataUndo(); } 
};


class BasicDBInsertObject: public BasicDBAbstractObject, public IBasicDBRedoObject
{
    
public:
    BasicDBInsertObject(const Datatype type,
     const std::string& name,
     std::shared_ptr<void> data)
    : BasicDBAbstractObject(type, name, data)
    {

    }

    BasicDBInsertObject(const Datatype type,
     std::shared_ptr<void> data): BasicDBAbstractObject(type, data)
    {
    }

    BasicDBInsertObject(const BasicDBInsertObject& obj)
    : BasicDBAbstractObject(obj)
    {

    }

    BasicDBInsertObject &operator=(const BasicDBInsertObject &obj) = delete;
    BasicDBInsertObject(BasicDBInsertObject&& obj) = delete;
    BasicDBInsertObject &operator=(BasicDBInsertObject && obj) = delete;

    BasicDBAbstractObject* parentRedo() const { return m_parent; }
    void parentRedo(BasicDBInsertObject* parent) { m_parent = parent; }


    Datatype typeRedo() const override { return m_type; }
    std::string nameRedo() const override { return m_name; }

    std::shared_ptr<void> dataRedo() const override { return m_data; }    
};


class BasicDBUpdateObject: public BasicDBAbstractObject, public IBasicDBRedoUndoObject
{    
protected:
    std::shared_ptr<void> m_dataUndo;
    // metadata
    const Datatype m_typeUndo;
    const std::string m_nameUndo;
    
    BasicDBAbstractObject* m_parentUndo;
public:
    BasicDBUpdateObject(const Datatype type,
     const std::string& name,
     std::shared_ptr<void> data,
     const Datatype typeUndo,
     const std::string& nameUndo,
     std::shared_ptr<void> dataUndo)
    : BasicDBAbstractObject(type, name, data),
      m_dataUndo(dataUndo),
      m_nameUndo(nameUndo),
      m_typeUndo(typeUndo)
    {

    }

    BasicDBUpdateObject(const BasicDBUpdateObject& obj)
    : BasicDBAbstractObject(obj.m_type, obj.m_name, obj.m_data),
      m_typeUndo(obj.m_typeUndo),
      m_nameUndo(obj.m_nameUndo),
      m_dataUndo(obj.m_dataUndo)
    {

    }

    BasicDBUpdateObject &operator=(const BasicDBUpdateObject &obj) = delete;
    BasicDBUpdateObject(BasicDBUpdateObject&& obj) = delete;
    BasicDBUpdateObject &operator=(BasicDBUpdateObject && obj) = delete;

    BasicDBAbstractObject* parentUndo() const override { return m_parentUndo; }
    void parentUndo(BasicDBAbstractObject* parent) { m_parentUndo = parent; }


    Datatype typeUndo() const override { return m_typeUndo; }
    std::string nameUndo() const override { return m_nameUndo; }

    std::shared_ptr<void> dataUndo() const override { return m_dataUndo; }

    Datatype typeRedo() const override { return m_type; }
    std::string nameRedo() const override { return m_name; }

    std::shared_ptr<void> dataRedo() const override { return m_data; }  

    BasicDBAbstractObject* parentRedo() const override { return m_parent; }
    void parentRedo(BasicDBAbstractObject* parent) { m_parent = parent; } 
    
};


class BasicDBDeleteObject:  public BasicDBAbstractObject, public IBasicDBUndoObject
{
    bool m_cascade;
public:
    BasicDBDeleteObject(const Datatype type,
     const std::string& name,
     std::shared_ptr<void> data,
     const bool cascade)
    : BasicDBAbstractObject(type, name, data), 
      m_cascade(cascade)
    {

    }

    BasicDBDeleteObject(const Datatype type,
     std::shared_ptr<void> data,
     const bool cascade = false)
    : BasicDBAbstractObject(type, data),
      m_cascade(cascade)
    {
    }

    BasicDBDeleteObject(const BasicDBDeleteObject& obj)
    : BasicDBAbstractObject(obj),
      m_cascade(obj.m_cascade)
    {

    }

    BasicDBDeleteObject &operator=(const BasicDBDeleteObject &obj) = delete;
    BasicDBDeleteObject(BasicDBDeleteObject&& obj) = delete;
    BasicDBDeleteObject &operator=(BasicDBDeleteObject && obj) = delete;

    Datatype typeUndo() const override { return m_type; }
    std::string nameUndo() const override { return m_name; }

    std::shared_ptr<void> dataUndo() const override { return m_data; }

    BasicDBAbstractObject* parentUndo() const override { return m_parent; }

};

#endif