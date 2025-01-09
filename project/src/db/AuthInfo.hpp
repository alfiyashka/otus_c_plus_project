#ifndef _AUTH_INFO_HPP_
#define _AUTH_INFO_HPP_

#include <string>


class Auth
{
    const std::string m_username;
    const std::string m_password;
public:
    Auth( const std::string& username, const std::string& password)
    : m_username(username),
      m_password(password)
    {

    }

    const std::string& username() const { return m_username; }
    const std::string& password() const { return m_password; }
    
};

#endif