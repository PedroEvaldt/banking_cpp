cmake_minimum_required(VERSION 3.16)
project(bank_app CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES
    src/account.cpp
    src/bank_system.cpp
    src/db.cpp
    src/main.cpp
    src/ui_utils.cpp)

# Threads para std::thread
find_package(Threads REQUIRED)

# PostgreSQL (para headers e libs)
find_package(PostgreSQL REQUIRED)

# Diretórios manuais do SOCI (porque está em /usr/local)
link_directories(/usr/local/lib)
include_directories(/usr/local/include)

add_executable(bank_app ${SOURCES})

target_include_directories(bank_app PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${PostgreSQL_INCLUDE_DIRS}
    /usr/local/include
)

target_link_libraries(bank_app PRIVATE
    Threads::Threads
    ${PostgreSQL_LIBRARIES}
    soci_core
    soci_postgresql
)
