/*
 * Code Copyright 2011 Robert Hodgin ( http://roberthodgin.com ) and Andrew Bell ( http://drawnline.net )
 * Used with permission for the Cinder Project ( http://libcinder.org )
 */

#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/TextureFont.h"

#include <list>
#include <algorithm>

#include "Resources.h"
#include "WordNode.h"
#include "Dictionary.h"
#include "CenterState.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class VisualDictionaryApp : public AppBasic {
  public:
	void prepareSettings( Settings *settings );
	void layoutWords( vector<string> words, float radius );	
	void setup();
	void initialize();
	void enableSelections() { mEnableSelections = true; }
	void mouseMove( MouseEvent event );	
	void mouseDown( MouseEvent event );
	void keyDown( KeyEvent event );
	float getLayoutRadius(){ return getWindowHeight() * 0.415f; }

	void selectNode( list<WordNode>::iterator selectedWord );

	void update();
	void draw();

	list<WordNode>::iterator	getNodeAtPoint( const Vec2f &point );
	
	shared_ptr<Dictionary>		mDictionary;
	list<WordNode>				mNodes, mDyingNodes;
	list<WordNode>::iterator	mMouseOverNode;
	
	CenterState					mCenterState;

	gl::Texture					mBgTex;
	gl::Texture					mCircleTex;
	gl::Texture					mSmallCircleTex;
	
	bool						mEnableSelections;
	WordNode					mCurrentNode;
	float						mCurrentCircleRadius;
};

void VisualDictionaryApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 800, 800 );
}

void VisualDictionaryApp::layoutWords( vector<string> words, float radius )
{
	int radiusDivisor = 26;//std::max<int>( 10, words.size() ); // don't let the circles get too small
	mCurrentCircleRadius = radius / radiusDivisor * M_PI;
	for( size_t w = 0; w < words.size(); ++w ) {
		int wordLength	= words[w].length();
		string s		= words[w];
		int charIndex	= (int)s[wordLength-1] - 97;
		float charPer	= charIndex/26.0f;
		float angle		= charPer * 2.0f * M_PI;
		//float angle = w / (float)words.size() * 2 * M_PI;
		Vec2f pos = getWindowCenter() + radius * Vec2f( cos( angle ), sin( angle ) );
		Color col(  CM_HSV, charPer, 0.875f, 1 );
		mNodes.push_back( WordNode( words[w] ) );
		mNodes.back().mPos = getWindowCenter() + radius * 0.5f * Vec2f( cos( angle ), sin( angle ) );
		mNodes.back().mColor = ColorA( col, 0.0f );
		mNodes.back().mRadiusDest = mCurrentCircleRadius;
		mNodes.back().mRadius = 0;
		
		timeline().apply( &mNodes.back().mRadius, mNodes.back().mRadiusDest, 0.4f, EaseOutAtan( 10 ) ).timelineEnd( -0.39f );
		timeline().apply( &mNodes.back().mPos, pos, 0.4f, EaseOutAtan( 10 ) ).timelineEnd( -0.39f );
		timeline().apply( &mNodes.back().mColor, ColorA( col, 1.0f ), 0.4f, EaseOutAtan( 10 ) ).timelineEnd( -0.39f );
	}
}

void VisualDictionaryApp::setup()
{
	mCenterState = CenterState( 140.0f );
	
	// load textures
	mBgTex = gl::Texture( loadImage( loadResource( RES_BACKGROUND_IMAGE ) ) );
	gl::Texture::Format fmt;
	fmt.enableMipmapping();
	mCircleTex = gl::Texture( loadImage( loadResource( RES_CIRCLE_IMAGE ) ), fmt );
	mSmallCircleTex = gl::Texture( loadImage( loadResource( RES_SMALL_CIRCLE_IMAGE ) ), fmt );
	
	// load the dictionary
	mDictionary = shared_ptr<Dictionary>( new Dictionary( loadResource( RES_DICTIONARY ) ) );

	// give the WordNodes their font
	WordNode::setFont( gl::TextureFont::create( Font( loadResource( RES_FONT ), 34 ), gl::TextureFont::Format().enableMipmapping( true ) ) );

	// give CenterState its font
	CenterState::setFont( gl::TextureFont::create( Font( loadResource( RES_FONT ), 150 ), gl::TextureFont::Format().enableMipmapping( true ) ) );
	
	initialize();
}

void VisualDictionaryApp::initialize()
{
	mNodes.clear();
	// make the first 26 nodes, one for each letter
	vector<string> initialWords;
	for( char c = 0; c < 26; ++c )
		initialWords.push_back( string( 1, (char)('a' + c) ) );
	
	layoutWords( initialWords, getLayoutRadius() );
	
	mCenterState.mCircles.clear();
	mCenterState.setWord( "" );
	
	// mark our currently highlighted node as "none"
	mMouseOverNode = mNodes.end();
	
	mEnableSelections = true;
}

list<WordNode>::iterator VisualDictionaryApp::getNodeAtPoint( const Vec2f &point )
{
	for( list<WordNode>::iterator nodeIt = mNodes.begin(); nodeIt != mNodes.end(); ++nodeIt ) {
		if( nodeIt->isPointInside( point ) )
			return nodeIt;
	}
	
	return mNodes.end();
}


void VisualDictionaryApp::keyDown( KeyEvent event )
{
	if( ! mEnableSelections )
		return;
	
	if( isalpha( event.getChar() ) ){
		// see if we can find a word that ends with this letter
		list<WordNode>::iterator foundWord = mNodes.end();
		for( foundWord = mNodes.begin(); foundWord != mNodes.end(); ++foundWord ) {
			if( foundWord->getWord()[foundWord->getWord().size()-1] == event.getChar() )
				break;
		}
		
		if( foundWord != mNodes.end() )
			selectNode( foundWord );
	} else {
		if( event.getCode() == KeyEvent::KEY_BACKSPACE ){
			initialize();
		}
	}
}

void VisualDictionaryApp::mouseDown( MouseEvent event )
{
	list<WordNode>::iterator clickedNode = getNodeAtPoint( event.getPos() );
	if( clickedNode != mNodes.end() ){
		selectNode( clickedNode );
	} else {
		if( ( event.getPos() - getWindowCenter() ).length() < 180.0f )
			initialize();
	}
}


void VisualDictionaryApp::mouseMove( MouseEvent event )
{
	if( ! mEnableSelections )
		return;
	
	list<WordNode>::iterator currentMouseOver = getNodeAtPoint( event.getPos() );

	if( currentMouseOver != mMouseOverNode ) {
		mMouseOverNode = currentMouseOver;
		
		// make all the circles not moused-over normal size, and the mouse-over big
		for( list<WordNode>::iterator nodeIt = mNodes.begin(); nodeIt != mNodes.end(); ++nodeIt ) {
			if( mMouseOverNode == nodeIt ){
				timeline().apply( &nodeIt->mRadius, mCurrentCircleRadius * 1.35f, 0.25f, EaseOutElastic( 2.0f, 1.2f ) );
			} else {
				timeline().apply( &nodeIt->mRadius, mCurrentCircleRadius, 0.5f, EaseOutAtan( 10 ) );
			}
		}
	}
}

void VisualDictionaryApp::selectNode( list<WordNode>::iterator selectedNode )
{
	// have all the nodes but this one disappear
	for( list<WordNode>::iterator nodeIt = mNodes.begin(); nodeIt != mNodes.end(); ++nodeIt ) {
		if( nodeIt != selectedNode ) {
			// copy this node to dying nodes and erase it from the current
			mDyingNodes.push_back( *nodeIt );
			timeline().apply( &mDyingNodes.back().mRadius, 0.0f, 0.5f, EaseInQuint() )
				.finishFn( bind( &WordNode::setShouldBeDeleted, &(mDyingNodes.back()) ) ); // when you're done, mark yourself for deletion
		}
	}
	
	mCurrentNode = *selectedNode;
	mCurrentNode.setSelected();
	mNodes.clear();
	
	Color c = Color( mCurrentNode.mColor().r, mCurrentNode.mColor().g, mCurrentNode.mColor().b );
	Vec2f dirToCenter = ( mCurrentNode.mPos() - getWindowCenter() ) * 0.5f;
	
	timeline().add( bind( &CenterState::addCircle, &mCenterState, mCurrentNode.getWord(), c, dirToCenter * 0.2f ), timeline().getCurrentTime() + 0.25f );

	// move the selected node towards the center
	
	timeline().apply( &mCurrentNode.mPos, getWindowCenter() + dirToCenter, 0.3f, EaseInQuint() );
	timeline().apply( &mCurrentNode.mColor, ColorA( mCurrentNode.mColor().r, mCurrentNode.mColor().g, mCurrentNode.mColor().b, 0.0f ), 0.3f, EaseInAtan( 10 ) );

	// now add all the descendants of the clicked node
	vector<string> children( mDictionary->getDescendants( mCurrentNode.getWord() ) );
	layoutWords( children, getLayoutRadius() );
	
	// mark our currently highlighted node as "none"
	mMouseOverNode = mNodes.end();
	
	// once everything is done animating, then we can allow selections, but for now, disable them
	mEnableSelections = false;
	std::function<void()> cueAction = bind( &VisualDictionaryApp::enableSelections, this );
	timeline().add( cueAction, timeline().getEndTime() - 2.0f );
}

void VisualDictionaryApp::update()
{
	if( mDictionary->isCompleteWord( mCurrentNode.getWord() ) ){
		mCenterState.update( mCurrentNode );
	}
	
	// erase any nodes which have been marked as ready to be deleted
	mDyingNodes.remove_if( bind( &WordNode::shouldBeDeleted, std::placeholders::_1 ) );
}

void VisualDictionaryApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::enableAlphaBlending();
	
	// draw background image
	gl::color( Color::white() );
	mBgTex.enableAndBind();
	gl::drawSolidRect( getWindowBounds() );
	
	
	mCircleTex.bind();
	
	// draw the center circles
	mCenterState.draw();
	
	// draw the dying nodes
	mSmallCircleTex.bind();
	for( list<WordNode>::const_iterator nodeIt = mDyingNodes.begin(); nodeIt != mDyingNodes.end(); ++nodeIt )
		nodeIt->draw();
	
	// draw all the non-mouseOver nodes
	for( list<WordNode>::const_iterator nodeIt = mNodes.begin(); nodeIt != mNodes.end(); ++nodeIt ){
		if( nodeIt != mMouseOverNode )
			nodeIt->draw();
	}
	
	// if there is a mouseOverNode, draw it last so it is 'above' the non-mouseOver nodes
	if( mMouseOverNode != mNodes.end() )
		mMouseOverNode->draw();
	
	// if there is a currentNode (previously selected), draw it
	if( ! mCurrentNode.getWord().empty() )
		mCurrentNode.draw();
	
	mSmallCircleTex.disable();
}


CINDER_APP_BASIC( VisualDictionaryApp, RendererGl )
