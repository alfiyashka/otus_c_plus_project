cmake_minimum_required(VERSION 3.28.3)

set(PATCH_VERSION "1" CACHE INTERNAL "PATCH_VERSION" )
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(nosql VERSION ${PROJECT_VERSION} LANGUAGES C CXX)

configure_file(version.h.in version.h)

set ( PROJECT_INCLUDE
   src/db/DBStore.hpp
   src/db/IdGenerator.hpp
   src/db/AuthInfo.hpp
   src/db/AuthManager.hpp
   src/db/DataComparator.hpp
   
   src/db/Job.hpp
   src/db/NoSQLDB.hpp
   src/db/Session.hpp
   src/db/SessionManager.hpp
   src/db/Transaction.hpp
   src/db/TransactionManager.hpp
   src/model/BasicDBObject.hpp
   src/model/ComplexDBObject.hpp
   src/model/Datatype.hpp
   src/model/BasicDBInputObject.hpp
)

set ( PROJECT_SOURCES 
   src/db/DBStore.cpp
   src/db/IdGenerator.cpp
   src/db/AuthManager.cpp
   src/db/SessionManager.cpp
   src/db/DataComparator.cpp
   src/db/Transaction.cpp
   src/db/TransactionManager.cpp
   src/db/Job.cpp
   src/db/NoSQLDB.cpp
   src/db/Session.cpp
   src/model/BasicDBObject.cpp
   src/model/ComplexDBObject.cpp
   
   

)

find_package (Threads)

include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inlude)
 
add_executable(nosql ${PROJECT_INCLUDE} ${PROJECT_SOURCES} 
   main.cpp)

add_library(nosqllib STATIC ${PROJECT_INCLUDE} ${PROJECT_SOURCES})

target_link_libraries (nosql 
   Threads::Threads
)

set_property(TARGET  PROPERTY CXX_STANDARD 23)

install(TARGETS nosql DESTINATION . )

enable_testing()
add_subdirectory(tests)

include("functions.cmake")
generate_packages(${PROJECT_NAME})










