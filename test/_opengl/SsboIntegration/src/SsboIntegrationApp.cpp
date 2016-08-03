#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/gl/Ssbo.h"
#include "cinder/Perlin.h"

#include "cinder/Rand.h"

//#define CPU_UPLOAD

using namespace ci;
using namespace ci::app;
using namespace std;

const int NUM_CUBES = 100000;

class SsboIntegrationApp : public App {
public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void update() override;
	void draw() override;

	gl::BatchRef mCubes;
	gl::VboRef mRandIndicesVbo;
	gl::GlslProgRef mGlsl;
	gl::SsboRef mTransformStorage;

	ci::CameraPersp mCamera;
	Perlin mPerlin;
	ci::Timer mTimer;

};

void SsboIntegrationApp::setup()
{
	mCamera.setPerspective(60, getWindowAspectRatio(), .01, 10000);
	mCamera.lookAt(vec3(0, 0, 20), vec3(0));

	std::vector<mat4> transforms;

	for (uint32_t i = 0; i < NUM_CUBES; i++) {

		auto t = mat4();
		t *= ci::translate(randVec3()*5.f);
		t *= ci::toMat4(quat(randVec3()));
		t *= ci::scale(randVec3());

		transforms.push_back(t);

	}

	auto copy = GL_STATIC_DRAW;

#if defined(CPU_UPLOAD)
	copy = GL_DYNAMIC_COPY;
#endif;

	mTransformStorage = gl::Ssbo::create(sizeof(mat4)*transforms.size(), transforms.data(), copy);
	mTransformStorage->bindBase(2);
	CI_CHECK_GL();

	auto mesh = gl::VboMesh::create(geom::Cube());
	geom::BufferLayout layout;

	try {
		auto fmt = gl::GlslProg::Format()
#if defined(CPU_UPLOAD)
			.define( "CPU_UPLOAD", "1" )
#else
			.define("CPU_UPLOAD", "0")
#endif
			.vertex(loadAsset("ssbo.vert"))
			.fragment(loadAsset("ssbo.frag"));
		mGlsl = gl::GlslProg::create(fmt);
	}
	catch (const gl::GlslProgCompileExc& e)
	{
		CI_LOG_E(e.what());
	}

	mGlsl->shaderStorageBlock("transform_data", 2);
	mGlsl->uniform("uNumCubes", static_cast<float>(NUM_CUBES));

	mCubes = gl::Batch::create(mesh, mGlsl);
	CI_CHECK_GL();

}

void SsboIntegrationApp::mouseDown(MouseEvent event)
{
}

void SsboIntegrationApp::update()
{
	if (getElapsedFrames() % 30 == 0)
		CI_LOG_I("frameTime: " << mTimer.getSeconds());
	mTimer.start();

#if defined( CPU_UPLOAD )
	auto mapped = (mat4*)mTransformStorage->map(GL_WRITE_ONLY);
	for (int i = 0; i < NUM_CUBES; i++ ) {
	auto posx = mPerlin.fBm( vec2( i + getElapsedSeconds()*.05 ) )*10.f;
	auto posy = mPerlin.fBm(vec2( i + 100.f + getElapsedSeconds()*.05 ))*10.f;
	auto posz = mPerlin.fBm(vec2( i + 200.f + getElapsedSeconds()*.05 ))*10.f;
	(*mapped)[3].x = posx;
	(*mapped)[3].y = posy;
	(*mapped)[3].z = posz;
	++mapped;
	}
	mTransformStorage->unmap();
#endif
}

void SsboIntegrationApp::draw()
{
	gl::clear(Color(0, 0, 0));

	gl::ScopedMatrices push;
	gl::setMatrices(mCamera);
	gl::ScopedDepth depth(true);

	mCubes->drawInstanced(NUM_CUBES);

}

CINDER_APP(SsboIntegrationApp, RendererGl)
