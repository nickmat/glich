@echo off
echo Glich Unit Tests Debug
..\build\msvc\Debug\glcunit.exe
echo.
cd glcscripts
..\..\build\msvc\Debug\glctest.exe suite
echo.
pause
