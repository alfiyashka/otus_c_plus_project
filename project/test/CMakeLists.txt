cmake_minimum_required(VERSION 3.28.3)

set(PROJECT_UNIT_TESTS 
NoSQLTest.cpp)

include_directories(..)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 23)

add_executable (ProjectTest_run ${PROJECT_UNIT_TESTS})

target_link_libraries(ProjectTest_run ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} pthread)


add_test(nosql ProjectTest_run)