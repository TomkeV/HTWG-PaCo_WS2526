@echo off

PATH=%PATH%;.\lib\OpenCL-SDK-v2025.07.23-Win-x64\bin;.\lib\SDL2-2.32.10\lib\x64

REM Serial depth loop
for %%D in (5 7 9 11 13 15 17 19 21) do (
    for /L %%I in (1,1,7) do (
        .\x64\Release\PythagorasTree.exe --show false --serial-depth %%D --openmp-depth 0 --opencl-depth 0
        if errorlevel 1 exit /b 1
    )
)

REM OpenMP depth loop
for %%D in (5 7 9 11 13 15 17 19 21) do (
    for /L %%I in (1,1,7) do (
        .\x64\Release\PythagorasTree.exe --show false --serial-depth 0 --openmp-depth %%D --opencl-depth 0
        if errorlevel 1 exit /b 1
    )
)

REM OpenCL depth loop
for %%D in (5 7 9 11 13 15 17 19 21) do (
    for /L %%I in (1,1,7) do (
        .\x64\Release\PythagorasTree.exe --show false --serial-depth 0 --openmp-depth 0 --opencl-depth %%D
        if errorlevel 1 exit /b 1
    )
)

endlocal
