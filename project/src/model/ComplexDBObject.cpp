#include "ComplexDBObject.hpp"

void ComplexDBObject::addChild(BasicDBObject::pointer_t child)
{
    m_childrens.insert(childs_t::value_type(child->id(), child));
    child->setParent(shared_from_this());
}

void ComplexDBObject::removeChild(BasicDBObject::pointer_t child)
{
    m_childrens.erase(child->id());
}

void ComplexDBObject::removeChild(std::size_t id)
{
    const auto find = m_childrens.find(id);
    if (find == m_childrens.end())
    {
        return;
    }     
    m_childrens.erase(id);
}

BasicDBAbstractObject::pointer_t ComplexDBObject::getChild(std::size_t id)
{
    const auto found = m_childrens.find(id);
    if (found != m_childrens.end())
    {
        return found->second;
    }
    return BasicDBObject::pointer_t(nullptr);
}

const BasicDBAbstractObject::childs_t & ComplexDBObject::getChildrens() const
{
    return m_childrens;
}

void ComplexDBObject::updateChilds(const childs_t &childrens)
{
    m_childrens = childrens;
}

bool ComplexDBObject::operator==(const ComplexDBObject &obj) const 
{
    if (obj.type() != Datatype::COMPOSITE)
    {
        return *dynamic_cast<const BasicDBAbstractObject *>(this)
             == *dynamic_cast<const BasicDBAbstractObject *>(&obj);
    }
    else
    {
        if (this->getChildrens().size() != obj.getChildrens().size())
        {
            return false;
        }
        for (auto child : this->getChildrens())
        {
            auto findValueChild = obj.getChildrens().find(child.first);
            if (findValueChild == obj.getChildrens().end())
            {
                return false;
            }
            BasicDBAbstractObject::pointer_t valueChild = findValueChild->second;
            if (valueChild->type() != Datatype::COMPOSITE && child.second->type() != Datatype::COMPOSITE)
            {
                return *dynamic_cast<BasicDBAbstractObject *>(child.second.get()) == *dynamic_cast<BasicDBAbstractObject *>(valueChild.get());
            }
            else
            {
                if (valueChild->type() == Datatype::COMPOSITE)
                {
                    return dynamic_cast<const ComplexDBObject *>(valueChild.get()) == dynamic_cast<const BasicDBAbstractObject *>(child.second.get());
                }
                else
                {
                    return dynamic_cast<const ComplexDBObject *>(child.second.get()) == dynamic_cast<const BasicDBAbstractObject *>(valueChild.get());
                }
            }
        }
    }

    return false;
}