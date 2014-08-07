#include "cinder/Camera.h"
#include "cinder/GeomIo.h"
#include "cinder/ImageIo.h"
#include "cinder/MayaCamUI.h"
#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/params/Params.h"

#include "DebugMesh.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class GeometryApp : public AppNative
{
public:
	typedef enum { CAPSULE, CONE, CUBE, CYLINDER, HELIX, ICOSAHEDRON, ICOSPHERE, SPHERE, TEAPOT, TORUS } Primitive;
	typedef enum { LOW, DEFAULT, HIGH } Quality;
	typedef enum { SHADED, WIREFRAME } ViewMode;

	void prepareSettings(Settings* settings);
	void setup();
	void update();
	void draw();

	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );

	void keyDown( KeyEvent event );

	void resize();
private:
	void createGrid();
	void createPhongShader();
	void createWireframeShader();
	void createPrimitive();
	void createParams();

	void setSubdivision(int subdivision) { mSubdivision = math<int>::clamp(subdivision, 1, 5); createPrimitive(); }
	int  getSubdivision() const { return mSubdivision; }

	void enableColors(bool enabled=true) { mShowColors = enabled; createPrimitive(); }
	bool isColorsEnabled() const { return mShowColors; }

	Primitive			mPrimitiveSelected;
	Primitive			mPrimitiveCurrent;
	Quality				mQualitySelected;
	Quality				mQualityCurrent;
	ViewMode			mViewMode;

	int					mSubdivision;

	bool				mShowColors;
	bool				mShowNormals;
	bool				mShowGrid;

	CameraPersp			mCamera;
	MayaCamUI			mMayaCam;
	bool				mRecenterCamera;
	Vec3f				mCameraCOI;

	gl::VertBatchRef	mGrid;

	gl::BatchRef		mPrimitive;
	gl::BatchRef		mPrimitiveWireframe;
	gl::BatchRef		mNormals;

	gl::GlslProgRef		mPhongShader;
	gl::GlslProgRef		mWireframeShader;

	gl::TextureRef		mTexture;
	
#if ! defined( CINDER_GL_ES )
	params::InterfaceGlRef	mParams;
#endif
};

void GeometryApp::prepareSettings(Settings* settings)
{
	settings->setWindowSize(1024, 768);
}

void GeometryApp::setup()
{
	// Initialize variables.
	mPrimitiveSelected = mPrimitiveCurrent = ICOSAHEDRON;
	mQualitySelected = mQualityCurrent = HIGH;
	mViewMode = SHADED;

	mShowColors = false;
	mShowNormals = false;
	mShowGrid = false;

	mSubdivision = 1;
	
	// Load the textures.
	gl::Texture::Format fmt;
	fmt.setAutoInternalFormat();
	fmt.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
	mTexture = gl::Texture::create( loadImage( loadAsset("stripes.jpg") ), fmt );

	// Setup the camera.
	mCamera.setEyePoint( Vec3f(3, 3, 6).normalized() * 5.0f );
	mCamera.setCenterOfInterestPoint( mCameraCOI );

	// Load and compile the shaders.
	createPhongShader();
	createWireframeShader();

	// Create the meshes.
	createGrid();
	createPrimitive();

	// Enable the depth buffer.
	gl::enableDepthRead();
	gl::enableDepthWrite();

	// Create a parameter window, so we can toggle stuff.
	createParams();
}

void GeometryApp::update()
{
	// If another primitive or quality was selected, reset the subdivision and recreate the primitive.
	if(mPrimitiveCurrent != mPrimitiveSelected || mQualitySelected != mQualityCurrent) {
		mSubdivision = 1;
		mPrimitiveCurrent = mPrimitiveSelected;
		mQualityCurrent = mQualitySelected;
		createPrimitive();
	}

	// After creating a new primitive, gradually move the camera to get a good view.
	if(mRecenterCamera) {
		float distance = mCamera.getEyePoint().distance(mCameraCOI);
		mCamera.setEyePoint( mCameraCOI - lerp(distance, 5.0f, 0.1f) * mCamera.getViewDirection() );
		mCamera.setCenterOfInterestPoint( lerp(mCamera.getCenterOfInterestPoint(), mCameraCOI, 0.25f) );
	}
}

void GeometryApp::draw()
{
	// Prepare for drawing.
	gl::clear( Color::black() );
	gl::setMatrices( mCamera );
	
	// Draw the grid.
	if(mShowGrid && mGrid) {
		gl::ScopedGlslProg scopedGlslProg( gl::context()->getStockShader( gl::ShaderDef().color() ) );
		mGrid->draw();
	}

	if(mPrimitive)
	{
		gl::ScopedTextureBind scopedTextureBind( mTexture );

		// Rotate it slowly around the y-axis.
		gl::pushModelViewMatrices();
		gl::rotate( 20.0f* float( getElapsedSeconds() ), 0.0f, 1.0f, 0.0f );

		// Draw the normals.
		if(mShowNormals && mNormals)
			mNormals->draw();

		// Draw the primitive.
		gl::color( Color(0.7f, 0.5f, 0.3f) );
		
		// (If transparent, render the back side first).
		if(mViewMode == WIREFRAME) {
			gl::enableAlphaBlending();

			gl::enable( GL_CULL_FACE );
			glCullFace( GL_FRONT );

			mWireframeShader->uniform("uBrightness", 0.5f);
			mPrimitiveWireframe->draw();
		}

		// (Now render the front side.)
		if(mViewMode == WIREFRAME) {
			glCullFace( GL_BACK );

			mWireframeShader->uniform("uBrightness", 1.0f);
			mPrimitiveWireframe->draw();
			
			gl::disable( GL_CULL_FACE );

			gl::disableAlphaBlending();
		}
		else
			mPrimitive->draw();
		
		// Done.
		gl::popModelViewMatrices();
	}

	// Render the parameter window.
#if ! defined( CINDER_GL_ES )
	if( mParams )
		mParams->draw();
#endif
}

void GeometryApp::mouseDown( MouseEvent event )
{
	mRecenterCamera = false;

	mMayaCam.setCurrentCam( mCamera );
	mMayaCam.mouseDown( event.getPos() );
}

void GeometryApp::mouseDrag( MouseEvent event )
{
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
	mCamera = mMayaCam.getCamera();
}

void GeometryApp::resize(void)
{
	mCamera.setAspectRatio( getWindowAspectRatio() );
	
	if(mWireframeShader)
		mWireframeShader->uniform( "uViewportSize", Vec2f( getWindowSize() ) );
}

void GeometryApp::keyDown( KeyEvent event )
{
	switch( event.getCode() )
	{
	case KeyEvent::KEY_SPACE:
		mPrimitiveSelected = static_cast<Primitive>( static_cast<int>(mPrimitiveSelected) + 1 );
		createPrimitive();
		break;
	case KeyEvent::KEY_c:
		mShowColors = !mShowColors;
		createPrimitive();
		break;
	case KeyEvent::KEY_n:
		mShowNormals = !mShowNormals;
		break;
	case KeyEvent::KEY_g:
		mShowGrid = !mShowGrid;
		break;
	case KeyEvent::KEY_q:
		mQualitySelected = Quality( (int)(mQualitySelected + 1) % 3 );
		break;
	case KeyEvent::KEY_w:
		if(mViewMode == WIREFRAME)
			mViewMode = SHADED;
		else
			mViewMode = WIREFRAME;
		break;
	case KeyEvent::KEY_RETURN:
		createPhongShader();
		createPrimitive();
		break;
	}
}

void GeometryApp::createParams()
{
#if ! defined( CINDER_GL_ES )
	std::string primitives[] = { "Capsule", "Cone", "Cube", "Cylinder", "Helix", "Icosahedron", "Icosphere", "Sphere", "Teapot", "Torus" };
	std::string qualities[] = { "Low", "Default", "High" };
	std::string viewmodes[] = { "Shaded", "Wireframe" };

	mParams = params::InterfaceGl::create( getWindow(), "Geometry Demo", Vec2i(340, 200) );
	mParams->setOptions( "", "valueswidth=100 refresh=0.1" );

	mParams->addParam( "Primitive", vector<string>(primitives,primitives+10), (int*) &mPrimitiveSelected );
	mParams->addParam( "Quality", vector<string>(qualities,qualities+3), (int*) &mQualitySelected );
	mParams->addParam( "Viewing Mode", vector<string>(viewmodes,viewmodes+2), (int*) &mViewMode );

	mParams->addSeparator();
	
	{
		std::function<void(int)> setter	= std::bind( &GeometryApp::setSubdivision, this, std::placeholders::_1 );
		std::function<int()> getter		= std::bind( &GeometryApp::getSubdivision, this );
		mParams->addParam( "Subdivision", setter, getter );
	}

	mParams->addSeparator();

	mParams->addParam( "Show Grid", &mShowGrid );
	mParams->addParam( "Show Normals", &mShowNormals );
	{
		std::function<void(bool)> setter	= std::bind( &GeometryApp::enableColors, this, std::placeholders::_1 );
		std::function<bool()> getter		= std::bind( &GeometryApp::isColorsEnabled, this );
		mParams->addParam( "Show Colors", setter, getter );
	}
#endif
}

void GeometryApp::createGrid()
{
	mGrid = gl::VertBatch::create( GL_LINES );
	mGrid->begin( GL_LINES );
	mGrid->color( Color(0.25f, 0.25f, 0.25f) ); mGrid->vertex( -10.0f, 0.0f, 0.0f );
	mGrid->color( Color(0.25f, 0.25f, 0.25f) ); mGrid->vertex( 0.0f, 0.0f, 0.0f );
	mGrid->color( Color(1, 0, 0) ); mGrid->vertex( 0.0f, 0.0f, 0.0f );
	mGrid->color( Color(1, 0, 0) ); mGrid->vertex( 20.0f, 0.0f, 0.0f );
	mGrid->color( Color(0, 1, 0) ); mGrid->vertex( 0.0f, 0.0f, 0.0f );
	mGrid->color( Color(0, 1, 0) ); mGrid->vertex( 0.0f, 20.0f, 0.0f );
	mGrid->color( Color(0.25f, 0.25f, 0.25f) ); mGrid->vertex( 0.0f, 0.0f, -10.0f );
	mGrid->color( Color(0.25f, 0.25f, 0.25f) ); mGrid->vertex( 0.0f, 0.0f, 0.0f );
	mGrid->color( Color(0, 0, 1) ); mGrid->vertex( 0.0f, 0.0f, 0.0f );
	mGrid->color( Color(0, 0, 1) ); mGrid->vertex( 0.0f, 0.0f, 20.0f );
	for( int i = -10; i <= 10; ++i ) {
		if( i == 0 )
			continue;

		mGrid->color( Color(0.25f, 0.25f, 0.25f) );
		mGrid->color( Color(0.25f, 0.25f, 0.25f) );
		mGrid->color( Color(0.25f, 0.25f, 0.25f) );
		mGrid->color( Color(0.25f, 0.25f, 0.25f) );
		
		mGrid->vertex( float(i), 0.0f, -10.0f );
		mGrid->vertex( float(i), 0.0f, +10.0f );
		mGrid->vertex( -10.0f, 0.0f, float(i) );
		mGrid->vertex( +10.0f, 0.0f, float(i) );
	}
	mGrid->end();
}

void GeometryApp::createPrimitive(void)
{
	geom::SourceRef primitive;
	std::string     name;

	switch( mPrimitiveCurrent ) {
	default:
		mPrimitiveSelected = CAPSULE;
	case CAPSULE:
		switch(mQualityCurrent) {
			case DEFAULT: primitive = geom::SourceRef( new geom::Capsule( geom::Capsule() ) ); break;
			case LOW: primitive = geom::SourceRef( new geom::Capsule( geom::Capsule().segments(6).slices(1) ) ); break;
			case HIGH: primitive = geom::SourceRef( new geom::Capsule( geom::Capsule().segments(60).slices(20) ) ); break;
		}
		break;
	case CONE:
		switch(mQualityCurrent) {
			case DEFAULT: primitive = geom::SourceRef( new geom::Cone( geom::Cone() ) ); break;
			case LOW: primitive = geom::SourceRef( new geom::Cone( geom::Cone().segments(6).slices(1) ) ); break;
			case HIGH: primitive = geom::SourceRef( new geom::Cone( geom::Cone().segments(60).slices(60) ) ); break;
		}
		break;
	case CUBE:
		primitive = geom::SourceRef( new geom::Cube( geom::Cube() ) );
		break;
	case CYLINDER:
		switch(mQualityCurrent) {
			case DEFAULT: primitive = geom::SourceRef( new geom::Cylinder( geom::Cylinder() ) ); break;
			case LOW: primitive = geom::SourceRef( new geom::Cylinder( geom::Cylinder().segments(6).slices(1) ) ); break;
			case HIGH: primitive = geom::SourceRef( new geom::Cylinder( geom::Cylinder().segments(60).slices(20) ) ); break;
		}
		break;
	case HELIX:
		switch(mQualityCurrent) {
			case DEFAULT: primitive = geom::SourceRef( new geom::Helix( geom::Helix() ) ); break;
			case LOW: primitive = geom::SourceRef( new geom::Helix( geom::Helix().segmentsAxis(12).segmentsRing(6) ) ); break;
			case HIGH: primitive = geom::SourceRef( new geom::Helix( geom::Helix().segmentsAxis(60).segmentsRing(60) ) ); break;
		}
		break;
	case ICOSAHEDRON:
		primitive = geom::SourceRef( new geom::Icosahedron( geom::Icosahedron() ) );
		break;
	case ICOSPHERE:
		switch(mQualityCurrent) {
			case DEFAULT: primitive = geom::SourceRef( new geom::Icosphere( geom::Icosphere() ) ); break;
			case LOW: primitive = geom::SourceRef( new geom::Icosphere( geom::Icosphere().subdivision(1) ) ); break;
			case HIGH: primitive = geom::SourceRef( new geom::Icosphere( geom::Icosphere().subdivision(5) ) ); break;
		}
		break;
	case SPHERE:
		switch(mQualityCurrent) {
			case DEFAULT: primitive = geom::SourceRef( new geom::Sphere( geom::Sphere() ) ); break;
			case LOW: primitive = geom::SourceRef( new geom::Sphere( geom::Sphere().segments(6) ) ); break;
			case HIGH: primitive = geom::SourceRef( new geom::Sphere( geom::Sphere().segments(60) ) ); break;
		}
		break;
	case TEAPOT:
		switch(mQualityCurrent) {
			case DEFAULT: primitive = geom::SourceRef( new geom::Teapot( geom::Teapot() ) ); break;
			case LOW: primitive = geom::SourceRef( new geom::Teapot( geom::Teapot().subdivision( 2 ) ) ); break;
			case HIGH: primitive = geom::SourceRef( new geom::Teapot( geom::Teapot().subdivision( 12 ) ) ); break;
		}
		break;
	case TORUS:
		switch(mQualityCurrent) {
			case DEFAULT: primitive = geom::SourceRef( new geom::Torus( geom::Torus() ) ); break;
			case LOW: primitive = geom::SourceRef( new geom::Torus( geom::Torus().segmentsAxis(12).segmentsRing(6) ) ); break;
			case HIGH: primitive = geom::SourceRef( new geom::Torus( geom::Torus().segmentsAxis(60).segmentsRing(60) ) ); break;
		}
		break;
	}

	if( mShowColors )
		primitive->enable( geom::Attrib::COLOR );
	
	TriMesh mesh( *primitive );
	mCameraCOI = mesh.calcBoundingBox().getCenter();
	mRecenterCamera = true;

	if(mSubdivision > 1)
		mesh.subdivide(mSubdivision);

	mPrimitive = gl::Batch::create( mesh, mPhongShader );
	mPrimitiveWireframe = gl::Batch::create( mesh, mWireframeShader );
	mNormals = gl::Batch::create( DebugMesh( mesh, Color(1,1,0) ), gl::context()->getStockShader( gl::ShaderDef().color() ) );

	getWindow()->setTitle( "Geometry - " + to_string( mesh.getNumVertices() ) + " vertices" );
}

void GeometryApp::createPhongShader(void)
{
	try {
		mPhongShader = gl::GlslProg::create( gl::GlslProg::Format()
			.vertex(
				"#version 150\n"
				"\n"
				"uniform mat4	ciModelViewProjection;\n"
				"uniform mat4	ciModelView;\n"
				"uniform mat3	ciNormalMatrix;\n"
				"\n"
				"in vec4		ciPosition;\n"
				"in vec3		ciNormal;\n"
				"in vec4		ciColor;\n"
				"\n"
				"out VertexData {\n"
				"	vec4 position;\n"
				"	vec3 normal;\n"
				"	vec4 color;\n"
				"} vVertexOut;\n"
				"\n"
				"void main(void) {\n"
				"	vVertexOut.position = ciModelView * ciPosition;\n"
				"	vVertexOut.normal = ciNormalMatrix * ciNormal;\n"
				"	vVertexOut.color = ciColor;\n"
				"	gl_Position = ciModelViewProjection * ciPosition;\n"
				"}\n"
			)
			.fragment(
				"#version 150\n"
				"\n"
				"in VertexData	{\n"
				"	vec4 position;\n"
				"	vec3 normal;\n"
				"	vec4 color;\n"
				"} vVertexIn;\n"
				"\n"
				"out vec4 oColor;\n"
				"\n"
				"void main(void) {\n"
				"	// set diffuse and specular colors\n"
				"	vec3 cDiffuse = vVertexIn.color.rgb;\n"
				"	vec3 cSpecular = vec3(0.3, 0.3, 0.3);\n"
				"\n"
				"	// light properties in view space\n"
				"	vec3 vLightPosition = vec3(0.0, 0.0, 0.0);\n"
				"\n"
				"	// lighting calculations\n"
				"	vec3 vVertex = vVertexIn.position.xyz;\n"
				"	vec3 vNormal = normalize( vVertexIn.normal );\n"
				"	vec3 vToLight = normalize( vLightPosition - vVertex );\n"
				"	vec3 vToEye = normalize( -vVertex );\n"
				"	vec3 vReflect = normalize( -reflect(vToLight, vNormal) );\n"
				"\n"
				"	// diffuse coefficient\n"
				"	vec3 diffuse = max( dot( vNormal, vToLight ), 0.0 ) * cDiffuse;\n"
				"\n"
				"	// specular coefficient with energy conservation\n"
				"	const float shininess = 20.0;\n"
				"	const float coeff = (2.0 + shininess) / (2.0 * 3.14159265);\n"
				"	vec3 specular = pow( max( dot( vReflect, vToEye ), 0.0 ), shininess ) * coeff * cSpecular;\n"
				"\n"
				"	// to conserve energy, diffuse and specular colors should not exceed one\n"
				"	float maxDiffuse = max(diffuse.r, max(diffuse.g, diffuse.b));\n"
				"	float maxSpecular = max(specular.r, max(specular.g, specular.b));\n"
				"	float fConserve = 1.0 / max(1.0, maxDiffuse + maxSpecular);\n"
				"\n"
				"	// final color\n"
				"	oColor.rgb = (diffuse + specular) * fConserve;\n"
				"	oColor.a = 1.0;\n"
				"}\n"
			)
		);
	}
	catch( const std::exception& e ) {
		console() << e.what() << std::endl;
	}
}

void GeometryApp::createWireframeShader(void)
{
	try {
		mWireframeShader = gl::GlslProg::create( gl::GlslProg::Format()
			.vertex(
				"#version 150\n"
				"\n"
				"uniform mat4	ciModelViewProjection;\n"
				"in vec4		ciPosition;\n"
				"in vec4		ciColor;\n"
				"in vec2		ciTexCoord0;\n"
				"\n"
				"out VertexData {\n"
				"	vec4 color;\n"
				"	vec2 texcoord;\n"
				"} vVertexOut;\n"
				"\n"
				"void main(void) {\n"
				"	vVertexOut.color = ciColor;\n"
				"	vVertexOut.texcoord = ciTexCoord0;\n"
				"	gl_Position = ciModelViewProjection * ciPosition;\n"
				"}\n"
			) 
			.geometry(
				"#version 150\n"
				"\n"
				"layout (triangles) in;\n"
				"layout (triangle_strip, max_vertices = 3) out;\n"
				"\n"
				"uniform vec2 			uViewportSize;\n"
				"\n"
				"in VertexData	{\n"
				"	vec4 color;\n"
				"	vec2 texcoord;\n"
				"} vVertexIn[];\n"
				"\n"
				"out VertexData	{\n"
				"	noperspective vec3 distance;\n"
				"	vec4 color;\n"
				"	vec2 texcoord;\n"
				"} vVertexOut;\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	// taken from 'Single-Pass Wireframe Rendering'\n"
				"	vec2 p0 = uViewportSize * gl_in[0].gl_Position.xy / gl_in[0].gl_Position.w;\n"
				"	vec2 p1 = uViewportSize * gl_in[1].gl_Position.xy / gl_in[1].gl_Position.w;\n"
				"	vec2 p2 = uViewportSize * gl_in[2].gl_Position.xy / gl_in[2].gl_Position.w;\n"
				"\n"
				"	vec2 v0 = p2-p1;\n"
				"	vec2 v1 = p2-p0;\n"
				"	vec2 v2 = p1-p0;\n"
				"	float fArea = abs(v1.x*v2.y - v1.y * v2.x);\n"
				"\n"
				"	vVertexOut.distance = vec3(fArea/length(v0),0,0);\n"
				"	vVertexOut.color = vVertexIn[0].color;\n"
				"	vVertexOut.texcoord = vVertexIn[0].texcoord;\n"
				"	gl_Position = gl_in[0].gl_Position;\n"
				"	EmitVertex();\n"
				"\n"
				"	vVertexOut.distance = vec3(0,fArea/length(v1),0);\n"
				"	vVertexOut.color = vVertexIn[1].color;\n"
				"	vVertexOut.texcoord = vVertexIn[1].texcoord;\n"
				"	gl_Position = gl_in[1].gl_Position;\n"
				"	EmitVertex();\n"
				"\n"
				"	vVertexOut.distance = vec3(0,0,fArea/length(v2));\n"
				"	vVertexOut.color = vVertexIn[2].color;\n"
				"	vVertexOut.texcoord = vVertexIn[2].texcoord;\n"
				"	gl_Position = gl_in[2].gl_Position;\n"
				"	EmitVertex();\n"
				"\n"
				"	EndPrimitive();\n"
				"}\n"
			) 
			.fragment(
				"#version 150\n"
				"\n"
				"uniform float uBrightness;\n"
				"\n"
				"in VertexData	{\n"
				"	noperspective vec3 distance;\n"
				"	vec4 color;\n"
				"	vec2 texcoord;\n"
				"} vVertexIn;\n"
				"\n"
				"out vec4				oColor;\n"
				"\n"
				"void main(void) {\n"
				"	// determine frag distance to closest edge\n"
				"	float fNearest = min(min(vVertexIn.distance[0],vVertexIn.distance[1]),vVertexIn.distance[2]);\n"
				"	float fEdgeIntensity = exp2(-1.0*fNearest*fNearest);\n"
				"\n"
				"	// blend between edge color and face color\n"
				"	vec3 vFaceColor = vVertexIn.color.rgb;\n"
				"	vec3 vEdgeColor = vec3(0.2, 0.2, 0.2);\n"
				"	oColor.rgb = mix(vFaceColor, vEdgeColor, fEdgeIntensity) * uBrightness;\n"
				"	oColor.a = 0.65;\n"
				"}\n"
			)
		);
	}
	catch( const std::exception& e ) {
		console() << e.what() << std::endl;
	}
}

CINDER_APP_NATIVE( GeometryApp, RendererGl )
