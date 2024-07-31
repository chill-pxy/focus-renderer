set(TARGET_NAME engine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#file(GLOB_RECURSE SOURCE_FILES "*.ixx")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")
file(GLOB_RECURSE HEADER_FILES "*.h")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})
add_library(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES})

find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
    message(STATUS "Vulkan include dir:${Vulkan_INCLUDE_DIR}")
    message(STATUS "Vulkan library dir:${Vulkan_LIBRARY}")
endif()

target_link_libraries(${TARGET_NAME} PUBLIC
    ${Vulkan_LIBRARY}
    glfw
    tinyobjloader
    glm
    stb
    volk
)

include_directories(${Vulkan_INCLUDE_DIRS})