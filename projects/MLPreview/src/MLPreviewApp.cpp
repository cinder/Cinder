#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/MayaCamUI.h"
#include "cinder/qtime/QuickTime.h"
#include "AssimpLoader.h"
#include "ciUI.h"
#include "QLogger.h"

using namespace ci;
using namespace ci::app;
using namespace std;


#define GUI_SETTINGS_FILENAME   "MLPreviewGuiSettings.xml"
string QLogger::APP_LOGS_PATH;


class MLPreviewApp : public AppBasic {

public:
    
    void shutDown();
    void prepareSettings( Settings *settings );
	void setup();
	
    void mouseDown( MouseEvent event );
    void mouseDrag( MouseEvent event );
    void resize( ResizeEvent event );
    
	void update();
	void draw();
    
    void initGui();
    void guiEvent( ciUIEvent *event );

    void fileDrop( FileDropEvent event );
    
    void loadVideo( fs::path filePath );
    void loadModel( fs::path filePath );
    
    ciUICanvas              *mGui;
    Rectf                   mGuiRect;
    
    bool                            mEnableWireframe;
    
    mndl::assimp::AssimpLoader      mAssimpLoader;
    ci::MayaCamUI                   mMayaCam;
    // camera(array), 3dmodel, fbo, moviewriter, movieplayer
    
    
    gl::Texture             mMovieTex;
    qtime::MovieGl			mMovie;
    float                   mMovieTimeNorm;
};


void MLPreviewApp::shutDown()
{
    if ( mGui )
        delete mGui;
}


void MLPreviewApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 1600, 900 );
    //    settings->setFrameRate( 25.0f );
    settings->setResizable(false);
}


void MLPreviewApp::setup()
{
    // Logger
	QLogger::APP_LOGS_PATH = getAssetPath("logs").generic_string();
    QLogger::init();
    QLogger::log("App started.");
    
    loadModel( getAssetPath( "default.obj" ) );
    loadVideo( getAssetPath( "default.mov" ) );
    
    CameraPersp initialCam;
	initialCam.setPerspective( 45.0f, getWindowAspectRatio(), 0.1, 10000 );
	mMayaCam.setCurrentCam( initialCam );
    
    initGui();
}


void MLPreviewApp::mouseDown( MouseEvent event )
{
    if ( event.isAltDown() )
        mMayaCam.mouseDown( event.getPos() );
}

void MLPreviewApp::mouseDrag( MouseEvent event )
{
    if ( event.isAltDown() )
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}


void MLPreviewApp::resize( ResizeEvent event )
{
	CameraPersp cam = mMayaCam.getCamera();
	cam.setAspectRatio( getWindowAspectRatio() );
	mMayaCam.setCurrentCam( cam );
}


void MLPreviewApp::update()
{
    if ( mMovie && mMovie.checkNewFrame() )
    {
        mMovieTex = mMovie.getTexture();
    }
    
    mGui->update();
}


void MLPreviewApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::setMatrices( mMayaCam.getCamera() );
    
//	mAssimpLoader.draw();
    //////////////////////////////////////////////////////
    
	gl::enableDepthWrite();
	gl::enableDepthRead();
    
	gl::color( Color::white() );
    
	if ( mEnableWireframe )
		gl::enableWireframe();
    
//	mAssimpLoader.draw();
    
    if ( mMovieTex )
        mMovieTex.enableAndBind();
    
    for( size_t k=0; k < mAssimpLoader.getNumMeshes(); k++ )
        gl::draw( mAssimpLoader.getMesh(k) );

    if ( mMovieTex )
        mMovieTex.disable();
    
	if ( mEnableWireframe )
		gl::disableWireframe();
    
    //////////////////////////////////////////////////////
    
    
    gl::setMatricesWindow( getWindowSize() );
    
    if ( mMovieTex )
        gl::draw( mMovieTex );
    
    mGui->draw();
}


void MLPreviewApp::initGui()
{
    mGuiRect        = Rectf( getWindowWidth() - 212, 0, getWindowWidth(), getWindowHeight() );
    float w         = mGuiRect.getWidth() - CI_UI_GLOBAL_WIDGET_SPACING * 2;
//    float half_w    = ( w - CI_UI_GLOBAL_WIDGET_SPACING - CI_UI_GLOBAL_PADDING ) / 2.0f;
    float h         = 10;
//    ciUIWidget          *widget;
    
    mGui = new ciUICanvas( mGuiRect.getX1(), mGuiRect.getY1(), mGuiRect.getWidth(), mGuiRect.getHeight() );
    
    //    mGui->setFont( RES_GUI_FONT );
    mGui->setFontSize( CI_UI_FONT_LARGE, 16 );
    mGui->setFontSize( CI_UI_FONT_MEDIUM, 14 );
    mGui->setFontSize( CI_UI_FONT_SMALL, 11 );
    
    
    mGui->addWidgetDown( new ciUIFPSSlider( w, h, 0, 120.00, 0.50, "FPS" ) );
    
    mGui->addWidgetDown( new ciUIToggle( h, h, &mEnableWireframe, "Wireframe", CI_UI_FONT_SMALL ) );
    
    //    mGui->addWidgetDown( new ciUISpacer(w, 1) );
    //    mGui->addWidgetDown( new ciUILabel("MOVIE PLAYBACK", CI_UI_FONT_MEDIUM) );
    mGui->registerUIEvents(this, &MLPreviewApp::guiEvent);
    
}


void MLPreviewApp::guiEvent( ciUIEvent *event )
{
    string name = event->widget->getName();
//    string meta = event->widget->getMeta();
}



void MLPreviewApp::fileDrop( FileDropEvent event )
{
    console() << "file drop: " << event.getFile(0) << endl;

    fs::path filePath = event.getFile(0);

    // load 3d model
    if ( filePath.extension() == ".obj" )
    {
        loadModel( filePath );
    }
    
    // load video
    else if ( filePath.extension() == ".mov" )
    {
        loadVideo( filePath );
        console() << "movie" << endl;
    }
    
    // load cameras txt
    else if ( filePath.extension() == ".txt" )
    {
        console() << "camera" << endl;
    }
}


void MLPreviewApp::loadModel( fs::path filePath )
{
    try
    {
        mAssimpLoader = mndl::assimp::AssimpLoader( filePath );
        mAssimpLoader.enableTextures( true );
    }
    catch ( ... )
    {
        QLogger::log( "Error while loading 3d model: " + filePath.generic_string() );
    }
    
    // generate thumbnail
}


void MLPreviewApp::loadVideo( fs::path filePath )
{
    mMovie = qtime::MovieGl( filePath );
    mMovie.setLoop();
    mMovie.play();
    
    mMovieTex = mMovie.getTexture();
    
    while( !mMovieTex )
        mMovieTex = mMovie.getTexture();
}


CINDER_APP_BASIC( MLPreviewApp, RendererGl )


