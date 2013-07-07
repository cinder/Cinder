@echo off
rem 
rem Portions Copyright (C) 2013 Cinder Project (http://libcinder.org)
rem
rem THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
rem ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
rem THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
rem PARTICULAR PURPOSE.
rem
rem Copyright (c) Microsoft Corporation. All rights reserved.
rem

setlocal ENABLEDELAYEDEXPANSION
set error=0

if "%1"=="" (goto all)
if %1==dx9 (goto dx9)
if %1==dx10 (goto dx10)
if %1==dx11 (goto dx11)
if %1==all (goto all)
goto done

:dx9
	echo Compiling for Feature Level 9.1
	call :CompileShaderForTarget _4_0_level_9_1
	echo Compiling for Feature Level 9.3
	call :CompileShaderForTarget _4_0_level_9_3
	goto done

:dx10
	echo Compiling for Feature Level 10.1
	call :CompileShaderForTarget _4_1
	goto done

:dx11
	echo Compiling for Feature Level 11.0 and 11.1
	call :CompileShaderForTarget _5_0
	goto done
	
:all	
	echo Compiling for All Supported Feature Levels
	call :CompileShaderForTarget _4_0_level_9_1
	call :CompileShaderForTarget _4_1
	call :CompileShaderForTarget _5_0
	goto done

:done
	
rem for %%a in ( _4_0_level_9_1, _4_1, _5_0 ) do (
rem 	set outdir=compiled%%a
rem 	if not exist !outdir! (
rem 		mkdir !outdir!
rem 	)
rem 	call :CompileShaderForTarget %%a
rem )

rem call :CompileShader FixedFunction vs VS %target%
rem call :CompileShader FixedFunction ps PS  %target%

rem call :CompileShader FixedFunctionLighting vs VS
rem call :CompileShader FixedFunctionLighting ps PS

rem call :CompileShader FixedFunctionTexture vs VS
rem call :CompileShader FixedFunctionTexture ps PS

rem call :CompileShader FixedFunctionTextureLighting vs VS
rem call :CompileShader FixedFunctionTextureLighting ps PS

rem call :CompileShader StandardVboLayout vs VS

rem call :CompileShader StandardVboLayout vs PositionVS
rem call :CompileShader StandardVboLayout vs PositionNormalVS
rem call :CompileShader StandardVboLayout vs PositionTextureVS
rem call :CompileShader StandardVboLayout vs PositionColorVS
rem call :CompileShader StandardVboLayout vs PositionNormalTextureVS
rem call :CompileShader StandardVboLayout vs PositionNormalColorVS
rem call :CompileShader StandardVboLayout vs PositionColorTextureVS
rem call :CompileShader StandardVboLayout vs PositionNormalColorVS
rem call :CompileShader StandardVboLayout vs PositionNormalColorTextureVS

rem call :CompileShader StandardVboLayout vs PositionLightVS
rem call :CompileShader StandardVboLayout vs PositionNormalLightVS
rem call :CompileShader StandardVboLayout vs PositionTextureLightVS
rem call :CompileShader StandardVboLayout vs PositionColorLightVS
rem call :CompileShader StandardVboLayout vs PositionNormalTextureLightVS
rem call :CompileShader StandardVboLayout vs PositionNormalColorLightVS
rem call :CompileShader StandardVboLayout vs PositionColorTextureLightVS
rem call :CompileShader StandardVboLayout vs PositionNormalColorLightVS
rem call :CompileShader StandardVboLayout vs PositionNormalColorTextureLightVS

echo.

if %error% == 0 (
    echo Shaders compiled ok
) else (
    echo There were shader compilation errors!
)

endlocal
exit /b

:CompileShaderForTarget
set outdir=compiled%1%
if not exist !outdir! (
	mkdir !outdir!
)

call :CompileShader FixedFunction vs VS %1%
call :CompileShader FixedFunction ps PS  %1%

call :CompileShader FixedFunctionLighting vs VS %1%
call :CompileShader FixedFunctionLighting ps PS %1%

call :CompileShader FixedFunctionTexture vs VS %1%
call :CompileShader FixedFunctionTexture ps PS %1%

call :CompileShader FixedFunctionTextureLighting vs VS %1%
call :CompileShader FixedFunctionTextureLighting ps PS %1%

call :CompileShader StandardVboLayout vs VS %1%

call :CompileShader StandardVboLayout vs PositionVS %1%
call :CompileShader StandardVboLayout vs PositionNormalVS %1%
call :CompileShader StandardVboLayout vs PositionTextureVS %1%
call :CompileShader StandardVboLayout vs PositionColorVS %1%
call :CompileShader StandardVboLayout vs PositionNormalTextureVS %1%
call :CompileShader StandardVboLayout vs PositionNormalColorVS %1%
call :CompileShader StandardVboLayout vs PositionColorTextureVS %1%
call :CompileShader StandardVboLayout vs PositionNormalColorVS %1%
call :CompileShader StandardVboLayout vs PositionNormalColorTextureVS %1%

call :CompileShader StandardVboLayout vs PositionLightVS %1%
call :CompileShader StandardVboLayout vs PositionNormalLightVS %1%
call :CompileShader StandardVboLayout vs PositionTextureLightVS %1%
call :CompileShader StandardVboLayout vs PositionColorLightVS %1%
call :CompileShader StandardVboLayout vs PositionNormalTextureLightVS %1%
call :CompileShader StandardVboLayout vs PositionNormalColorLightVS %1%
call :CompileShader StandardVboLayout vs PositionColorTextureLightVS %1%
call :CompileShader StandardVboLayout vs PositionNormalColorLightVS %1%
call :CompileShader StandardVboLayout vs PositionNormalColorTextureLightVS %1%
exit /b

:CompileShader
set fxc=fxc /nologo %1.fx /T%2%4 /Zpc /Qstrip_reflect /Qstrip_debug /E%3 /Fhcompiled%4\Compiled%1%3.inc /Vn%1%3
rem set fxc=fxc /nologo %1.fx /T%2_4_0_level_9_1 /Zpc /Qstrip_reflect /Qstrip_debug /E%3 /Fhcompiled\Compiled%1%3.inc /Vn%1%3
rem set fxc=fxc /nologo %1.fx /T%2_5_0 /Zpc /Qstrip_reflect /Qstrip_debug /E%3 /Fhcompiled\Compiled%1%3.inc /Vn%1%3
echo.
echo %fxc%
%fxc% || set error=1
exit /b
