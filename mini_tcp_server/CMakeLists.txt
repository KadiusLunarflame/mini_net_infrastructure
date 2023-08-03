cmake_minimum_required(VERSION 3.24)
project(mini_tcp_server)

set(CMAKE_CXX_STANDARD 20)


set(BOOST_ROOT "/home/kadius/boost_1_82_0")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.82.0)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(mini_server main.cpp)
    target_link_libraries(mini_server ${Boost_LIBRARIES})
endif()


add_executable(mini_tcp_server main.cpp mini_server.hpp)
