cmake_minimum_required(VERSION 3.28.3)
set(PROJECT_UNIT_TESTS_HEADER
    Utils.hpp
)

set(PROJECT_UNIT_TESTS 
    Utils.cpp
    NoSQLTest.cpp
    AuthTest.cpp
    AuthManagerTest.cpp
    DataComparatorTest.cpp
    IdGeneratorTest.cpp
    DBStoreTest.cpp
    InsertJobTest.cpp
    DeleteJobTest.cpp
    SelectJobTest.cpp
    UpdateJobTest.cpp
    CommitJobTest.cpp
    NoSQLDBTest.cpp
    SessionTest.cpp
    SessionManagerTest.cpp
    TransactionTest.cpp
    TransactionManagerTest.cpp
)

include_directories(../src/db/)
include_directories(../src/model/)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 23)
set_property(TARGET  PROPERTY CXX_STANDARD 23)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable (nosql_test ${PROJECT_UNIT_TESTS_HEADER} ${PROJECT_UNIT_TESTS} main.cpp)


target_link_libraries(nosql_test nosqllib ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} pthread)

