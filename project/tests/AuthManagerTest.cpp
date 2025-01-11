#include "../src/db/AuthManager.hpp"
#include <gtest/gtest.h>
#include <string>

TEST(auth_manager_test_case, add_new_user)
{
    AuthManager manager;
    const std::string name{"Username"};
    const std::string pwd{"Pwd"};
    EXPECT_TRUE(manager.registerUser(name, pwd));
}

TEST(auth_manager_test_case, re_add_same_user)
{
    AuthManager manager;
    const std::string name{"Username"};
    const std::string pwd{"Pwd"};
    EXPECT_TRUE(manager.registerUser(name, pwd));
    EXPECT_FALSE(manager.registerUser(name, pwd));
}

TEST(auth_manager_test_case, login_test_no_exist)
{
    AuthManager manager;
    const std::string name{"Username"};
    const std::string pwd{"Pwd"};

    const auto res = manager.login(name, pwd);
    EXPECT_FALSE(res.has_value());
}

TEST(auth_manager_test_case, login_test_exist)
{
    AuthManager manager;
    const std::string name{"Username"};
    const std::string pwd{"Pwd"};
    EXPECT_TRUE(manager.registerUser(name, pwd));

    const auto res = manager.login(name, pwd);
    EXPECT_TRUE(res.has_value());
    EXPECT_EQ(res.value()->username(), name);
}

TEST(auth_manager_test_case, remove_user_test)
{
    AuthManager manager;
    const std::string name{"Username"};
    const std::string pwd{"Pwd"};
    EXPECT_TRUE(manager.registerUser(name, pwd));

    manager.removeUser(name);

    const auto res = manager.login(name, pwd);
    EXPECT_FALSE(res.has_value());
}