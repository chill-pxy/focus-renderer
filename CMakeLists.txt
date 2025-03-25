cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(FOCUS)

set(ENGINE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Sources)
set(PLATFORM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Platform)
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)

add_subdirectory(ThirdParty)
add_subdirectory(Sources)
add_subdirectory(Shaders)
#add_subdirectory(Example)

     

 