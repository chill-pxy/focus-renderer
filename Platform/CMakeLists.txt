set(TARGET_NAME platform)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE INTERFACE_FILES "*.ixx")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")
file(GLOB_RECURSE HEADER_FILES "*.h")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES} ${INTERFACE_FILES})
add_library(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${INTERFACE_FILES})

target_link_libraries(${TARGET_NAME} PUBLIC
    glfw
    tinyobjloader
    glm
    stb
    volk
)


#---------------------------------#
#------------配置文件-------------#
#---------------------------------#
set(content " ")
set(codeDefine "undefine")
file(READ ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Config/config.txt content)
if(content STREQUAL "vk")
    set(codeDefine "#define VULKAN_IMPLEMENTATION")
endif()

if(content STREQUAL "dx12")
    set(codeDefine "#define DIRECT3D12_IMPLEMENTATION")
endif()

if(content STREQUAL "undefine")
    message(FATAL_ERROR "没有定义图形API")
endif()

file(WRITE ${PLATFORM_INCLUDE_DIR}/Interface/InterfaceConfig.h "${codeDefine}" )
add_definitions(-DINCLUDE_GENERATED_HEADER)