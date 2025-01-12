#ifndef _BASIC_DB_INSERT_OBJECT_HPP_
#define _BASIC_DB_INSERT_OBJECT_HPP_

#include "Datatype.hpp"
#include "../db/DataComparator.hpp"

#include <string>
#include <vector>
#include <memory>
#include <map>

class ComplexDBObject;

class BasicDBAbstractObject
{
public:
    using pointer_t = std::shared_ptr<BasicDBAbstractObject>;
    using weak_ptr = std::weak_ptr<BasicDBAbstractObject>;
    using childs_t = std::map<std::size_t, BasicDBAbstractObject::pointer_t>;
    using parent_t = std::shared_ptr<ComplexDBObject>;    

    BasicDBAbstractObject(const Datatype type,
     const std::string& name,
     Data_t data)
    : m_type(type),
      m_name(name),
      m_data(data)
    {

    }

    BasicDBAbstractObject(const Datatype type,
     Data_t data)
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

    bool operator==(const BasicDBAbstractObject &obj) const
    {
      if (m_type != obj.m_type)
      {
        return false;
      }
      else if (m_name != obj.m_name)
      {
        return false;
      }
      else if (m_type != Datatype::COMPOSITE)
      {
        return DataComparator::isPlainDataEqual(DataWithType(m_type, m_data), 
          DataWithType(obj.m_type, obj.m_data));
      }
      return false;
    }

    BasicDBAbstractObject &operator=(const BasicDBAbstractObject &obj) = delete;
    BasicDBAbstractObject(BasicDBAbstractObject&& obj) = delete;
    BasicDBAbstractObject &operator=(BasicDBAbstractObject&& obj) = delete;
    virtual ~BasicDBAbstractObject() {}

    Data_t data() const { return m_data; }
    Datatype type() const { return m_type; }
    const std::string& name() const { return m_name; }

    const parent_t parent() const { return m_parent; }
    
protected:
    Data_t m_data;
    // metadata
    Datatype m_type;
    std::string m_name;
    parent_t m_parent;      
};

class IBasicDBWhereObject
{
public:
    virtual ~IBasicDBWhereObject() {}

    virtual Datatype typeWhere() const = 0;
    virtual std::string nameWhere() const = 0;
    virtual Data_t dataWhere() const = 0;
    
};

class IBasicDBRedoObject: public IBasicDBWhereObject
{
public:
    virtual ~IBasicDBRedoObject() {}

    virtual BasicDBAbstractObject::parent_t parentRedo() const = 0;
    virtual BasicDBAbstractObject::childs_t childsRedo() const { return BasicDBAbstractObject::childs_t(); }
    virtual Datatype typeRedo() const = 0;
    virtual std::string nameRedo() const = 0;
    virtual Data_t dataRedo() const = 0;

    Datatype typeWhere() const override { return typeRedo(); }
    std::string nameWhere() const override { return nameRedo(); }
    Data_t dataWhere() const override { return dataRedo(); }
};



class IBasicDBUndoObject: public IBasicDBWhereObject
{
public:
    virtual ~IBasicDBUndoObject() {}

    virtual BasicDBAbstractObject::parent_t parentUndo() const = 0;
    virtual Datatype typeUndo() const = 0;
    virtual std::string nameUndo() const = 0;
    virtual Data_t dataUndo() const = 0;

    Datatype typeWhere() const override { return typeUndo(); }
    std::string nameWhere() const override { return nameUndo(); }
    Data_t dataWhere() const override { return dataUndo(); }
};


class IBasicDBRedoUndoObject: public IBasicDBRedoObject
{
public:
    virtual ~IBasicDBRedoUndoObject() {}

    virtual BasicDBAbstractObject::parent_t parentUndo() const = 0;
    virtual Datatype typeUndo() const { return Datatype::UNDEFINED; };
    virtual std::string nameUndo() const { return ""; };
    virtual Data_t dataUndo() const { return Data_t(nullptr); }

    Datatype typeWhere() const override { return typeUndo(); }
    std::string nameWhere() const override { return nameUndo(); }
    Data_t dataWhere() const override { return dataUndo(); } 
};


class BasicDBInsertObject: public BasicDBAbstractObject, public IBasicDBRedoObject
{
    
public:
    BasicDBInsertObject(const Datatype type,
     const std::string& name,
     Data_t data)
    : BasicDBAbstractObject(type, name, data)
    {

    }

    BasicDBInsertObject(const Datatype type,
     Data_t data): BasicDBAbstractObject(type, data)
    {
    }

    BasicDBInsertObject(const BasicDBInsertObject& obj)
    : BasicDBAbstractObject(obj)
    {

    }

    BasicDBInsertObject &operator=(const BasicDBInsertObject &obj) = delete;
    BasicDBInsertObject(BasicDBInsertObject&& obj) = delete;
    BasicDBInsertObject &operator=(BasicDBInsertObject && obj) = delete;

    BasicDBAbstractObject::parent_t parentRedo() const { return m_parent; }
    void parentRedo(BasicDBInsertObject::parent_t parent) { m_parent = parent; }


    Datatype typeRedo() const override { return m_type; }
    std::string nameRedo() const override { return m_name; }

    Data_t dataRedo() const override { return m_data; }
    bool operator==(const BasicDBInsertObject& obj) const = delete;
};

class BasicDBSelectObject : public BasicDBInsertObject
{
public:
    BasicDBSelectObject(const Datatype type,
     const std::string& name,
     Data_t data)
    : BasicDBInsertObject(type, name, data)
    {

    }

    BasicDBSelectObject(const Datatype type,
     Data_t data): BasicDBInsertObject(type, data)
    {
    }

    BasicDBSelectObject(const std::string& name)
     : BasicDBInsertObject(Datatype::UNDEFINED, name, Data_t(nullptr))
    {
    }    

    BasicDBSelectObject(const BasicDBSelectObject& obj)
    : BasicDBInsertObject(obj)
    {

    }

    BasicDBSelectObject &operator=(const BasicDBSelectObject &obj) = delete;
    BasicDBSelectObject(BasicDBSelectObject&& obj) = delete;
    BasicDBSelectObject &operator=(BasicDBSelectObject && obj) = delete;
    bool operator==(const BasicDBSelectObject& obj) const = delete;
};

class BasicDBUpdateObject: public BasicDBAbstractObject, public IBasicDBRedoUndoObject
{    
protected:
    Data_t m_dataUndo;
    // metadata
    const Datatype m_typeUndo;
    const std::string m_nameUndo;
    
    BasicDBAbstractObject::parent_t m_parentUndo;
public:
    BasicDBUpdateObject(const Datatype type,
     const std::string& name,
     Data_t data,
     const Datatype typeUndo,
     const std::string& nameUndo,
     Data_t dataUndo)
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
    bool operator==(const BasicDBUpdateObject& obj) = delete;

    BasicDBAbstractObject::parent_t parentUndo() const override { return m_parentUndo; }
    void parentUndo(BasicDBAbstractObject::parent_t parent) { m_parentUndo = parent; }

    Datatype typeUndo() const override { return m_typeUndo; }
    std::string nameUndo() const override { return m_nameUndo; }

    Data_t dataUndo() const override { return m_dataUndo; }

    Datatype typeRedo() const override { return m_type; }
    std::string nameRedo() const override { return m_name; }

    Data_t dataRedo() const override { return m_data; }  

    BasicDBAbstractObject::parent_t parentRedo() const override { return m_parent; }
    void parentRedo(BasicDBAbstractObject::parent_t parent) { m_parent = parent; } 
    
};


class BasicDBDeleteObject:  public BasicDBAbstractObject, public IBasicDBUndoObject
{
    bool m_cascade;
public:
    BasicDBDeleteObject(const Datatype type,
     const std::string& name,
     Data_t data,
     const bool cascade)
    : BasicDBAbstractObject(type, name, data), 
      m_cascade(cascade)
    {

    }

    BasicDBDeleteObject(const Datatype type,
     const std::string& name,
     Data_t data)
    : BasicDBAbstractObject(type, name, data), 
      m_cascade(false)
    {

    }

    BasicDBDeleteObject(const std::string& name)
    : BasicDBAbstractObject(Datatype::UNDEFINED, name, Data_t(nullptr)), 
      m_cascade(false)
    {

    }

    BasicDBDeleteObject(const Datatype type,
     Data_t data,
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
    bool operator==(BasicDBDeleteObject && obj) const = delete;

    Datatype typeUndo() const override { return m_type; }
    std::string nameUndo() const override { return m_name; }

    Data_t dataUndo() const override { return m_data; }

    BasicDBAbstractObject::parent_t parentUndo() const override { return m_parent; }

};

#endif