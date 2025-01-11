#ifndef _DB_COMPARATOR_HPP_
#define _DB_COMPARATOR_HPP_

#include <string>
#include <tuple>
#include <memory>
#include "../model/Datatype.hpp"
#include <iostream>

/**
 * helps to compare data
 */
class DataComparator
{
private:
public:
    DataComparator() {}
    ~DataComparator(){}

    /**
     * defines that 2 plain data are equal with each other
     * @param r - first data definition
     * @param l - second data definition
     */
    static bool isPlainDataEqual(const DataWithType& r, const DataWithType& l);

    
};




#endif