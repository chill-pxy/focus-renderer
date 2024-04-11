set(TARGET_NAME Editor)

file(GLOB EDITOR_HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB EDITOR_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)
file(GLOB EDITOR_RESOURCE CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resource/Editor.rc)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${EDITOR_HEADERS} ${EDITOR_SOURCES} ${EDITOR_RESOURCE})

add_executable(${TARGET_NAME} ${EDITOR_HEADERS} ${EDITOR_SOURCES} ${EDITOR_RESOURCE})

target_sources(${TARGET_NAME} PRIVATE
        source/main.cpp
#        source/editor.cpp
#        source/editor_global_context.cpp
#        source/editor_input_manager.cpp
#        source/editor_scene_manager.cpp
#        source/editor_ui.cpp
#        source/editor_language.cpp
)

target_include_directories(${TARGET_NAME} PRIVATE
        include
)

add_compile_definitions("FOCUS_ROOT_DIR=${BINARY_ROOT_DIR}")

target_include_directories(
        ${TARGET_NAME}
        PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/stb>
)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 23 OUTPUT_NAME "Editor")
#set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Engine")

target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")

target_link_libraries(${TARGET_NAME} Runtime)

set(POST_BUILD_COMMANDS
        COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/resource" "${BINARY_ROOT_DIR}/resource"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/" "${BINARY_ROOT_DIR}"
        #COMMAND ${CMAKE_COMMAND} -E copy "${ENGINE_ROOT_DIR}/${DEPLOY_CONFIG_DIR}/${TARGET_NAME}.ini" "${BINARY_ROOT_DIR}"
        #COMMAND ${CMAKE_COMMAND} -E copy "${ENGINE_ROOT_DIR}/${DEVELOP_CONFIG_DIR}/${TARGET_NAME}.ini" "$<TARGET_FILE_DIR:${TARGET_NAME}>/"
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${BINARY_ROOT_DIR}/${ENGINE_ASSET_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${ENGINE_ROOT_DIR}/${ENGINE_ASSET_DIR}" "${BINARY_ROOT_DIR}/${ENGINE_ASSET_DIR}"
)

add_custom_command(TARGET ${TARGET_NAME} ${POST_BUILD_COMMANDS})