@echo off
rem THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
rem ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
rem THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
rem PARTICULAR PURPOSE.
rem
rem Copyright (c) Microsoft Corporation. All rights reserved.

setlocal
set error=0

call :CompileShader FixedFunction vs VS
call :CompileShader FixedFunction ps PS

call :CompileShader FixedFunctionLighting vs VS
call :CompileShader FixedFunctionLighting ps PS

call :CompileShader FixedFunctionTexture vs VS
call :CompileShader FixedFunctionTexture ps PS

call :CompileShader FixedFunctionTextureLighting vs VS
call :CompileShader FixedFunctionTextureLighting ps PS

call :CompileShader StandardVboLayout vs VS

call :CompileShader StandardVboLayout vs PositionVS
call :CompileShader StandardVboLayout vs PositionNormalVS
call :CompileShader StandardVboLayout vs PositionTextureVS
call :CompileShader StandardVboLayout vs PositionColorVS
call :CompileShader StandardVboLayout vs PositionNormalTextureVS
call :CompileShader StandardVboLayout vs PositionNormalColorVS
call :CompileShader StandardVboLayout vs PositionColorTextureVS
call :CompileShader StandardVboLayout vs PositionNormalColorVS
call :CompileShader StandardVboLayout vs PositionNormalColorTextureVS

call :CompileShader StandardVboLayout vs PositionLightVS
call :CompileShader StandardVboLayout vs PositionNormalLightVS
call :CompileShader StandardVboLayout vs PositionTextureLightVS
call :CompileShader StandardVboLayout vs PositionColorLightVS
call :CompileShader StandardVboLayout vs PositionNormalTextureLightVS
call :CompileShader StandardVboLayout vs PositionNormalColorLightVS
call :CompileShader StandardVboLayout vs PositionColorTextureLightVS
call :CompileShader StandardVboLayout vs PositionNormalColorLightVS
call :CompileShader StandardVboLayout vs PositionNormalColorTextureLightVS

echo.

if %error% == 0 (
    echo Shaders compiled ok
) else (
    echo There were shader compilation errors!
)

endlocal
exit /b

:CompileShader
set fxc=fxc /nologo %1.fx /T%2_4_0_level_9_1 /Zpc /Qstrip_reflect /Qstrip_debug /E%3 /Fhcompiled\Compiled%1%3.inc /Vn%1%3
echo.
echo %fxc%
%fxc% || set error=1
exit /b
