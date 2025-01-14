#ifndef _AUTH_MANAGER_HPP_
#define _AUTH_MANAGER_HPP_

#include <map>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <optional>

#include "AuthInfo.hpp"

/**
 * Stores all users info
 */
class AuthManager
{
public:
    using auths_t = std::map<std::string, std::shared_ptr<Auth>>;

    /**
     * adds a new user
     * @param username - username
     * @param pwd - password
     * @returns if a new user added successfully, else false
     */
    bool registerUser(const std::string& username, const std::string& pwd);

    /**
     * logins by user login data
     * @param username - username
     * @param pwd - password
     * @returns user's login info
     */
    std::optional<IAuth::pointer_t> login(const std::string& username, const std::string& pwd) const;

    /**
     * removes by username login data
     * @param username - username
     */
    void removeUser(const std::string& username);

private:
    /**
     * gets user login data by username
     * @param username - username
     * @return login data
     */
    auto getUser(const std::string& username) const;

    auths_t m_auths;

};



#endif