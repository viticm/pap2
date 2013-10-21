@echo off
echo "Building RC files for ShaderLibrary"
"ShaderRCFileBuilder.exe" /dir:"../../KG3DEngine/shaders" /h:"resource.h" /rc:"Shaders.rc" /id:"ShaderID.h" /out:"Debug/shaderDirState.txt"
pause
