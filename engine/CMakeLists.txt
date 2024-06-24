set(TARGET_NAME engine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCE_FILES "*.ixx")
#file(GLOB_RECURSE SOURCE_FILES "*.cpp")

add_library(${TARGET_NAME} ${SOURCE_FILES})

target_link_libraries(${TARGET_NAME} PUBLIC
    ${vulkan_lib}
    glfw
)

target_include_directories(${TARGET_NAME} PUBLIC 
    $<BUILD_INTERFACE:${vulkan_include}>
)