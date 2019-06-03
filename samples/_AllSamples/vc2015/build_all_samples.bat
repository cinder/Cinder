@echo off
setlocal

pushd ..\..\

call "%VS140COMNTOOLS%..\..\vc\bin\amd64\vcvars64.bat"
call :cmpls x64 Release
call :cmpls x64 Debug

call "%VS140COMNTOOLS%vsvars32.bat"
call :cmpls Win32 Release
call :cmpls Win32 Debug

popd

goto :END


:cmpls
set PLATFORM=%1
set CONFIGURATION=%2
call :conv1 _audio\BufferPlayer\vc2015 BufferPlayer.sln
call :conv1 _audio\DelayFeedback\vc2015 DelayFeedback.sln
call :conv1 _audio\InputAnalyzer\vc2015 InputAnalyzer.sln
call :conv1 _audio\MultichannelOutput\vc2015 MultichannelOutput.sln
call :conv1 _audio\NodeAdvanced\vc2015 NodeAdvanced.sln
call :conv1 _audio\NodeBasic\vc2015 NodeBasic.sln
call :conv1 _audio\NodeSubclassing\vc2015 NodeSubclassing.sln
call :conv1 _audio\VoiceBasic\vc2015 VoiceBasic.sln
call :conv1 _audio\VoiceBasicProcessing\vc2015 VoiceBasicProcessing.sln
call :conv1 _opengl\ClothSimulation\vc2015 ClothSimulation.sln
call :conv1 _opengl\Cube\vc2015 Cube.sln
call :conv1 _opengl\CubeMapping\vc2015 CubeMapping.sln
call :conv1 _opengl\DeferredShading\vc2015 DeferredShading.sln
call :conv1 _opengl\DeferredShadingAdvanced\vc2015 DeferredShadingAdvanced.sln
call :conv1 _opengl\DynamicCubeMapping\vc2015 DynamicCubeMapping.sln
call :conv1 _opengl\FboBasic\vc2015 FboBasic.sln
call :conv1 _opengl\FboMultipleRenderTargets\vc2015 FboMultipleRenderTargets.sln
call :conv1 _opengl\GeometryShaderBasic\vc2015 GeometryShaderBasic.sln
call :conv1 _opengl\HighDynamicRange\vc2015 HighDynamicRange.sln
call :conv1 _opengl\ImmediateMode\vc2015 ImmediateMode.sln
call :conv1 _opengl\InstancedTeapots\vc2015 InstancedTeapots.sln
call :conv1 _opengl\LevelOfDetailBasic\vc2015 LevelOfDetailBasic.sln
call :conv1 _opengl\LevelOfDetailIndirect\vc2015 LevelOfDetailIndirect.sln
call :conv1 _opengl\MipMap\vc2015 MipMap.sln
call :conv1 _opengl\MotionBlurFbo\vc2015 MotionBlurFbo.sln
call :conv1 _opengl\MotionBlurVelocityBuffer\vc2015 MotionBlurVelocityBuffer.sln
call :conv1 _opengl\NVidiaComputeParticles\vc2015 NVidiaComputeParticles.sln
call :conv1 _opengl\NormalMapping\vc2015 NormalMapping.sln
call :conv1 _opengl\NormalMappingBasic\vc2015 NormalMappingBasic.sln
call :conv1 _opengl\ObjLoader\vc2015 ObjLoader.sln
call :conv1 _opengl\PBOReadBack\vc2015 PBOReadBack.sln
call :conv1 _opengl\ParticleSphereCPU\vc2015 ParticleSphereCPU.sln
call :conv1 _opengl\ParticleSphereCS\vc2015 ParticleSphereCS.sln
call :conv1 _opengl\ParticleSphereGPU\vc2015 ParticleSphereGPU.sln
call :conv1 _opengl\ParticlesBasic\vc2015 ParticlesBasic.sln
call :conv1 _opengl\PickingFBO\vc2015 PickingFBO.sln
call :conv1 _opengl\PostProcessingAA\vc2015 PostProcessingAA.sln
call :conv1 _opengl\ShadowMapping\vc2015 ShadowMapping.sln
call :conv1 _opengl\ShadowMappingBasic\vc2015 ShadowMappingBasic.sln
call :conv1 _opengl\StencilReflection\vc2015 StencilReflection.sln
call :conv1 _opengl\SuperformulaGPU\vc2015 SuperformulaGPU.sln
call :conv1 _opengl\TessellationBasic\vc2015 TessellationBasic.sln
call :conv1 _opengl\TessellationBezier\vc2015 TessellationBezier.sln
call :conv1 _opengl\TransformFeedbackSmokeParticles\vc2015 TransformFeedbackSmokeParticles.sln
call :conv1 _opengl\VboMesh\vc2015 VboMesh.sln
call :conv1 _svg\AnimatedReveal\vc2015 AnimatedReveal.sln
call :conv1 _svg\EuroMap\vc2015 EuroMap.sln
call :conv1 _svg\GoodNightMorning\vc2015 GoodNightMorning.sln
call :conv1 _svg\SimpleViewer\vc2015 SimpleViewer.sln
call :conv1 _timeline\BasicAppendTween\vc2015 BasicAppendTween.sln
call :conv1 _timeline\BasicTween\vc2015 BasicTween.sln
call :conv1 _timeline\CustomCallback\vc2015 CustomCallback.sln
call :conv1 _timeline\CustomLerp\vc2015 CustomLerp.sln
call :conv1 _timeline\DragTween\vc2015 DragTween.sln
call :conv1 _timeline\ImageAccordion\vc2015 ImageAccordion.sln
call :conv1 _timeline\PaletteBrowser\vc2015 PaletteBrowser.sln
call :conv1 _timeline\TextInputTween\vc2015 TextInputTween.sln
call :conv1 _timeline\VisualDictionary\vc2015 VisualDictionary.sln
call :conv1 ArcballDemo\vc2015 ArcballDemo.sln
call :conv1 BSpline\vc2015 BSpline.sln
call :conv1 BasicApp\vc2015 BasicApp.sln
call :conv1 BasicAppMultiWindow\vc2015 BasicAppMultiWindow.sln
call :conv1 BezierPath\vc2015 BezierPath.sln
call :conv1 BezierPathIteration\vc2015 BezierPathIteration.sln
call :conv1 CairoBasic\vc2015 CairoBasic.sln
call :conv1 CameraPersp\vc2015 CameraPersp.sln
call :conv1 CaptureBasic\vc2015 CaptureBasic.sln
call :conv1 CaptureCube\vc2015 CaptureCube.sln
call :conv1 ClipboardBasic\vc2015 ClipboardBasic.sln
call :conv1 Earthquake\vc2015 Earthquake.sln
call :conv1 EaseGallery\vc2015 EaseGallery.sln
call :conv1 Extrude\vc2015 Extrude.sln
call :conv1 FallingGears\vc2015 FallingGears.sln
call :conv1 FlickrTestMultithreaded\vc2015 FlickrTestMultithreaded.sln
call :conv1 FontSample\vc2015 FontSample.sln
call :conv1 FrustumCulling\vc2015 FrustumCulling.sln
call :conv1 Geometry\vc2015 Geometry.sln
call :conv1 HighDensityDisplay\vc2015 HighDensityDisplay.sln
call :conv1 ImageFileBasic\vc2015 ImageFileBasic.sln
call :conv1 ImageHeightField\vc2015 ImageHeightField.sln
call :conv1 Logging\vc2015 Logging.sln
call :conv1 MandelbrotGLSL\vc2015 MandelbrotGLSL.sln
call :conv1 MultiTouchBasic\vc2015 MultiTouchBasic.sln
call :conv1 ParamsBasic\vc2015 ParamsBasic.sln
call :conv1 perlinTest\vc2015 perlinTest.sln
call :conv1 Picking3D\vc2015 Picking3D.sln
call :conv1 QuaternionAccum\vc2015 QuaternionAccum.sln
call :conv1 RDiffusion\vc2015 RDiffusion.sln
call :conv1 Renderer2dBasic\vc2015 Renderer2d.sln
call :conv1 SaveImage\vc2015 SaveImage.sln
call :conv1 SerialCommunication\vc2015 SerialCommunication.sln
call :conv1 slerpBasic\vc2015 slerpBasic.sln
call :conv1 StereoscopicRendering\vc2015 StereoscopicRendering.sln
call :conv1 SurfaceBasic\vc2015 SurfaceBasic.sln
call :conv1 TextBox\vc2015 TextBox.sln
call :conv1 TextTest\vc2015 TextTest.sln
call :conv1 TextureFont\vc2015 TextureFont.sln
call :conv1 Triangulation\vc2015 Triangulation.sln
call :conv1 Tubular\vc2015 Tubular.sln
call :conv1 VoronoiGpu\vc2015 VoronoiGpu.sln
call :conv1 Wisteria\vc2015 Wisteria.sln
call :conv1 QuickTimeAdvanced\vc2015 QTimeAdvApp.sln	Win32Only
call :conv1 QuickTimeBasic\vc2015 QuickTimeBasic.sln	Win32Only
call :conv1 QuickTimeIteration\vc2015 QuickTimeIteration.sln	Win32Only
call :conv1 Kaleidoscope\vc2015 Instascope.sln
rem call :conv1 _AllSamples\vc2015 AllSamples.sln
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
