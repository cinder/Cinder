@echo off
setlocal

pushd ..\..\

set "sav_path=%PATH%"
set "PATH=%sav_path%"
call "%VS140COMNTOOLS%..\..\vc\bin\amd64\vcvars64.bat"
call :cmpls x64 Release
rem call :cmpls x64 Debug


set "PATH=%sav_path%"
call "%VS140COMNTOOLS%vsvars32.bat"
call :cmpls Win32 Release
rem call :cmpls Win32 Debug


popd

goto :END


:cmpls
set PLATFORM=%1
set CONFIGURATION=%2

call :conv1 _audio\DeviceTest\vc2015 DeviceTest.sln
call :conv1 _audio\NodeEffectsTest\vc2015 NodeEffectsTest.sln
call :conv1 _audio\NodeTest\vc2015 NodeTest.sln
call :conv1 _audio\ParamTest\vc2015 ParamTest.sln
call :conv1 _audio\SampleTest\vc2015 SampleTest.sln
call :conv1 _audio\SpectralTest\vc2015 SpectralTest.sln
call :conv1 _audio\StressTest\vc2015 StressTest.sln
call :conv1 _audio\VoiceTest\vc2015 VoiceTest.sln
call :conv1 _audio\WaveTableTest\vc2015 WaveTableTest.sln
call :conv1 _opengl\BufferTexture\vc2015 BufferTexture.sln
call :conv1 _opengl\CompressedTexture\vc2015 CompressedTexture.sln
call :conv1 _opengl\ConvenienceDrawingMethods\vc2015 ConvenienceDrawingMethods.sln
call :conv1 _opengl\CubeMapLayout\vc2015 CubeMapLayout.sln
call :conv1 _opengl\GlslProgAttribTest\vc2015 GlslProgAttribTest.sln
call :conv1 _opengl\ImageSourcePbo\vc2015 ImageSourcePbo.sln
call :conv1 _opengl\IntegerAttribTest\vc2015 IntegerAttribTest.sln
call :conv1 _opengl\ObjectTracking\vc2015 ObjectTracking.sln
call :conv1 _opengl\PboUploadTest\vc2015 PboUploadTest.sln
call :conv1 _opengl\QueryTest\vc2015 QueryTest.sln
call :conv1 _opengl\SamplerObject\proj\vc2015 SamplerObject.sln
call :conv1 _opengl\ShaderPreprocessorTest\vc2015 ShaderPreprocessorTest.sln
call :conv1 _opengl\Texture1d\vc2015 Texture1d.sln
call :conv1 _opengl\Texture3d\vc2015 Texture3d.sln
call :conv1 _opengl\TextureUpload\vc2015 TextureUpload.sln
call :conv1 _opengl\TransformFeedbackIntro\vc2015 TransformFeedbackSingleObject.sln
call :conv1 _opengl\usampler2D\vc2015 usampler2D.sln
call :conv1 AABBtest\vc2015 AABBtest.sln
call :conv1 AppTest\vc2015 AppTest.sln
call :conv1 ArcballTest\vc2015 ArcballTest.sln
call :conv1 assetTest\vc2015 assetTest.sln
call :conv1 base64Test\vc2015 base64Test.sln
call :conv1 CameraLensShiftTest\vc2015 CameraLensShiftTest.sln
call :conv1 CaptureTest\vc2015 CaptureTest.sln
call :conv1 CinderDLL\proj\vc2015 CinderDLL.sln
call :conv1 DebugTest\vc2015 DebugTest.sln
call :conv1 cmdLineArgs\vc2015 cmdLineArgs.sln
call :conv1 DisplayTest\vc2015 DisplayTest.sln
call :conv1 eventTest\vc2015 eventTest.sln
call :conv1 ExtrudeSpline\vc2015 ExtrudeSpline.sln
call :conv1 FloatCamera\vc2015 FloatCamera.sln
call :conv1 GeomSourceMods\vc2015 GeomSourceMods.sln
call :conv1 imageProcessing\vc2015 imageProcessing.sln
call :conv1 LineBreakTest\vc2015 LineBreakTest.sln
call :conv1 resizeTest\vc2015 resizeTest.sln
call :conv1 ScreenSaverTest\vc2015 TestScreensaver.sln
call :conv1 ShapeTest\vc2015 ShapeTest.sln
call :conv1 SphereProjection\vc2015 SphereProjection.sln
call :conv1 StackBlurTest\vc2015 StackBlurTest.sln
call :conv1 streamFileTest\vc2015 streamFileTest.sln
call :conv1 SystemTest\vc2015 SystemTest.sln
call :conv1 TriMeshReadWriteTest\vc2015 TriMeshReadWriteTest.sln
call :conv1 unit\vc2015 unit.sln
call :conv1 windowTest\vc2015 windowTest.sln
call :conv1 WindowToWorldTest\vc2015 WindowToWorldTest.sln
call :conv1 XMLTest\vc2015 XMLTest.sln
rem call :conv1 _audio\AudioTest.msw AudioTest.sln
rem call :conv1 cairoTest\vc2015 cairoTest.sln
rem call :conv1 mathTest\vc2015 mathTest.sln
rem call :conv1 SignalsTest\vc2015 SignalsTest.sln

exit /b

:conv1
set TGTDIR=%1
set TGTSLN=%2

if not "%3"=="Win32Only" goto SKIP1
if "%PLATFORM%"=="Win32" goto SKIP1
exit /b
:SKIP1

pushd %TGTDIR%
msbuild %TGTSLN% /t:Build /p:Platform=%PLATFORM% /p:Configuration=%CONFIGURATION%
popd

exit /b


:END
echo END
endlocal
