#ifndef _COMPLEX_DB_OBJECT_HPP_
#define _COMPLEX_DB_OBJECT_HPP_

#include "BasicDBObject.hpp"

#include <string>
#include <vector>
#include <memory>
#include <map>

class ComplexDBObject : public BasicDBObject
{

public:
    ComplexDBObject(const std::string &name,
                    std::shared_ptr<void> data)
         : BasicDBObject(Datatype::COMPOSITE, name, data)
    {
    }

    ComplexDBObject(const ComplexDBObject& obj)
         : BasicDBObject(obj)
    {
    }

    ComplexDBObject &operator=(const ComplexDBObject &obj) = delete;
    ComplexDBObject(ComplexDBObject &&obj) = delete;
    ComplexDBObject &operator=(ComplexDBObject &&obj) = delete;

    using childs_t = std::map<std::size_t, std::shared_ptr<BasicDBObject>>;

    void addChild(std::shared_ptr<BasicDBObject> child)
    {
        m_childrens.insert(childs_t::value_type(child->id(), child));
    }

    void removeChild(std::shared_ptr<BasicDBObject> child)
    {
        m_childrens.erase(child->id());
    }

    void removeChild(std::size_t id)
    {
        m_childrens.erase(id);
    }

    std::shared_ptr<BasicDBObject> getChild(std::size_t id)
    {
        const auto found = m_childrens.find(id);
        if (found != m_childrens.end())
        {
            return found->second;
        }
        return std::shared_ptr<BasicDBObject>(nullptr);
    }

    const childs_t &getChildrens() const
    {
        return m_childrens;
    }

private:
    childs_t m_childrens;
};

#endif