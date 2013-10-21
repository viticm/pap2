cd ..\
.\autinf.exe
cd build\
C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727\MSBuild.exe .\msbuild\build.sln /t:Rebuild  /p:OutputPath=%CD%\output /p:Configuration=Release
echo "Compilazione completata"


