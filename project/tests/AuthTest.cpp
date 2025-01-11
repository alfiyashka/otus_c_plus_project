#include "../src/db/AuthInfo.hpp"
#include <gtest/gtest.h>
#include <string>

TEST(auth_test_case, auth_test)
{
    const std::string name{"Username"};
    const std::string pwd{"Pwd"};
    const Auth auth(name, pwd);
    EXPECT_EQ(auth.username(), name);
    EXPECT_EQ(auth.password(), pwd);
}