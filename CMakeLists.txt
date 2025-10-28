cmake_minimum_required(VERSION 3.16)
project(bank_app)

set(CMAKE_CXX_STANDARD 17)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(bank_app banking.cpp)
target_link_libraries(bank_app soci_core soci_postgresql pq)

