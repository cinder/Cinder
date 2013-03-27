#include "cinder/app/Renderer.h"
#include "cinder/Exception.h"
#include "cinder/app/AppImplMswRendererDx.h"
#include "cinder/ip/Flip.h"

namespace cinder { namespace app {

RendererDx::RendererDx( int aAntiAliasing )
	: Renderer(), mImpl( 0 ), mAntiAliasing( aAntiAliasing )
{
}

RendererDx::RendererDx( const RendererDx &renderer ) : mImpl(0), mWnd(renderer.mWnd)
{
}

RendererDx::~RendererDx()
{
	delete mImpl;
}


#if defined( CINDER_MSW )
void RendererDx::setup( App *aApp, HWND wnd, HDC dc )
{
	mWnd = wnd;
	mApp = aApp;
	if( ! mImpl )
		mImpl = new AppImplMswRendererDx( mApp, this );
	mImpl->initialize( wnd, dc );
}
#elif defined( CINDER_WINRT )
void RendererDx::setup( App *aApp, DX_WINDOW_TYPE wnd)
{
	mWnd = wnd;
	mApp = aApp;
	if( ! mImpl )
		mImpl = new AppImplMswRendererDx( mApp, this );
	mImpl->initialize( wnd);

	// enable Vertical Sync drawing on WinRT
	mImpl->enableVsync(TRUE);
}
#endif

void RendererDx::kill()
{
	mImpl->kill();
}

void RendererDx::prepareToggleFullScreen()
{
	mImpl->prepareToggleFullScreen();
}

void RendererDx::finishToggleFullScreen()
{
	mImpl->finishToggleFullScreen();
}

void RendererDx::startDraw()
{
	mImpl->makeCurrentContext();
}

void RendererDx::finishDraw()
{
	mImpl->swapBuffers();
}

void RendererDx::defaultResize()
{
	mImpl->defaultResize();
}

Surface	RendererDx::copyWindowSurface( const Area &area )
{
	Surface s( area.getWidth(), area.getHeight(), false );
	//glFlush(); // there is some disagreement about whether this is necessary, but ideally performance-conscious users will use FBOs anyway
	mImpl->mDeviceContext->Flush();
	//GLint oldPackAlignment;
	//glGetIntegerv( GL_PACK_ALIGNMENT, &oldPackAlignment ); 
	//glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	//glReadPixels( area.x1, mApp->getWindowHeight() - area.y2, area.getWidth(), area.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, s.getData() );
	//glPixelStorei( GL_PACK_ALIGNMENT, oldPackAlignment );	

	ip::flipVertical( &s );
	return s;
}

void RendererDx::makeCurrentContext()
{
	mImpl->makeCurrentContext();
}

} } //namespace cinder::app
