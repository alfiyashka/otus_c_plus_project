#include "DataComparator.hpp"

bool DataComparator::compare(const std::tuple<Datatype, std::shared_ptr<void>> &r,
                             const std::tuple<Datatype, std::shared_ptr<void>> &l)
{
    if (std::get<0>(r) != std::get<0>(l))
    {
        return false;
    }
    const Datatype type = std::get<0>(r);
    switch (type)
    {
    case Datatype::BIGINT:
        return (*(static_cast<uint64_t *>(std::get<1>(r).get()))) == (*(static_cast<uint64_t *>(std::get<1>(l).get())));
    case Datatype::BIT:
        return (*(static_cast<uint8_t *>(std::get<1>(r).get()))) == (*(static_cast<uint8_t *>(std::get<1>(l).get())));
    case Datatype::DOUBLE:
        return (*(static_cast<double *>(std::get<1>(r).get()))) == (*(static_cast<double *>(std::get<1>(l).get())));
    case Datatype::FLOAT:
        return (*(static_cast<float *>(std::get<1>(r).get()))) == (*(static_cast<float *>(std::get<1>(l).get())));
    case Datatype::INT:
        return (*(static_cast<uint32_t *>(std::get<1>(r).get()))) == (*(static_cast<uint32_t *>(std::get<1>(l).get())));
    case Datatype::SMALLINT:
        return (*(static_cast<uint16_t *>(std::get<1>(r).get()))) == (*(static_cast<uint16_t *>(std::get<1>(l).get())));
    case Datatype::STRING:
        return (*(static_cast<std::string *>(std::get<1>(r).get()))) == (*(static_cast<std::string *>(std::get<1>(l).get())));
    default:
        std::cerr << "Cannot compare data, because types undefined" << std::endl;
        return false;
    }

    return false;
}
