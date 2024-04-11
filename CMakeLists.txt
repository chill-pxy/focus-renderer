cmake_minimum_required(VERSION 3.20)

project(FOCUS VERSION 0.1.0)

SET(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

include(CMakeDependentOption)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(
            FATAL_ERROR
            "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

set(FOCUS_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_INSTALL_PREFIX "${FOCUS_ROOT_DIR}/bin")
set(BINARY_ROOT_DIR "${CMAKE_INSTALL_PREFIX}/")

SET(EXECUTABLE_OUTPUT_PATH ${FOCUS_ROOT_DIR})
SET(LIBRARY_OUTPUT_PATH ${BINARY_ROOT_DIR})

add_subdirectory(engine)