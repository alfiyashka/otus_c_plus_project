#ifndef _DATATYPE_HPP_
#define _DATATYPE_HPP_

#include <tuple>
#include <memory>

/**
 * supported datatype definition
 */
enum class Datatype
{
    UNDEFINED,
    BYTE,
    SMALLINT,
    INT,
    BIGINT,
    FLOAT,
    DOUBLE,
    STRING,
    COMPOSITE
    
};

using Data_t = std::shared_ptr<void>;  // stored data type

using DataWithType = std::tuple<Datatype, Data_t>; // defined type of data with its type definition



#endif