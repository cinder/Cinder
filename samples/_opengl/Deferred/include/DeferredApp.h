#pragma once

#include "cinder/app/AppBasic.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Ubo.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/Json.h"
#include "cinder/MayaCamUI.h"
#include "cinder/params/Params.h"

#include "Light.h"
#include "Material.h"

class DeferredApp : public ci::app::AppBasic 
{
private:
	enum : GLuint {
		HORIZONTAL	= GL_COLOR_ATTACHMENT0,
		VERTICAL	= GL_COLOR_ATTACHMENT1
	} typedef BlurDirection;

	enum : GLuint {
		ALBEDO			= GL_COLOR_ATTACHMENT0,
		MATERIAL		= GL_COLOR_ATTACHMENT1, // r = material ID
		NORMAL_DEPTH	= GL_COLOR_ATTACHMENT2, // rgb = normal, a = depth
		POSITION		= GL_COLOR_ATTACHMENT3
	} typedef GBuffer;

	enum : size_t {
		NONE,
		AREA,
		DIRECTIONAL,
		POINT,
		SPOT
	} typedef LightType;

	enum : int32_t {
		MATERIAL_CUBE, 
		MATERIAL_FLOOR, 
		MATERIAL_LIGHT
	} typedef MaterialType;

	typedef std::map<std::string, LightType>	StringLightTypeMap;
public:
	void								mouseDown( ci::app::MouseEvent event );
	void								mouseDrag( ci::app::MouseEvent event );
	void								mouseUp( ci::app::MouseEvent event );
	void								prepareSettings( ci::app::AppBasic::Settings* settings );
	void								setup();
	void								update();
private:
	bool								configRead();
	void								configWrite();
	ci::JsonTree						mConfig;
	ci::fs::path						mConfigPath;
	
	ci::Vec3f							mCameraEyePoint;
	ci::Vec3f							mCameraEyePointPrev;
	ci::Vec3f							mCameraCenterOfInterest;
	ci::Vec3f							mCameraCenterOfInterestPrev;
	ci::MayaCamUI						mMayaCam;

	bool								mMouseDown;
	bool								mMouseDownLeft;
	bool								mMouseDownMiddle;
	bool								mMouseDownRight;
	ci::Vec2f							mMousePosition;
	ci::Vec2f							mMouseTarget;

	void								createCubes();
	std::vector<ci::Matrix44f>			mCubes;
	int32_t								mCubeCount;
	int32_t								mCubeCountPrev;
	ci::Vec3f							mCubeDistanceMax;
	ci::Vec3f							mCubeDistanceMaxPrev;
	ci::Vec3f							mCubeDistanceMin;
	ci::Vec3f							mCubeDistanceMinPrev;
	ci::Vec3f							mCubeOffset;
	float								mCubeRotationSpeed;
	float								mCubeScaleMax;
	float								mCubeScaleMaxPrev;
	float								mCubeScaleMin;
	float								mCubeScaleMinPrev;
	ci::gl::TextureRef					mTextureCube;

	ci::Vec3f							mFloorPosition;
	ci::Vec3f							mFloorRotation;
	float								mFloorScale;

	std::vector<AreaLight>				mAreaLights;
	std::vector<DirectionalLight>		mDirectionalLights;
	bool								mLightEnableArea;
	bool								mLightEnableDirectional;
	bool								mLightEnablePoint;
	bool								mLightEnableSpot;
	std::vector<std::string>			mLightTypeNames;
	StringLightTypeMap					mLightTypeMap;
	std::map<size_t, PointLight>		mPointLights;
	std::vector<SpotLight>				mSpotLights;

	bool								mBufferMaterials;
	std::vector<std::string>			mMaterialNames;
	std::vector<Material>				mMaterials;
	ci::gl::UboRef						mUboMaterial;
		
	float								mDepthScale;
	float								mSsaoBlurAttenuation;
	float								mSsaoBlurSize;
	float								mSsaoFalloff; 
	float								mSsaoOffset;
	float								mSsaoRadius;
	float								mSsaoStrength;
	ci::gl::TextureRef					mTextureRandom;
	
	float								mShadowBias;
	float								mShadowBlurSize;
	ci::CameraPersp						mShadowCamera;
	ci::Vec3f							mShadowEyePoint;
	float								mShadowFov;
	float								mShadowFovPrev;
	float								mShadowSamples;
	ci::Vec3f							mShadowTarget;

	float								mBloomAttenuation;
	float								mBloomSize;

	float 								mDofBias;
	float 								mDofBlend;
	float 								mDofBlurSize;
	float								mDofFocalDepth;
	float								mDofRange;

	float								mColorBlend;
	float								mColorOffset;
	float								mColorContrast;
	float								mColorMultiply;
	ci::ColorAf							mColorSaturation;
	
	void								loadShaders();
	ci::gl::GlslProgRef					mGlslProgBlend;
	ci::gl::GlslProgRef					mGlslProgBloom;
	ci::gl::GlslProgRef					mGlslProgBlur;
	ci::gl::GlslProgRef					mGlslProgColor;
	ci::gl::GlslProgRef					mGlslProgComposite;
	ci::gl::GlslProgRef					mGlslProgCubeShadow;
	ci::gl::GlslProgRef					mGlslProgDebugGbuffer;
	ci::gl::GlslProgRef					mGlslProgDebugMaterial;
	ci::gl::GlslProgRef					mGlslProgDof;
	ci::gl::GlslProgRef					mGlslProgFxaa;
	ci::gl::GlslProgRef					mGlslProgGBuffer;
	ci::gl::GlslProgRef					mGlslProgGBufferLight;
	ci::gl::GlslProgRef					mGlslProgLightDirectional;
	ci::gl::GlslProgRef					mGlslProgLightPoint;
	ci::gl::GlslProgRef					mGlslProgShadowMap;
	ci::gl::GlslProgRef					mGlslProgSsao;

	ci::gl::GlslProgRef					mShaderColorDebug;
	ci::gl::GlslProgRef					mShaderTextureDebug;
	ci::gl::GlslProgRef					mShaderColorMain;
	ci::gl::GlslProgRef					mShaderTextureMain;

	ci::gl::FboRef						mFboBloom;
	ci::gl::FboRef						mFboColor;
	ci::gl::FboRef						mFboComposite;
	ci::gl::FboRef						mFboDof;
	ci::gl::FboRef						mFboGBuffer;
	ci::gl::FboRef						mFboLBuffer;
	ci::gl::FboRef						mFboShadowMap;
	ci::gl::FboRef						mFboSsao;

	ci::gl::Texture2dRef				mTextureFboBloomHorizontal;
	ci::gl::Texture2dRef				mTextureFboBloomVertical;
	ci::gl::Texture2dRef				mTextureFboColor;
	ci::gl::Texture2dRef				mTextureFboComposite;
	ci::gl::Texture2dRef				mTextureFboDofHorizontal;
	ci::gl::Texture2dRef				mTextureFboDofVertical;
	ci::gl::Texture2dRef				mTextureFboGBufferAlbedo;
	ci::gl::Texture2dRef				mTextureFboGBufferDepth;
	ci::gl::Texture2dRef				mTextureFboGBufferMaterial;
	ci::gl::Texture2dRef				mTextureFboGBufferNormalDepth;
	ci::gl::Texture2dRef				mTextureFboGBufferPosition;
	ci::gl::Texture2dRef				mTextureFboLBuffer;
	ci::gl::Texture2dRef				mTextureFboShadowMap;
	ci::gl::Texture2dRef				mTextureFboSsaoHorizontal;
	ci::gl::Texture2dRef				mTextureFboSsaoVertical;
	ci::gl::Texture2dRef				mTextureFboSsao;

	ci::gl::VboMeshRef					mMeshCube;
	ci::gl::VboMeshRef					mMeshRect;
	ci::gl::VboMeshRef					mMeshSphere;

	void								drawDebug();
	void								drawMain();
	void								drawParams();
	void								keyDownMain( ci::app::KeyEvent event );
	void								resizeMain();
	ci::DisplayRef						mDisplayPrimary;
	ci::DisplayRef						mDisplaySecondary;
	ci::app::WindowRef					mWindowDebug;
	ci::app::WindowRef					mWindowMain;
	ci::app::WindowRef					mWindowParams;

	bool								mDrawBloom;
	bool								mDrawDof;
	bool								mDrawFxaa;
	bool								mDrawShadow;
	bool								mDrawSsao;
	bool								mDrawWireframe;

	float								mFrameRate;
	bool								mFullScreen;
	ci::params::InterfaceGlRef			mParams;
	void								screenShot();
};
