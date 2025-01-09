#ifndef _DB_COMPARATOR_HPP_
#define _DB_COMPARATOR_HPP_

#include <string>
#include <tuple>
#include <memory>
#include "../model/Datatype.hpp"
#include <iostream>

class DataComparator
{
private:
    /* data */
public:
    DataComparator(/* args */) {}
    ~DataComparator(){}


    static bool compare(const std::tuple<Datatype, std::shared_ptr<void>>& r,
        const std::tuple<Datatype, std::shared_ptr<void>>& l);
};




#endif