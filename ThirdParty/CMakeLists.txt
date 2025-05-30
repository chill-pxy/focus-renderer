set(third_party_folder "ThirdParty")

#set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if(NOT TARGET glm)
    option(BUILD_STATIC_LIBS "" ON)
    option(BUILD_TESTING "" OFF)
    option(GLM_TEST_ENABLE "" OFF)
    add_subdirectory(glm)
    set_target_properties(glm_static PROPERTIES FOLDER ${third_party_folder}/glm)
endif()

if(NOT TARGET glfw)
    option(GLFW_BUILD_EXAMPLES "" OFF)
    option(GLFW_BUILD_TESTS "" OFF)
    option(GLFW_BUILD_DOCS "" OFF)
    option(GLFW_INSTALL "" OFF)
    add_subdirectory(glfw)
    # set_target_properties(glfw PROPERTIES FOLDER ${third_party_folder}/glfw)
    # set_target_properties(update_mappings PROPERTIES FOLDER ${third_party_folder}/glfw)
endif()

# if(NOT TARGET spdlog)
#     option(SPDLOG_BUILD_EXAMPLE "" OFF)
#     option(SPDLOG_INSTALL "" OFF)
#     add_subdirectory(spdlog)
#     set_target_properties(spdlog PROPERTIES FOLDER ${third_party_folder}/spdlog)
# endif()

if(NOT TARGET stb)
    include(stb.cmake)
    set_target_properties(stb PROPERTIES FOLDER ${third_party_folder}/stb)
endif()

if(NOT TARGET tinyobjloader)
    add_subdirectory(tinyobjloader)
    set_target_properties(tinyobjloader PROPERTIES FOLDER ${third_party_folder}/tinyobjloader)
    set_target_properties(uninstall PROPERTIES FOLDER ${third_party_folder}/tinyobjloader)
endif()

if(NOT TARGET volk)
    add_subdirectory(volk)
endif()

if(NOT TARGET imgui)
    include(imgui.cmake)
    set_target_properties(imgui PROPERTIES FOLDER ${third_party_folder}/imgui)
endif()

# ffx
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ffx/ffx-api/lib/amd_fidelityfx_vk.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# drhi
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/drhi/drhi.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})