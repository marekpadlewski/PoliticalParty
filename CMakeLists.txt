cmake_minimum_required(VERSION 3.10)
project(partiapolityczna)

set(CMAKE_CXX_STANDARD 11)

include_directories(libpqxx_build/include)
include_directories(libpqxx/include)

add_executable(partiapolityczna main.cpp json.hpp InputReader.cpp InputReader.h)

find_library(PQXX_LIBRARY pqxx libpqxx_build/src)
target_link_libraries(partiapolityczna LINK_PUBLIC ${PQXX_LIBRARY})