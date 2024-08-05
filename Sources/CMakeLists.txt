set(TARGET_NAME engine)

project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE INTERFACE_FILES "*.ixx")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")
file(GLOB_RECURSE HEADER_FILES "*.h")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES} ${INTERFACE_FILES})

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${INTERFACE_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "engine")

#---------------------------------#
#--------- 调用原生Vulkan --------#
#---------------------------------#
#find_package(Vulkan REQUIRED)
#if(Vulkan_FOUND)
#    message(STATUS "Vulkan include dir:${Vulkan_INCLUDE_DIR}")
#    message(STATUS "Vulkan library dir:${Vulkan_LIBRARY}")
#endif()

#include_directories(${Vulkan_INCLUDE_DIRS})
#---------------------------------#
#---------------------------------#
#---------------------------------# 

target_link_libraries(${TARGET_NAME} PUBLIC
    glfw
    volk
)

target_link_libraries(${TARGET_NAME} PRIVATE
    ${THIRD_PARTY_DIR}/drhi/drhi.lib
)

include_directories(${THIRD_PARTY_DIR}/drhi/Include)
