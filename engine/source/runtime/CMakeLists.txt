set(TARGET_NAME Runtime)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE HEADER_FILES "*.ixx")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})

add_library(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 23)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Engine")

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")


set(JSON_INCLUDE ${THIRD_PARTY_DIR}/json11)

add_library(json11 ${JSON_INCLUDE}/json11.cpp)
set_target_properties(json11 PROPERTIES FOLDER "ThirdParty/json11")

target_link_libraries(
        ${TARGET_NAME} PUBLIC
        glfw
        glm
        imgui
        stb
)

target_link_libraries(${TARGET_NAME} PRIVATE $<BUILD_INTERFACE:json11>)

target_link_libraries(${TARGET_NAME} PUBLIC ${vulkan_lib})

target_link_libraries(
        ${TARGET_NAME} PRIVATE
        tinyobjloader
)

target_include_directories(
        ${TARGET_NAME}
        PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/source>
        $<INSTALL_INTERFACE:include/${TARGET_NAME}-${PROJECT_VERSION}>
)

target_include_directories(
        ${TARGET_NAME}
        PUBLIC $<BUILD_INTERFACE:${JSON_INCLUDE}>
)

target_include_directories(
        ${TARGET_NAME}
        PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/source/runtime>
        $<INSTALL_INTERFACE:include/${TARGET_NAME}-${PROJECT_VERSION}>
)

target_include_directories(
        ${TARGET_NAME}
        PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/source/runtime/function/render/include>
        $<INSTALL_INTERFACE:include/${TARGET_NAME}-${PROJECT_VERSION}>
)

target_include_directories(
        ${TARGET_NAME}
        PUBLIC $<BUILD_INTERFACE:${vulkan_include}>)

string(TOLOWER ${TARGET_NAME}/version.h VERSION_HEADER_LOCATION)

##precompile
#set gloval vari used by precompile
set(Focus_RUNTIME_HEADS "${HEADER_FILES}" PARENT_SCOPE)
