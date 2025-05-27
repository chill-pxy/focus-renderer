set(TARGET_NAME engine)

project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE INTERFACE_FILES "*.ixx")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")
file(GLOB_RECURSE HEADER_FILES "*.h")

add_compile_definitions(FOCUS_EXPORT)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES} ${INTERFACE_FILES})

include_directories(${THIRD_PARTY_DIR}/ktx/include)
include_directories(${THIRD_PARTY_DIR}/ktx/other_include)
set(KTX_DIR ${THIRD_PARTY_DIR}/ktx)
set(KTX_SOURCES
    ${KTX_DIR}/lib/texture.c
    ${KTX_DIR}/lib/hashlist.c
    ${KTX_DIR}/lib/checkheader.c
    ${KTX_DIR}/lib/swap.c
    ${KTX_DIR}/lib/memstream.c
    ${KTX_DIR}/lib/filestream.c)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(${TARGET_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES} ${INTERFACE_FILES} ${KTX_SOURCES})

set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "engine")

#---------------------------------#
#--------- Use raw Vulkan --------#
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
    tinyobjloader
    glm
    stb
    imgui
)

# ---------------drhi------------------
target_link_libraries(${TARGET_NAME} PRIVATE
    ${THIRD_PARTY_DIR}/drhi/drhi.lib
)

include_directories(${THIRD_PARTY_DIR}/drhi/Include)
#--------------------------------------

#---------------ffx api----------------
target_link_libraries(${TARGET_NAME} PRIVATE
    ${THIRD_PARTY_DIR}/ffx/ffx-api/lib/amd_fidelityfx_vk.lib
)

include_directories(${THIRD_PARTY_DIR}/ffx/ffx-api/include)
#--------------------------------------

# imgui
include_directories(${THIRD_PARTY_DIR}/imgui/backends)

