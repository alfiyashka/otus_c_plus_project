#ifndef _AUTH_INFO_HPP_
#define _AUTH_INFO_HPP_

#include <string>

class IAuth
{
public:
  virtual const std::string& username() const = 0;
  virtual ~IAuth() {}
};

/**
 * Store authorization manager 
 */
class Auth: public IAuth
{
    const std::string m_username; // user name
    const std::string m_password; // user password
public:
    Auth( const std::string& username, const std::string& password)
    : m_username(username),
      m_password(password)
    {

    }

    const std::string& username() const override { return m_username; }
    const std::string& password() const { return m_password; }
    
};

#endif