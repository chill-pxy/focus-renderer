execute_process(
    COMMAND cmd /c glslc -c -fshader-stage=vert ./
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE OUTPUT
    ERROR_VARIABLE ERROR
    RESULT_VARIABLE RESULT
)

message(STATUS "output")