
set(ENGINE_ROOT_DIR "${EAT_ROOT_DIR}/engine")
set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/3rdparty")
set(ENGINE_ASSET_DIR "/asset")

#set(DEPLOY_CONFIG_DIR "configs/deployment")
#set(DEVELOP_CONFIG_DIR "configs/development")

# option(ENABLE_PHYSICS_DEBUG_RENDERER "Enable Physics Debug Renderer" OFF)

# if(NOT WIN32)
#   if(ENABLE_PHYSICS_DEBUG_RENDERER)
#     message(WARNING "Disable Physics Debug Renderer")
#     set(ENABLE_PHYSICS_DEBUG_RENDERER OFF CACHE BOOL "" FORCE)
#   endif()
# endif()

# if(ENABLE_PHYSICS_DEBUG_RENDERER)
#   set(JOLT_ASSET_DIR "/jolt-asset")
# endif()

# set(SHADER_COMPILE_TARGET SmoothShaderCompile)
#add_subdirectory(shader)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/MP")
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SmoothEditor)
endif()

#
#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)

#add_subdirectory(3rdparty)
add_subdirectory(source/runtime)
add_subdirectory(source/editor)
