@echo off
echo Glich Unit Tests Release
..\build\msvc\Release\glcunit.exe
echo.
cd glcscripts
..\..\build\msvc\Release\glctest.exe suite
echo.
pause