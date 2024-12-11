@echo off
setlocal enabledelayedexpansion

set /a total_lines=0

for %%f in (*.cpp *.h) do (
    set /a file_lines=0
    for /f %%l in ('type "%%f" ^| find /c /v ""') do (
        set /a file_lines=%%l
    )
    set /a total_lines+=file_lines
    echo %%f: !file_lines! lines
)

echo Total lines in .cpp and .h files: %total_lines%
endlocal
@REM pause
