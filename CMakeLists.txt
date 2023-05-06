cmake_minimum_required(VERSION 3.0.0)
project(LoadBalancerAgent VERSION 0.1.0)


enable_testing()

add_executable(LoadBalancerAgent main.cpp src/LoadBalancerAgent.cpp src/Config.cpp)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

target_link_libraries(LoadBalancerAgent PRIVATE Threads::Threads spdlog::spdlog nlohmann_json::nlohmann_json)
target_include_directories(LoadBalancerAgent PUBLIC ./)

# add optimization flags
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(LoadBalancerAgent PRIVATE -O3)
endif()