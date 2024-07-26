cmake_minimum_required(VERSION 3.20)

project(cpp-mysql-connectionPool
    LANGUAGES CXX
    VERSION 0.0.1
    DESCRIPTION "A simple connection pool for cpp-mysql"
    HOMEPAGE_URL "https://github.com/breeze-wink/cpp-mysql-connectionPool"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_PATH ${PROJECT_SOURCE_DIR}/lib)

add_executable(test main.cpp connection.cpp)

target_link_libraries(test PUBLIC mysqlclient)
