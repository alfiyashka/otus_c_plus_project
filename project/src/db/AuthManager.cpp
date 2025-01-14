#include "AuthManager.hpp"
#include "yelloger.h"

bool AuthManager::registerUser(const std::string &username, const std::string &pwd)
{
    if (username.empty())
    {
        Yellog::Error("Username is empty: cannot registrate");
        return false;
    }
    const auto &userIter = m_auths.find(username);
    if (userIter != m_auths.end())
    {
        Yellog::Error("The user with username '%s' is exists. Cannot registrate", username.c_str());
        return false;
    }
    m_auths.insert(auths_t::value_type(username, std::shared_ptr<Auth>(new Auth(username, pwd))));
    return true;
}

std::optional<IAuth::pointer_t> AuthManager::login(const std::string &username, const std::string &pwd) const
{
    if (username.empty())
    {
        Yellog::Error("Username is empty: cannot login");
        return std::nullopt;
    }
    const auto &userIter = m_auths.find(username);
    if (userIter == m_auths.end() || userIter != m_auths.end() && userIter->second->password() != pwd)
    {
        Yellog::Error("Username or password is incorrect. Cannot login");
        return std::nullopt;
    }
    return std::optional(userIter->second);
}

void AuthManager::removeUser(const std::string &username)
{
    m_auths.erase(username);
}

auto AuthManager::getUser(const std::string &username) const
{
    const auto &userIter = m_auths.find(username);
    if (userIter != m_auths.end())
    {
        userIter->second;
    }
    else
    {
        std::string error = "There is no user with username " + (username);
        throw std::runtime_error(error.c_str());
    }
}
