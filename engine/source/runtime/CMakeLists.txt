set(TARGET_NAME EATRuntime)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE HEADER_FILES "*.cpp")
file(GLOB_RECURSE SOURCE_FILES "*.ixx")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})

add_library(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 23)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Engine")

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")


target_include_directories(
        ${TARGET_NAME}
        PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/source>
        $<INSTALL_INTERFACE:include/${TARGET_NAME}-${PROJECT_VERSION}>
)


target_include_directories(
        ${TARGET_NAME}
        PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/source/runtime>
        $<INSTALL_INTERFACE:include/${TARGET_NAME}-${PROJECT_VERSION}>
)
