#ifndef _AUTH_MANAGER_HPP_
#define _AUTH_MANAGER_HPP_

#include <map>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <optional>

#include "AuthInfo.hpp"


class AuthManager
{
public:
    using auths_t = std::map<std::string, std::shared_ptr<Auth>>;

    bool registerUser(const std::string& username, const std::string& pwd);

    std::optional<std::shared_ptr<Auth>> login(const std::string& username, const std::string& pwd) const;


    void removeUser(const std::string& username);

    auto getUser(const std::string& username) const;


private:
    auths_t m_auths;

};



#endif