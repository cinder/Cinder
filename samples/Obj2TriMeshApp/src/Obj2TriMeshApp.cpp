/*
 Copyright (c) 2011, Paul Houx
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

/*
 This sample shows how to load OBJ files and export them to binary data files that load much quicker.
 Drag-and-drop either an OBJ file or a binary file onto the window and check the output window to
 compare load times and marvel at the difference.

 See the fileDrop() method for the interesting bits of code.
*/

#include "cinder/Camera.h"
#include "cinder/MayaCamUI.h"
#include "cinder/ObjLoader.h"
#include "cinder/Utilities.h"
#include "cinder/app/AppBasic.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicApp : public AppBasic {
 public:
	void setup();
	void update();
	void draw();

	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void keyDown( KeyEvent event );
	void resize( ResizeEvent event );
	void fileDrop( FileDropEvent event );

	void drawGrid(int size=100, int step=10);
protected:
	MayaCamUI		mMayaCam;

	vector<TriMesh>	mMeshes;
};

void BasicApp::setup()
{
	mMeshes.clear();

	// set up the camera
	CameraPersp cam;
	cam.setEyePoint( Vec3f(50.0f, 50.0f, 100.0f) );
	cam.setCenterOfInterestPoint( Vec3f(0.0f, 0.0f, 0.0f) );
	cam.setPerspective( 45.0f, getWindowAspectRatio(), 1.0f, 1000.0f );
	mMayaCam.setCurrentCam( cam );
}

void BasicApp::update()
{
	// not used in this example
}

void BasicApp::draw()
{	
	vector<TriMesh>::iterator itr;

	// gray background
	gl::clear( Colorf(0.5f, 0.5f, 0.5f) );

	// set up the camera 
	gl::pushMatrices();
	gl::setMatrices( mMayaCam.getCamera() );

	// enable the depth buffer (after all, we are doing 3D)
	gl::enableDepthRead();
	gl::enableDepthWrite();

	// draw the grid on the floor
	drawGrid();

	// enable backface culling to improve frame rate
	glCullFace( GL_BACK );
	glEnable( GL_CULL_FACE );

	// draw objects as wireframes first
	gl::color( Color::white() );
	gl::enableWireframe();
		for(itr=mMeshes.begin();itr!=mMeshes.end();++itr) 
			gl::draw( *itr );
	gl::disableWireframe();

	// fill the polygons (hidden-line removal trick)
	gl::color( Color(0.5f, 0.5f, 0.5f) );
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);
	for(itr=mMeshes.begin();itr!=mMeshes.end();++itr) 
		gl::draw( *itr );
	glDisable(GL_POLYGON_OFFSET_FILL);

	// disable backface culling
	glDisable( GL_CULL_FACE );

	// restore matrices
	gl::popMatrices();
}

void BasicApp::mouseDown( MouseEvent event )
{	
	// let the camera handle the interaction
	mMayaCam.mouseDown( event.getPos() );
}

void BasicApp::mouseDrag( MouseEvent event )
{
	// let the camera handle the interaction
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}

void BasicApp::keyDown( KeyEvent event )
{
	switch( event.getCode() ) {
		case KeyEvent::KEY_ESCAPE:
			quit();
			break;
		case KeyEvent::KEY_F4:
			if( event.isAltDown() ) {
				quit();
			}
			break;
		case KeyEvent::KEY_DELETE:
			mMeshes.clear();
			break;
	}
}

void BasicApp::resize( ResizeEvent event )
{
	// adjust aspect ratio
	CameraPersp cam = mMayaCam.getCamera();
	cam.setAspectRatio( getWindowAspectRatio() );
	mMayaCam.setCurrentCam( cam );
}

void BasicApp::fileDrop( FileDropEvent event )
{
	vector<string>	files = event.getFiles();
	vector<string>::iterator	itr;

	for(itr=files.begin();itr!=files.end();++itr)
	{
		double		t;
		string		path =  getPathDirectory( expandPath(*itr) );
		string		filename = getPathFileName(*itr);

		// clear current meshes
		mMeshes.clear();

		console() << "-------------" << std::endl;
		
		try {
			// first: try to load it as a TriMesh data file (which is quite fast)
			//console() << "Loading binary data file..." << std::endl;
			t = getElapsedSeconds();
			TriMesh	mesh;
			mesh.read( DataSourcePath::createRef( path + filename ) );
			mMeshes.push_back( mesh );
			console() << "Mesh data file loaded in " << (getElapsedSeconds() - t) << " seconds" << std::endl;
		}
		catch(...) {
			// if that didn't work, try to load it as an OBJ file (which can be very slow)
			try	{				
				// load OBJ file
				console() << "Loading OBJ file..." << std::endl;
				t = getElapsedSeconds();	
				ObjLoader loader( DataSourcePath::createRef( path + filename ) );
				double tObj = (getElapsedSeconds() - t);
				console() << "OBJ data file loaded in " << tObj << " seconds" << std::endl;

				// export each mesh group to a binary file
				console() << "Exporting mesh groups to separate files that load faster..." << std::endl;
				loader.writeGroups( path, true );
				console() << "Export complete" << std::endl;

				// we could create the TriMesh now, but for demonstration purposes we will load
				// the generated TriMeshes from their files
				//TriMesh mesh;
				//loader.load( &mesh, true, true, true );
				//mMeshes.push_back( mesh );

				console() << "Loading this model as separate TriMesh files..." << std::endl;
				t = getElapsedSeconds();	
				size_t numGroups = loader.getNumGroups();
				for(size_t i=0;i<numGroups;++i) {
					string name = loader.getGroupName(i);

					TriMesh	mesh;
					mesh.read( DataSourcePath::createRef( path + name + ".msh" ) );
					mMeshes.push_back( mesh );
				}
				double tMesh = (getElapsedSeconds() - t);
				console() << "Loading as separate TriMesh files took " << tMesh << " seconds" << std::endl;
				console() << "Compared to the ObjLoader, this is " << (int)(tObj / tMesh) << " times faster" << std::endl;
			}
			catch(...){}
		}
	}	
}

void BasicApp::drawGrid(int size, int step)
{
	gl::color( Color::white() );
	for(int i=-size;i<=size;i+=step) {
		gl::drawLine( Vec3f((float)i, 0.0f, -1.0f * (float)size), Vec3f((float)i, 0.0f, (float)size) );
		gl::drawLine( Vec3f(-1.0f * (float)size, 0.0f, (float)i), Vec3f((float)size, 0.0f, (float)i) );
	}
}

CINDER_APP_BASIC( BasicApp, RendererGl )