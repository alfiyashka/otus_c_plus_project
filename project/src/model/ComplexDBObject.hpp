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
    ComplexDBObject(const std::string &name,
                    Data_t data)
         : BasicDBObject(Datatype::COMPOSITE, name, data)
    {
    }

    ComplexDBObject(const ComplexDBObject& obj)
         : BasicDBObject(obj),
         m_childrens(obj.getChildrens())
    {
    }

    ComplexDBObject &operator=(const ComplexDBObject &obj) = delete;
    ComplexDBObject(ComplexDBObject &&obj) = delete;
    ComplexDBObject &operator=(ComplexDBObject &&obj) = delete;

    using childs_t = std::map<std::size_t, BasicDBObject::pointer_t>;

    void addChild(BasicDBObject::pointer_t child)
    {
        m_childrens.insert(childs_t::value_type(child->id(), child));
        child->parent(shared_from_this());
    }

    void removeChild(BasicDBObject::pointer_t child)
    {
        child->parent(BasicDBObject::pointer_t(nullptr));
        m_childrens.erase(child->id());
    }

    void removeChild(std::size_t id)
    {
        const auto find = m_childrens.find(id);
        if (find == m_childrens.end())
        {
            return;
        }
        find->second->parent(BasicDBObject::pointer_t(nullptr));
        m_childrens.erase(id);
    }

    BasicDBObject::pointer_t getChild(std::size_t id)
    {
        const auto found = m_childrens.find(id);
        if (found != m_childrens.end())
        {
            return found->second;
        }
        return BasicDBObject::pointer_t(nullptr);
    }

    const childs_t &getChildrens() const
    {
        return m_childrens;
    }

    void updateChilds(const childs_t& childrens)
    {
        m_childrens = childrens;
    }

private:
    childs_t m_childrens;
};

#endif