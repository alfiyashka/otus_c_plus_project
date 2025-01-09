#include "AuthManager.hpp"

bool AuthManager::registerUser(const std::string &username, const std::string &pwd)
{
    if (username.empty())
    {
        std::cerr << "Username is empty: cannot registrate" << std::endl;
        return false;
    }
    const auto &userIter = m_auths.find(username);
    if (userIter != m_auths.end())
    {
        std::cerr << "The user with username " << username << " is exists. Cannot registrate" << std::endl;
        return false;
    }
    m_auths.insert(auths_t::value_type(username, std::shared_ptr<Auth>(new Auth(username, pwd))));
    return true;
}

std::optional<std::shared_ptr<Auth>> AuthManager::login(const std::string &username, const std::string &pwd) const
{
    if (username.empty())
    {
        std::cerr << "Username is empty: cannot login" << std::endl;
        return std::nullopt;
    }
    const auto &userIter = m_auths.find(username);
    if (userIter == m_auths.end() || userIter != m_auths.end() && userIter->second->password() != pwd)
    {
        std::cerr << "Username or password is incorrect. Cannot login" << std::endl;
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
