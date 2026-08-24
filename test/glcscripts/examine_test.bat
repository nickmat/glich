rem Examine test output

@echo off

python split_test_output.py
if %errorlevel% neq 0 (
    echo Aborted.
    pause
    exit /b 1
)

git diff --no-index expected.txt actual.txt 

pause
