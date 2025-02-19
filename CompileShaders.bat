for /R ./Shaders %%f in (*.vertex) do (
    glslc -c -fshader-stage=vert "%%f" -o "%%~dpf%%~nfVertex.spv"
)

for /R ./Shaders %%f in (*.fragment) do (
    glslc -c -fshader-stage=frag "%%f" -o "%%~dpf%%~nfFragment.spv"
)

pause