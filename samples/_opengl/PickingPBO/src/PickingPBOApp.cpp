#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Pbo.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Batch.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Utilities.h"
#include "cinder/CinderAssert.h"
#include "cinder/Log.h"
#include "cinder/params/Params.h"

// Define this to blow up the selection area that is picked.
//#define DEBUG_PICKING 1

using namespace ci;
using namespace ci::app;

class PickingPBOApp : public AppNative {
  public:
	virtual void setup() override;
	virtual void resize() override;
	virtual void draw() override;
	virtual void mouseDown( MouseEvent event ) override;
	virtual void mouseDrag( MouseEvent event ) override;

	int pick( const ivec2 &mousePos );
	void setupParams();
	void setupShader();
	void setupGeometry();
	void setupGrid( int xSize, int zSize, int spacing );
	void setupGradient();
	void setupFbo();
	void setupPbo();

	void renderScene();
	uint32_t colorToIndex( const ColorA8u &color );
	ColorA8u indexToColor( uint32_t index );
	vec4 colorToVec4( const ColorA8u &color );
	void setPickingColors( bool selectVertices, bool selectEdges );
	void restoreDefaultColors();
	void setSelectedColors( int selected );

	enum { CHANNEL_COUNT = 4 };

	gl::TextureRef		mTexture;
	gl::GlslProgRef		mGlsl;
	gl::VboMeshRef		mVerticesMesh;
	gl::VboMeshRef		mEdgesMesh;
	gl::VertBatchRef	mGridMesh;
	gl::BatchRef		mGradientMesh;
	gl::BatchRef		mVerticesBatch;
	gl::BatchRef		mEdgesBatch;

	gl::FboRef			mFbo;
	gl::PboRef			mPbo;
	gl::GlslProgRef		mPickableProg;
	gl::GlslProgRef		mNotPickableProg;
	int					mPickPixelSize;
	ivec2				mPickPos;
	MayaCamUI			mMayaCam;
	vec4				mDefaultVertexColor;
	vec4				mDefaultEdgeColor;
	vec4				mSelectedVertexColor;
	vec4				mSelectedEdgeColor;
	mat4				mBoxTransform;
	gl::TextureRef		mDebugTexture;
	float				mDebugDisplaySize;
	gl::Texture2dRef	mPickingTexture;
	bool				mNeedsRedraw;
	bool				mSelectVertices;
	bool				mSelectEdges;
	params::InterfaceGlRef		mParams;
};

void PickingPBOApp::setup()
{
	mDefaultVertexColor = vec4( 0.7f, 0.3f, 0.7f, 1.0f );
	mDefaultEdgeColor = vec4( 0.3f, 0.7f, 0.9f, 1.0f );
	mSelectedVertexColor = vec4( 0.7f, 0.7f, 0.3f, 1.0f );
	mSelectedEdgeColor = vec4( 0.9f, 0.7f, 0.3f, 1.0f );

	mSelectVertices = true;
	mSelectEdges = true;

	setupParams();
	setupShader();
	setupGradient();
	setupGrid( 200, 200, 10 );
	setupGeometry();
	setupPbo();
	setupFbo();

	CameraPersp cam( mMayaCam.getCamera() );
	cam.lookAt( vec3( 100, 100, 100 ), vec3( 0 ) );
	cam.setCenterOfInterestPoint( vec3( 0 ) );
	mMayaCam.setCurrentCam( cam );

	mBoxTransform = mat4( 1.0f );
	mBoxTransform = glm::scale( mBoxTransform, vec3( 30.0f ) );
	mBoxTransform = glm::translate( mBoxTransform, vec3( 0.0f, 0.5f, 0.0f ) );

	mNeedsRedraw = true;
	mDebugDisplaySize = 20.0f;

	gl::enableDepthWrite();
	gl::enableDepthRead();
}

void PickingPBOApp::resize()
{
	CameraPersp cam( mMayaCam.getCamera() );
	cam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	mMayaCam.setCurrentCam( cam );

	setupFbo();
	mNeedsRedraw = true;
}

void PickingPBOApp::draw()
{
	if( mNeedsRedraw ) {
		renderScene();
	}

	gl::clear();
	gl::setMatricesWindow( toPixels( getWindowSize() ) );
	gl::draw( mFbo->getColorTexture(), getWindowBounds() );
	mParams->draw();
}

void PickingPBOApp::renderScene()
{
	gl::ScopedFramebuffer scopedFbo( mFbo );
	gl::clear( ci::ColorA( 0.0f, 0.0f, 0.0f, 0.0f ) );

	gl::ScopedGlslProg glslScope( mNotPickableProg );
	gl::setDefaultShaderVars();

	// Draw gradient.
	gl::disableDepthWrite();
	gl::pushMatrices();
	gl::setMatricesWindow( 1, 1 );
	mGradientMesh->draw();
	gl::popMatrices();
	gl::enableDepthWrite();

	// Draw scene.
	gl::pointSize( 6.0f );
	gl::pushMatrices();
	gl::setMatrices( mMayaCam.getCamera() );
	mGridMesh->draw();
	gl::multModelMatrix( mBoxTransform );
	mEdgesBatch->draw();
	mVerticesBatch->draw();
	gl::popMatrices();

#if DEBUG_PICKING
	if( mDebugTexture ) {
		gl::ScopedTextureBind scopeTexture( mDebugTexture );
		gl::draw( mDebugTexture, ci::Rectf( mPickPos.x - mDebugDisplaySize, mPickPos.y - mDebugDisplaySize, mPickPos.x + mDebugDisplaySize, mPickPos.y + mDebugDisplaySize ) );
	}
#endif

	mNeedsRedraw = false;
}

void PickingPBOApp::mouseDown( MouseEvent event )
{
	restoreDefaultColors();
#if DEBUG_PICKING
	// Avoid selecting debug texture.
	float sqDisplaySize = mDebugDisplaySize * mDebugDisplaySize;
	if( glm::distance( static_cast<vec2>( mPickPos ), static_cast<vec2>( event.getPos() ) ) <= glm::sqrt( sqDisplaySize + sqDisplaySize ) ) {
		return;
	}
#endif

	mPickPos = event.getPos();

	int selection = pick( mPickPos );
	if( selection != -1 ) {
		setSelectedColors( selection );
	}
	else {
		mMayaCam.mouseDown( mPickPos );
	}
	mNeedsRedraw = true;
}

void PickingPBOApp::mouseDrag( MouseEvent event )
{
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
	mNeedsRedraw = true;
}

int PickingPBOApp::pick( const ivec2 &mousePos )
{
	gl::ScopedFramebuffer scopedFbo( mFbo );
	gl::readBuffer( GL_COLOR_ATTACHMENT1 );
	// Read the pbo containting the selection square.
	int selectedIndex = -1;
	int halfPickPixelSize = static_cast<int>(mPickPixelSize / 2);
	int numPixels = mPickPixelSize * mPickPixelSize;
	gl::ScopedBuffer pboScope( mPbo );
	gl::readPixels( mousePos.x - halfPickPixelSize, getWindowHeight() - mousePos.y - halfPickPixelSize, mPickPixelSize, mPickPixelSize, GL_BGRA, GL_UNSIGNED_BYTE, 0 );

	// Count the votes for color number in the square to determine the winner.
	GLubyte *pixelBuffer = static_cast<GLubyte *>(mPbo->map( GL_READ_ONLY ));
	CI_ASSERT( pixelBuffer != nullptr );
	int numBytes = numPixels * CHANNEL_COUNT;
	std::map<int, int> voteCount;
	int winner = 0;
	int winningCount = -1;
	for( int byteIdx = 0; byteIdx < numBytes; byteIdx += CHANNEL_COUNT ) {
		ci::ColorA8u color( pixelBuffer[byteIdx + 2], pixelBuffer[byteIdx + 1], pixelBuffer[byteIdx], 0 );
		int colorNumber = colorToIndex( color );
		if( colorNumber != 0 ) {
			if( voteCount.find( colorNumber ) == voteCount.end() ) {
				voteCount[colorNumber] = 1;
			}
			else {
				voteCount[colorNumber]++;
			}
			if( voteCount[colorNumber] > winningCount ) {
				winningCount = voteCount[colorNumber];
				winner = colorNumber;
			}
		}// end if
	}// end for

	if( winner != 0 ) {
		selectedIndex = winner - 1;
	}

#if DEBUG_PICKING
	for( int byteIdx = 0; byteIdx < numBytes; byteIdx += CHANNEL_COUNT ) {
		int colorNumber = colorToIndex(
			ci::ColorA8u( pixelBuffer[byteIdx + 2], pixelBuffer[byteIdx + 1], pixelBuffer[byteIdx], 0 ) );
		if( colorNumber == 0 ) {
			for( unsigned int chanIdx = 0; chanIdx < CHANNEL_COUNT; ++chanIdx ) {
				pixelBuffer[byteIdx + chanIdx] = 0xFF;
			}
		}
	}// end for
	mDebugTexture = gl::Texture::create( pixelBuffer, GL_BGRA, mPickPixelSize, mPickPixelSize );
#endif
	mPbo->unmap();

	return selectedIndex;
}

void PickingPBOApp::setupParams()
{
	mParams = params::InterfaceGl::create( "Settings", ivec2( 200, 60 ) );
	mParams->addParam( "Select Vertices", &mSelectVertices ).updateFn( [&] { setPickingColors( mSelectVertices, mSelectEdges ); renderScene(); } );
	mParams->addParam( "Select Edges", &mSelectEdges ).updateFn( [&] { setPickingColors( mSelectVertices, mSelectEdges ); renderScene(); } );
}

void PickingPBOApp::setupGeometry()
{
	// Setup wire frame cube.
	std::array<vec3, 8> vertexPositions = {
		vec3( 0.5f, -0.5f, -0.5f ),
		vec3( -0.5f, -0.5f, -0.5f ),
		vec3( -0.5f, -0.5f, 0.5f ),
		vec3( 0.5f, -0.5f, 0.5f ),
		vec3( 0.5f, 0.5f, -0.5f ),
		vec3( -0.5f, 0.5f, -0.5f ),
		vec3( -0.5f, 0.5f, 0.5f ),
		vec3( 0.5f, 0.5f, 0.5f )
	};

	std::array<GLushort, 24> indices = {
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7
	};

	std::vector<vec4> vertexColors( vertexPositions.size(), mDefaultVertexColor );

	std::vector<vec3> edgePositions;
	edgePositions.reserve( indices.size() );
	for( auto idx : indices ) {
		edgePositions.push_back( vertexPositions[idx] );
	}
	std::vector<vec4> edgeColors( edgePositions.size(), mDefaultEdgeColor );

	std::vector<vec4> edgePickingColors( edgePositions.size() );
	std::vector<vec4> vertexPickingColors( vertexPositions.size() );

	gl::VboMesh::Layout layout;
	layout.usage( GL_STATIC_DRAW ).attrib( geom::POSITION, 3 ).attrib( geom::COLOR, 4 ).attrib( geom::CUSTOM_0, 4 );
	geom::BufferLayout instanceDataLayout;
	instanceDataLayout.append( geom::Attrib::CUSTOM_0, 3, 0, 0, 1 /* per instance */ );

	mEdgesMesh = gl::VboMesh::create( edgePositions.size(), GL_LINES, { layout } );
	mVerticesMesh = gl::VboMesh::create( vertexPositions.size(), GL_POINTS, { layout } );
	mEdgesMesh->bufferAttrib( geom::POSITION, edgePositions.size() * sizeof( vec3 ), edgePositions.data() );
	mEdgesMesh->bufferAttrib( geom::COLOR, edgeColors );
	mVerticesMesh->bufferAttrib( geom::POSITION, vertexPositions.size() * sizeof( vec3 ), vertexPositions.data() );
	mVerticesMesh->bufferAttrib( geom::COLOR, vertexColors );

	gl::Batch::AttributeMapping mapping( { { geom::Attrib::CUSTOM_0, "pickingColor" } } );
	mVerticesBatch = gl::Batch::create( mVerticesMesh, mPickableProg, mapping );
	mEdgesBatch = gl::Batch::create( mEdgesMesh, mPickableProg, mapping );

	setPickingColors( mSelectVertices, mSelectEdges );
}

void PickingPBOApp::setupGrid( int xSize, int zSize, int spacing )
{
	CI_ASSERT( ( spacing <= xSize ) && ( spacing <= zSize ) );

	// Cut in half and adjust for spacing.
	xSize = ( ( xSize / 2 ) / spacing ) * spacing;
	zSize = ( ( zSize / 2 ) / spacing ) * spacing;

	const int xMax = xSize + spacing;
	const int zMax = zSize + spacing;
	const Colorf defaultColor( 0.75f, 0.75f, 0.75f );
	const Colorf black( 0, 0, 0 );

	mGridMesh = gl::VertBatch::create( GL_LINES );

	// Add x lines.
	for( int xVal = -xSize; xVal < xMax; xVal += spacing ) {
		if( xVal == 0 ) // Center line.
			mGridMesh->color( black );
		else
			mGridMesh->color( defaultColor );
		mGridMesh->vertex( (float)xVal, 0, (float)-zSize );
		mGridMesh->vertex( (float)xVal, 0, (float)zSize );
	}// end for each x dir line

	// Add z lines.
	for( int zVal = -zSize; zVal < zMax; zVal += spacing ) {
		if( zVal == 0 ) // Center line.
			mGridMesh->color( black );
		else 
			mGridMesh->color( defaultColor );
		
		mGridMesh->vertex( (float)xSize, 0, (float)zVal );
		mGridMesh->vertex( (float)-xSize, 0, (float)zVal );
	}// end for each z dir line
}

void PickingPBOApp::setupGradient()
{
	const Colorf topColor( 0.50f, 0.50f, 0.50f );
	const Colorf bottomColor( 0.25f, 0.25f, 0.25f );

	auto rectSource = geom::Rect( Rectf( 0, 0, 1, 1 ) ).colors( topColor, topColor, bottomColor, bottomColor );
	mGradientMesh = gl::Batch::create( rectSource, mNotPickableProg );
}

void PickingPBOApp::setupFbo()
{
	gl::Fbo::Format fmt;
	int width = getWindowWidth();
	int height = getWindowHeight();
	gl::Texture::Format texFmt;
	texFmt.internalFormat( GL_RGBA ).pixelDataFormat( GL_RGBA ).pixelDataType( GL_UNSIGNED_BYTE );
	mPickingTexture = gl::Texture2d::create( width, height, texFmt );
	fmt.attachment( GL_COLOR_ATTACHMENT1, mPickingTexture );
	mFbo = gl::Fbo::create( width, height, fmt );
}

void PickingPBOApp::setupPbo()
{
	mPickPixelSize = 6;
	int pboSize = mPickPixelSize * mPickPixelSize * CHANNEL_COUNT;
	mPbo = gl::Pbo::create( GL_PIXEL_PACK_BUFFER, pboSize );
}

void PickingPBOApp::setupShader()
{
	try
	{
		mPickableProg = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "pickable.vs.glsl" ) )
			.fragment( loadAsset( "pickable.fs.glsl" ) ).fragDataLocation( 0, "oColor" ).fragDataLocation( 1, "oPickingColor" ) );
	}
	catch( ci::gl::GlslProgCompileExc &exc )
	{
		CI_LOG_E( "Shader load error: " << exc.what() );
		shutdown();
	}
	catch( ci::Exception &exc )
	{
		CI_LOG_E( "Shader load error: " << exc.what() );
		shutdown();
	}

	try
	{
		mNotPickableProg = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "not_pickable.vs.glsl" ) )
			.fragment( loadAsset( "pickable.fs.glsl" ) ).fragDataLocation( 0, "oColor" ).fragDataLocation( 1, "oPickingColor" ) );
	}
	catch( ci::gl::GlslProgCompileExc &exc )
	{
		CI_LOG_E( "Shader load error: " << exc.what() );
		shutdown();
	}
	catch( ci::Exception &exc )
	{
		CI_LOG_E( "Shader load error: " << exc.what() );
		shutdown();
	}
}

uint32_t PickingPBOApp::colorToIndex( const ci::ColorA8u &color )
{
	return ((color.a << 24) | (color.r << 16) | (color.g << 8) | (color.b));
}

ci::ColorA8u PickingPBOApp::indexToColor( uint32_t index )
{
	return ci::ColorA8u( ( ( index >> 16 ) & 0xFF ), ( ( index >> 8 ) & 0xFF ), ( index & 0xFF ), ( ( index >> 24 ) & 0xFF ) );
}

vec4 PickingPBOApp::colorToVec4( const ColorA8u &color )
{
	return vec4( static_cast<float>( color.r ) / 255.0f,
		static_cast<float>( color.g ) / 255.0f,
		static_cast<float>( color.b ) / 255.0f,
		static_cast<float>( color.a ) / 255.0f );
}

void PickingPBOApp::setPickingColors( bool selectVertices, bool selectEdges )
{
	// Set ordered colors picking.
	uint32_t colorIdx = 1;
	const vec4 zeroVec4( 0 );

	// Handle edges.  Edges need the same color for both vertices that comprise the edge.
	uint32_t numEdgesColors = mEdgesMesh->getNumVertices();
	CI_ASSERT( (numEdgesColors % 2) == 0 );
	gl::VboMesh::MappedAttrib<vec4> edgeColorIter = mEdgesMesh->mapAttrib4f( geom::CUSTOM_0, false );
	for( uint32_t idx = 0; idx < numEdgesColors; idx += 2, ++colorIdx ) {
		vec4 color = selectEdges ? colorToVec4( indexToColor( colorIdx ) ) : zeroVec4;
		*edgeColorIter++ = color;
		*edgeColorIter++ = color;
	}
	edgeColorIter.unmap();

	if( ! selectEdges ) {
		colorIdx = 1;
	}

	if( selectVertices ) {
		// Handle vertices.
		uint32_t numVerticesColors = mVerticesMesh->getNumVertices();
		gl::VboMesh::MappedAttrib<vec4> vertexColorIter = mVerticesMesh->mapAttrib4f( geom::CUSTOM_0, false );
		for( uint32_t idx = 0; idx < numVerticesColors; ++idx, ++colorIdx ) {
			vec4 color = selectVertices ? colorToVec4( indexToColor( colorIdx ) ) : zeroVec4;
			*vertexColorIter++ = color;
		}
		vertexColorIter.unmap();
	}
}

void PickingPBOApp::restoreDefaultColors()
{
	// Set edges back to default color.
	uint32_t numEdgesColors = mEdgesMesh->getNumVertices();
	gl::VboMesh::MappedAttrib<vec4> edgeColorIter = mEdgesMesh->mapAttrib4f( geom::COLOR, false );
	for( uint32_t idx = 0; idx < numEdgesColors; ++idx ) {
		*edgeColorIter++ = mDefaultEdgeColor;
	}
	edgeColorIter.unmap();

	// Set vertices back to default color.
	uint32_t numVerticesColors = mVerticesMesh->getNumVertices();
	gl::VboMesh::MappedAttrib<vec4> vertexColorIter = mVerticesMesh->mapAttrib4f( geom::COLOR, false );
	for( uint32_t idx = 0; idx < numVerticesColors; ++idx ) {
		*vertexColorIter++ = mDefaultVertexColor;
	}
	vertexColorIter.unmap();
}

void PickingPBOApp::setSelectedColors( int selected )
{
	if( selected < 0 ) {
		return;
	}

	uint32_t numEdgeVertices = mEdgesMesh->getNumVertices();
	CI_ASSERT( ( numEdgeVertices % 2 ) == 0 );
	uint32_t numEdgeColors = numEdgeVertices / 2;

	if( mSelectEdges )	{
		if( static_cast<uint32_t>( selected ) < numEdgeColors ) {
			gl::VboMesh::MappedAttrib<vec4> edgeColorIter = mEdgesMesh->mapAttrib4f( geom::COLOR, false );
			uint32_t edgeIdx = selected * 2;
			edgeColorIter[edgeIdx] = mSelectedEdgeColor;
			edgeColorIter[edgeIdx + 1] = mSelectedEdgeColor;
			edgeColorIter.unmap();
			return;
		}
	}

	if( mSelectVertices ) {
		uint32_t numVerticesColors = mVerticesMesh->getNumVertices();
		if( mSelectEdges ) {
			CI_ASSERT( static_cast<uint32_t>( selected ) < ( numEdgeColors + numVerticesColors ) );
			selected -= numEdgeColors;
		}
		gl::VboMesh::MappedAttrib<vec4> vertexColorIter = mVerticesMesh->mapAttrib4f( geom::COLOR, false );
		vertexColorIter[selected] = mSelectedVertexColor;
		vertexColorIter.unmap();
	}
}

CINDER_APP_NATIVE( PickingPBOApp, RendererGl )
