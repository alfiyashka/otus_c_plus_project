#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include <map>
#include <list>
#include <memory>

#include "../model/BasicDBObject.hpp"

enum class JOB_TYPE
{
    INSERT,
    UPDATE,
    DELETE,
    SELECT,
    COMMIT
};

using tempDbStoreType = std::map<std::size_t, std::pair<JOB_TYPE, std::list<std::shared_ptr<BasicDBObject>>>>;

#endif