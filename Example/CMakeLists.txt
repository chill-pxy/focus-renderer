cmake_minimum_required(VERSION 3.25)
project(simple)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(simple simple/main.cpp "Engine/Platform/RenderHardwareInterface/Context.h")

target_link_libraries(simple engine)