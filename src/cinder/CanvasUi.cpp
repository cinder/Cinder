/*
 Copyright (c) 2025, The Cinder Project, All rights reserved.
 Portions copyright Paul Houx.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include "cinder/CanvasUi.h"
#include "cinder/CinderAssert.h"
#include "cinder/Log.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

namespace cinder {

CanvasUi::CanvasUi()
{
	// Set default hotkeys
	using namespace app;
#if defined( CINDER_MSW ) || defined( CINDER_MAC )
	// On Windows, use ACCEL_DOWN (which maps to CTRL_DOWN)
	unsigned int modifier = KeyEvent::ACCEL_DOWN;
#else
	// On Linux, use CTRL_DOWN directly for consistency with Windows behavior
	unsigned int modifier = KeyEvent::CTRL_DOWN;
#endif
	mHotkeysZoomIn.emplace_back( KeyEvent::KEY_PLUS, modifier );
	mHotkeysZoomIn.emplace_back( KeyEvent::KEY_EQUALS, modifier );
	mHotkeysZoomIn.emplace_back( KeyEvent::KEY_KP_PLUS, modifier );

	mHotkeysZoomOut.emplace_back( KeyEvent::KEY_MINUS, modifier );
	mHotkeysZoomOut.emplace_back( KeyEvent::KEY_KP_MINUS, modifier );

	mHotkeysReset.emplace_back( KeyEvent::KEY_0, modifier );
	mHotkeysReset.emplace_back( KeyEvent::KEY_KP0, modifier );

	mHotkeysOneToOne.emplace_back( KeyEvent::KEY_1, modifier );
	mHotkeysOneToOne.emplace_back( KeyEvent::KEY_KP1, modifier );
	
	// Set default pan mouse buttons: Alt+Left, Middle, or Right mouse
	mPanMouseButtons.emplace_back( app::MouseEvent::LEFT_DOWN, app::MouseEvent::ALT_DOWN );
	mPanMouseButtons.emplace_back( app::MouseEvent::MIDDLE_DOWN, 0 );
	mPanMouseButtons.emplace_back( app::MouseEvent::RIGHT_DOWN, 0 );
}

CanvasUi::CanvasUi( const ci::app::WindowRef& window, int signalPriority )
	: CanvasUi()
{
	connect( window, signalPriority );
}

CanvasUi::CanvasUi( const app::WindowRef& window, const Rectf& bounds, int signalPriority )
	: CanvasUi( window, signalPriority )
{
	setContentBounds( bounds );
}

CanvasUi::~CanvasUi()
{
	disconnect();
}

void CanvasUi::setContentBounds( const Rectf& bounds )
{
	mCanvasBounds = bounds;
	if( isBounded() )
		constrainPosition();
}

const mat4& CanvasUi::getModelMatrix() const
{
	if( mIsDirty ) {
		updateMatrices();
		mIsDirty = false;
	}
	return mModelMatrix;
}

const mat4& CanvasUi::getInverseModelMatrix() const
{
	if( mIsDirty ) {
		updateMatrices();
		mIsDirty = false;
	}
	return mInverseModelMatrix;
}

void CanvasUi::updateMatrices() const
{
	// Transform: Window = Translate(viewport_offset + position) * Scale(zoom) * Canvas
	vec2 viewportOffset = mViewport.getUpperLeft();
	mModelMatrix = translate( vec3( viewportOffset + mPosition, 0 ) );
	mModelMatrix *= scale( vec3( mZoom ) );

	mInverseModelMatrix = inverse( mModelMatrix );
}

vec2 CanvasUi::toContent( const vec2& posWindow ) const
{
	// Inverse model matrix now handles full window-to-content transformation (includes viewport offset)
	return vec2( getInverseModelMatrix() * vec4( posWindow, 0, 1 ) );
}

vec2 CanvasUi::toWindow( const vec2& posContent ) const
{
	// Convert from content directly to window coordinates (model matrix now includes viewport offset)
	return vec2( getModelMatrix() * vec4( posContent, 0, 1 ) );
}

Rectf CanvasUi::getVisibleRect() const
{
	// Return the visible area of the viewport
	return getVisibleRect( Rectf( vec2( 0 ), mViewport.getSize() ) );
}

Rectf CanvasUi::getVisibleRect( const Rectf& rectViewport ) const
{
	// Convert viewport rect to window coordinates first
	Rectf windowRect = rectViewport + mViewport.getUpperLeft();
	// Convert the corners to content coordinates
	return Rectf{ toContent( windowRect.getUpperLeft() ), toContent( windowRect.getLowerRight() ) };
}

void CanvasUi::panBy( const vec2& deltaViewport )
{
	vec2 newPosition = mPosition + deltaViewport;
	if( isBounded() && ! mAllowPanOutside ) {
		newPosition = calculateConstrainedPosition( newPosition );
	}

	if( newPosition != mPosition ) {
		mPosition = newPosition;
		mIsDirty = true;
	}
}

void CanvasUi::panToViewportPos( const vec2& posViewport, bool disableAnimation )
{
	vec2 newPosition = posViewport;
	if( isBounded() && ! mAllowPanOutside ) {
		newPosition = calculateConstrainedPosition( newPosition );
	}

	if( newPosition != mPosition ) {
		if( mAnimationEnabled && ! disableAnimation ) {
			startPositionAnimation( newPosition );
		}
		else {
			mPosition = newPosition;
			mIsDirty = true;
		}
	}
}

void CanvasUi::panToContentPos( const vec2& posContent, bool disableAnimation )
{
	// Convert content coordinates to the position needed to center that content in the viewport
	vec2 viewportCenter = mViewport.getSize() * 0.5f;
	vec2 newPositionViewport = viewportCenter - posContent * mZoom;

	panToViewportPos( newPositionViewport, disableAnimation );
}

void CanvasUi::zoomBy( float factor, bool disableAnimation )
{
	// Zoom around viewport center
	vec2 viewportCenter = mViewport.getUpperLeft() + mViewport.getSize() * 0.5f;
	zoomBy( factor, viewportCenter, disableAnimation );
}

void CanvasUi::zoomBy( float factor, const vec2& anchorWindow, bool disableAnimation )
{
	if( factor <= 0.0f )
		return;

	float newZoom = glm::clamp( mZoom * factor, getEffectiveMinZoom(), mMaxZoom );
	if( newZoom == mZoom )
		return;

	// Convert provided window anchor to viewport-local coordinates
	vec2 anchorViewport = anchorWindow - mViewport.getUpperLeft();

	if( mAnimationEnabled && ! disableAnimation ) {
		// Use animation system
		startZoomAnimation( factor, anchorViewport );
	}
	else {
		// Immediate zoom using existing step function
		zoomByStep( factor, anchorViewport );
	}
}

void CanvasUi::zoomTo( float zoom, bool disableAnimation )
{
	// Zoom around viewport center
	vec2 viewportCenter = mViewport.getUpperLeft() + mViewport.getSize() * 0.5f;
	zoomTo( zoom, viewportCenter, disableAnimation );
}

void CanvasUi::zoomTo( float zoom, const vec2& anchorWindow, bool disableAnimation )
{
	if( zoom <= 0.0f )
		return;
	zoomBy( zoom / mZoom, anchorWindow, disableAnimation );
}

void CanvasUi::constrainPosition()
{
	if( ! isBounded() || mAllowPanOutside )
		return;

	vec2 constrainedPos = calculateConstrainedPosition( mPosition );
	if( constrainedPos != mPosition ) {
		mPosition = constrainedPos;
		mIsDirty = true;
	}
}

vec2 CanvasUi::calculateConstrainedPosition( const vec2& desiredPos ) const
{
	return calculateConstrainedPosition( desiredPos, mZoom );
}

vec2 CanvasUi::calculateConstrainedPosition( const vec2& desiredPos, float zoom ) const
{
	if( ! isBounded() || mAllowPanOutside ) {
		return desiredPos;
	}

	// Calculate canvas bounds in viewport space
	vec2 canvasMin = mCanvasBounds->getUpperLeft() * zoom + desiredPos;
	vec2 canvasMax = mCanvasBounds->getLowerRight() * zoom + desiredPos;
	vec2 viewportSize = mViewport.getSize();

	vec2 constrainedPos = desiredPos;

	// If canvas is smaller than viewport, center it
	if( canvasMax.x - canvasMin.x < viewportSize.x ) {
		constrainedPos.x = ( viewportSize.x - ( canvasMax.x - canvasMin.x ) ) * 0.5f - mCanvasBounds->getUpperLeft().x * zoom;
	}
	else {
		// Canvas is larger than viewport, constrain edges
		if( canvasMin.x > 0 ) {
			constrainedPos.x = -mCanvasBounds->getUpperLeft().x * zoom;
		}
		if( canvasMax.x < viewportSize.x ) {
			constrainedPos.x = viewportSize.x - mCanvasBounds->getLowerRight().x * zoom;
		}
	}

	// Same logic for Y
	if( canvasMax.y - canvasMin.y < viewportSize.y ) {
		constrainedPos.y = ( viewportSize.y - ( canvasMax.y - canvasMin.y ) ) * 0.5f - mCanvasBounds->getUpperLeft().y * zoom;
	}
	else {
		if( canvasMin.y > 0 ) {
			constrainedPos.y = -mCanvasBounds->getUpperLeft().y * zoom;
		}
		if( canvasMax.y < viewportSize.y ) {
			constrainedPos.y = viewportSize.y - mCanvasBounds->getLowerRight().y * zoom;
		}
	}

	return constrainedPos;
}


void CanvasUi::zoomIn( bool disableAnimation )
{
	vec2 centerViewport = mViewport.getSize() * 0.5f; // Center of viewport in viewport-local coordinates

	if( mAnimationEnabled && ! disableAnimation ) {
		// Start smooth animation from center (expects viewport-local coordinates)
		startZoomAnimation( 1.0f + mZoomFactor, centerViewport );
	}
	else {
		// Immediate zoom (expects viewport-local coordinates)
		zoomByStep( 1.0f + mZoomFactor, centerViewport );
	}
}

void CanvasUi::zoomOut( bool disableAnimation )
{
	vec2 centerViewport = mViewport.getSize() * 0.5f; // Center of viewport in viewport-local coordinates

	if( mAnimationEnabled && ! disableAnimation ) {
		// Start smooth animation from center (expects viewport-local coordinates)
		startZoomAnimation( 1.0f / ( 1.0f + mZoomFactor ), centerViewport );
	}
	else {
		// Immediate zoom (expects viewport-local coordinates)
		zoomByStep( 1.0f / ( 1.0f + mZoomFactor ), centerViewport );
	}
}

void CanvasUi::viewOnetoOne( bool disableAnimation )
{
	vec2 centerViewport = mViewport.getSize() * 0.5f; // Center of viewport in viewport-local coordinates

	// Calculate the zoom factor needed to reach 1.0
	float zoomFactor = 1.0f / mZoom;

	if( mAnimationEnabled && ! disableAnimation ) {
		startZoomAnimation( zoomFactor, centerViewport );
	} else {
		zoomByStep( zoomFactor, centerViewport );
	}
}

// Helper to set zoom and position with proper constraints
void CanvasUi::setZoomAndPosition( float newZoom, const vec2& newPositionViewport )
{
	mZoom = glm::clamp( newZoom, getEffectiveMinZoom(), mMaxZoom );

	if( isBounded() && ! mAllowPanOutside ) {
		mPosition = calculateConstrainedPosition( newPositionViewport, mZoom );
	}
	else {
		mPosition = newPositionViewport;
	}
	mIsDirty = true;
}

// Helper for fit operations that center the canvas
void CanvasUi::fitWithCentering( float newZoom, bool disableAnimation )
{
	CI_ASSERT( isBounded() && "CanvasUi: Fit operations require bounded content. Use setContentBounds() first." );

	vec2 canvasCenter = mCanvasBounds->getCenter();
	vec2 viewportCenter = mViewport.getSize() * 0.5f;
	vec2 newPosition = viewportCenter - canvasCenter * newZoom;

	if( mAnimationEnabled && ! disableAnimation ) {
		startZoomAndPositionAnimation( newZoom, newPosition );
	}
	else {
		setZoomAndPosition( newZoom, newPosition );
	}
}

// Dedicated zoom preset functions
void CanvasUi::fitAll( bool disableAnimation )
{
	CI_ASSERT( isBounded() && "CanvasUi: fitAll() requires bounded content. Use setContentBounds() first." );

	vec2 canvasSize = mCanvasBounds->getSize();
	vec2 viewportSize = mViewport.getSize();

	if( canvasSize.x <= 0 || canvasSize.y <= 0 || viewportSize.x <= 0 || viewportSize.y <= 0 ) {
		return;
	}

	float scaleX = viewportSize.x / canvasSize.x;
	float scaleY = viewportSize.y / canvasSize.y;

	// fitAll() always shows all content (uses min scale)
	fitWithCentering( glm::min( scaleX, scaleY ) );
}

void CanvasUi::fitWidth( bool disableAnimation )
{
	CI_ASSERT( isBounded() && "CanvasUi: fitWidth() requires bounded content. Use setContentBounds() first." );

	vec2  canvasSize = mCanvasBounds->getSize();
	float viewportWidth = mViewport.getWidth();

	if( canvasSize.x <= 0 || viewportWidth <= 0 ) {
		return;
	}

	fitWithCentering( viewportWidth / canvasSize.x );
}

void CanvasUi::fitHeight( bool disableAnimation )
{
	CI_ASSERT( isBounded() && "CanvasUi: fitHeight() requires bounded content. Use setContentBounds() first." );

	vec2  canvasSize = mCanvasBounds->getSize();
	float viewportHeight = mViewport.getHeight();

	if( canvasSize.y <= 0 || viewportHeight <= 0 ) {
		return;
	}

	fitWithCentering( viewportHeight / canvasSize.y );
}

void CanvasUi::fitRect( const Rectf& contentRect, bool disableAnimation )
{
	vec2 rectSize = contentRect.getSize();
	vec2 viewportSize = mViewport.getSize();

	if( rectSize.x <= 0 || rectSize.y <= 0 || viewportSize.x <= 0 || viewportSize.y <= 0 ) {
		return;
	}

	// Calculate zoom to fit the rectangle in the viewport (with aspect ratio preservation)
	float scaleX = viewportSize.x / rectSize.x;
	float scaleY = viewportSize.y / rectSize.y;
	float newZoom = glm::min( scaleX, scaleY );

	// Center the rectangle in the viewport
	vec2 rectCenter = contentRect.getCenter();
	vec2 viewportCenter = mViewport.getSize() * 0.5f;
	vec2 newPosition = viewportCenter - rectCenter * newZoom;

	if( mAnimationEnabled && ! disableAnimation ) {
		startZoomAndPositionAnimation( newZoom, newPosition );
	}
	else {
		setZoomAndPosition( newZoom, newPosition );
	}
}


void CanvasUi::zoomByStep( float factor, const vec2& anchorViewport )
{
	if( factor <= 0.0f )
		return;

	// Calculate new zoom target
	float newZoomTarget = mZoom * factor;
	newZoomTarget = glm::clamp( newZoomTarget, getEffectiveMinZoom(), mMaxZoom );

	if( newZoomTarget == mZoom )
		return;

	// Convert anchor from viewport coordinates to window coordinates for toContent()
	vec2 anchorWindow = anchorViewport + mViewport.getUpperLeft();
	vec2 contentPoint = toContent( anchorWindow );

	// Calculate new position to keep content point at anchor (anchorViewport is in viewport coordinates)
	// Formula: viewportPoint = position + contentPoint * zoom
	// Solving for position: position = viewportPoint - contentPoint * zoom
	vec2 newPosition = anchorViewport - contentPoint * newZoomTarget;

	// Apply constraints after zoom using the target zoom value
	if( isBounded() && ! mAllowPanOutside ) {
		newPosition = calculateConstrainedPosition( newPosition, newZoomTarget );
	}

	// Set new values
	mZoom = newZoomTarget;
	mPosition = newPosition;
	mIsDirty = true;
}

void CanvasUi::connect( const app::WindowRef& window, int signalPriority )
{
	disconnect(); // Clean up existing connections

	mWindow = window;
	updateViewportSize();

	if( window ) {
		// Connect to window events
		mConnections.push_back( window->getSignalMouseDown().connect( signalPriority, [this]( app::MouseEvent& event ) {
			if( mEnabled )
				mouseDown( event );
		} ) );

		mConnections.push_back( window->getSignalMouseUp().connect( signalPriority, [this]( app::MouseEvent& event ) {
			if( mEnabled )
				mouseUp( event );
		} ) );

		mConnections.push_back( window->getSignalMouseDrag().connect( signalPriority, [this]( app::MouseEvent& event ) {
			if( mEnabled )
				mouseDrag( event );
		} ) );

		mConnections.push_back( window->getSignalMouseWheel().connect( signalPriority, [this]( app::MouseEvent& event ) {
			if( mEnabled && mMouseWheelEnabled )
				mouseWheel( event );
		} ) );

		mConnections.push_back( window->getSignalKeyDown().connect( signalPriority, [this]( app::KeyEvent& event ) {
			if( mEnabled && mKeyboardEnabled )
				keyDown( event );
		} ) );

		mConnections.push_back( window->getSignalResize().connect( signalPriority, [this]() {
			if( mEnabled )
				resize( mWindow->getSize() );
		} ) );

		// Update loop for animation
		mConnections.push_back( window->getSignalDraw().connect( signalPriority, [this]() {
			if( mEnabled && mAnimationEnabled ) {
				updateAnimation();
			}
		} ) );
	}
}

void CanvasUi::disconnect()
{
	for( auto& connection : mConnections ) {
		connection.disconnect();
	}
	mConnections.clear();
	mWindow.reset();
}

void CanvasUi::updateViewportSize()
{
	if( mWindow ) {
		ivec2 newSize = mWindow->getSize();
		if( newSize != mWindowSize ) {
			mWindowSize = newSize;

			// Update viewport if not using custom viewport
			if( ! mHasCustomViewport ) {
				Rectf oldViewport = mViewport;
				mViewport = Rectf( 0, 0, (float)newSize.x, (float)newSize.y );
			}

			// Enforce zoom constraint after viewport change
			if( mConstrainZoomToContent && isBounded() ) {
				float effectiveMinZoom = getEffectiveMinZoom();
				if( mZoom < effectiveMinZoom ) {
					mZoom = effectiveMinZoom;
				}
			}

			// Always constrain position after resize
			constrainPosition();
			mIsDirty = true;
		}
	}
}

void CanvasUi::mouseDown( app::MouseEvent& event )
{
	// Check if mouse is within viewport
	vec2 mousePos = vec2( event.getPos() );
	if( ! mViewport.contains( mousePos ) ) {
		return; // Event outside viewport, don't handle
	}

	if( matchesPanMouseButton( event, mPanMouseButtons ) ) {
		// Only start new drag if not already dragging
		if( ! mIsDragging ) {
			// Stop any animation to prevent conflicts during drag
			if( mAnimating ) {
				mAnimating = false;
				mAnimatingZoom = false;
				mAnimatingPosition = false;
			}

			mIsDragging = true;
			mDragStartPosWindow = event.getPos();
			mDragStartPosition = mPosition;
			mDragInitiator = event.getInitiator();
		}
		event.setHandled();
	}
}

void CanvasUi::mouseUp( app::MouseEvent& event )
{
	// End drag if the button that started it was released
	if( mIsDragging && event.getInitiator() == mDragInitiator ) {
		mIsDragging = false;
		mDragInitiator = 0;
		event.setHandled();
	}
}

void CanvasUi::mouseDrag( app::MouseEvent& event )
{
	if( mIsDragging ) {
		// Both mouse positions are in window coordinates, so delta is also window coordinates
		vec2 windowDragDelta = vec2( event.getPos() - mDragStartPosWindow );
		// Since mPosition is stored in viewport coordinates, the delta should be the same
		// (viewport is just translated window coordinates, so deltas are identical)

		// For mouse dragging, use the mouse pan animation setting (only if animations enabled)
		panToViewportPos( mDragStartPosition + windowDragDelta, ! ( mAnimationEnabled && mAnimateMousePan ) );
		event.setHandled();
	}
}

void CanvasUi::mouseWheel( app::MouseEvent& event )
{
	// Check if mouse is within viewport
	vec2 mousePos = vec2( event.getPos() );
	if( ! mViewport.contains( mousePos ) ) {
		return; // Event outside viewport, don't handle
	}

	float inc = event.getWheelIncrement();
	if( mWheelInverted )
		inc = -inc;

	// Calculate zoom factor relative to zoom factor setting
	float effectiveZoomFactor = mZoomFactor * mWheelMultiplier;
	float zoomMultiplier;
	if( inc > 0 ) {
		zoomMultiplier = 1.0f + effectiveZoomFactor; // Zoom in
	}
	else {
		zoomMultiplier = 1.0f / ( 1.0f + effectiveZoomFactor ); // Zoom out (reciprocal)
	}
	// Determine anchor point in viewport-local coordinates
	vec2 anchorViewport = mZoomToCursor ? ( vec2( event.getPos() ) - mViewport.getUpperLeft() ) : ( mViewport.getSize() * 0.5f );

	if( mAnimationEnabled ) {
		// Start smooth animation to target (expects viewport-local coordinates)
		startZoomAnimation( zoomMultiplier, anchorViewport );
	}
	else {
		// Immediate zoom (expects viewport-local coordinates)
		zoomByStep( zoomMultiplier, anchorViewport );
	}
	event.setHandled();
}

void CanvasUi::keyDown( app::KeyEvent& event )
{
	if( ! mKeyboardEnabled )
		return;

	// Check hotkeys using the new system
	if( matchesHotkey( event, mHotkeysZoomIn ) ) {
		zoomIn();
		event.setHandled();
	}
	else if( matchesHotkey( event, mHotkeysZoomOut ) ) {
		zoomOut();
		event.setHandled();
	}
	else if( matchesHotkey( event, mHotkeysReset ) ) {
		if( isBounded() )
			fitAll();
		else
			viewOnetoOne();
		event.setHandled();
	}
	else if( matchesHotkey( event, mHotkeysOneToOne ) ) {
		viewOnetoOne();
		event.setHandled();
	}
}

void CanvasUi::resize( const ivec2& newSize )
{
	updateViewportSize();
}

void CanvasUi::zoomByStep( float factor )
{
	// Zoom from viewport center
	vec2 centerViewport = mViewport.getSize() * 0.5f;
	zoomByStep( factor, centerViewport );
}


void CanvasUi::startZoomAnimation( float zoomFactor, const vec2& anchorViewport )
{
	// Calculate final zoom target - if already animating, use the target, not current interpolated zoom
	float baseZoom = mAnimating ? mZoomAnimationTarget : mZoom;
	float newZoomTarget = baseZoom * zoomFactor;
	newZoomTarget = glm::clamp( newZoomTarget, getEffectiveMinZoom(), mMaxZoom );

	if( newZoomTarget == baseZoom )
		return; // No change needed

	// Capture the content point under the anchor at the START of animation
	// anchorViewport is in viewport-local coordinates, convert to window coordinates for toContent()
	vec2 anchorWindow = anchorViewport + mViewport.getUpperLeft();
	vec2 contentPoint = toContent( anchorWindow );

	// Start animation
	mAnimating = true;
	mAnimatingZoom = true;
	mAnimatingPosition = false;
	mZoomAnimationStart = mZoom;
	mZoomAnimationTarget = newZoomTarget;
	mZoomAnimationAnchor = anchorViewport; // Store viewport-local coordinates
	mZoomAnimationContentPoint = contentPoint;
	mAnimationStartTime = app::getElapsedSeconds();
}

void CanvasUi::updateAnimation()
{
	if( ! mAnimating )
		return;

	double currentTime = app::getElapsedSeconds();
	float  elapsed = (float)( currentTime - mAnimationStartTime );
	float  progress = elapsed / mAnimationDuration;

	if( progress >= 1.0f ) {
		// Animation complete
		progress = 1.0f;
		mAnimating = false;
		mAnimatingZoom = false;
		mAnimatingPosition = false;
	}

	// Smooth easing (configurable ease-out)
	float easedProgress = 1.0f - pow( 1.0f - progress, mAnimationEasing );

	if( mAnimatingZoom && mAnimatingPosition ) {
		// Animate both zoom and position
		float currentZoom = mZoomAnimationStart + ( mZoomAnimationTarget - mZoomAnimationStart ) * easedProgress;
		vec2  currentPosition = mPositionAnimationStart + ( mPositionAnimationTarget - mPositionAnimationStart ) * easedProgress;

		// Apply constraints
		if( isBounded() && ! mAllowPanOutside ) {
			currentPosition = calculateConstrainedPosition( currentPosition, currentZoom );
		}

		mZoom = currentZoom;
		mPosition = currentPosition;
	}
	else if( mAnimatingZoom ) {
		// Animate zoom with anchor point
		float currentZoom = mZoomAnimationStart + ( mZoomAnimationTarget - mZoomAnimationStart ) * easedProgress;
		vec2  newPosition = mZoomAnimationAnchor - mZoomAnimationContentPoint * currentZoom;

		// Apply constraints
		if( isBounded() && ! mAllowPanOutside ) {
			newPosition = calculateConstrainedPosition( newPosition, currentZoom );
		}

		mZoom = currentZoom;
		mPosition = newPosition;
	}
	else if( mAnimatingPosition ) {
		// Animate position only
		vec2 currentPosition = mPositionAnimationStart + ( mPositionAnimationTarget - mPositionAnimationStart ) * easedProgress;

		// Apply constraints
		if( isBounded() && ! mAllowPanOutside ) {
			currentPosition = calculateConstrainedPosition( currentPosition );
		}

		mPosition = currentPosition;
	}

	mIsDirty = true;
}

void CanvasUi::startPositionAnimation( const vec2& targetPositionViewport )
{
	mAnimating = true;
	mAnimatingZoom = false;
	mAnimatingPosition = true;
	mPositionAnimationStart = mPosition;
	mPositionAnimationTarget = targetPositionViewport;
	mAnimationStartTime = app::getElapsedSeconds();
}

void CanvasUi::startZoomAndPositionAnimation( float targetZoom, const vec2& targetPositionViewport )
{
	targetZoom = glm::clamp( targetZoom, getEffectiveMinZoom(), mMaxZoom );

	mAnimating = true;
	mAnimatingZoom = true;
	mAnimatingPosition = true;
	mZoomAnimationStart = mZoom;
	mZoomAnimationTarget = targetZoom;
	mPositionAnimationStart = mPosition;
	mPositionAnimationTarget = targetPositionViewport;
	mAnimationStartTime = app::getElapsedSeconds();
}

// Hotkey system implementation
void CanvasUi::setHotkeysZoomIn( const std::vector<HotkeyBinding>& hotkeys )
{
	mHotkeysZoomIn = hotkeys;
}

void CanvasUi::setHotkeysZoomOut( const std::vector<HotkeyBinding>& hotkeys )
{
	mHotkeysZoomOut = hotkeys;
}

void CanvasUi::setHotkeysReset( const std::vector<HotkeyBinding>& hotkeys )
{
	mHotkeysReset = hotkeys;
}

void CanvasUi::setHotkeysOneToOne( const std::vector<HotkeyBinding>& hotkeys )
{
	mHotkeysOneToOne = hotkeys;
}

bool CanvasUi::matchesHotkey( const app::KeyEvent& event, const std::vector<HotkeyBinding>& hotkeys ) const
{
	int			 keyCode = event.getCode();
	unsigned int modifiers = event.getModifiers();

	for( const auto& binding : hotkeys ) {
		if( keyCode == binding.key && modifiers == binding.modifiers ) {
			return true;
		}
	}
	return false;
}

// Mouse button system implementation
void CanvasUi::setPanMouseButtons( const std::vector<MouseButtonBinding>& buttons )
{
	mPanMouseButtons = buttons;
}

bool CanvasUi::matchesPanMouseButton( const app::MouseEvent& event, const std::vector<MouseButtonBinding>& buttons ) const
{
	// If no buttons configured, don't allow panning
	if( buttons.empty() )
		return false;

	unsigned int eventButtons = 0;
	unsigned int eventModifiers = event.getModifiers();

	// Build button mask from what's currently down
	if( event.isLeftDown() )
		eventButtons |= app::MouseEvent::LEFT_DOWN;
	if( event.isMiddleDown() )
		eventButtons |= app::MouseEvent::MIDDLE_DOWN;
	if( event.isRightDown() )
		eventButtons |= app::MouseEvent::RIGHT_DOWN;

	// Mask out mouse button flags from modifiers to avoid double-counting
	eventModifiers &= ~( app::MouseEvent::LEFT_DOWN | app::MouseEvent::MIDDLE_DOWN | app::MouseEvent::RIGHT_DOWN );

	for( const auto& binding : buttons )
		if( ( eventButtons == binding.button ) && ( eventModifiers == binding.modifiers ) )
			return true;

	return false;
}

// Viewport management
void CanvasUi::setCustomViewport( const Rectf& viewport )
{
	mViewport = viewport;
	mHasCustomViewport = true;
	mIsDirty = true;

	// Constrain position for new viewport
	if( isBounded() ) {
		constrainPosition();
	}
}

void CanvasUi::disableCustomViewport()
{
	mHasCustomViewport = false;
	// Update viewport to match window
	mViewport = Rectf( 0, 0, (float)mWindowSize.x, (float)mWindowSize.y );
	mIsDirty = true;

	// Constrain position for new viewport
	if( isBounded() ) {
		constrainPosition();
	}
}

float CanvasUi::getEffectiveMinZoom() const
{
	// Always respect the absolute minimum zoom
	float effectiveMin = mMinZoom;

	// If content constraint is enabled and content is bounded, calculate content-based minimum
	// NOTE: Only applies when allowPanOutside=false, since allowPanOutside=true makes this
	// constraint meaningless (you can create gaps by panning anyway)
	if( mConstrainZoomToContent && isBounded() && ! mAllowPanOutside ) {
		vec2 canvasSize = mCanvasBounds->getSize();
		vec2 viewportSize = mViewport.getSize();

		if( canvasSize.x > 0 && canvasSize.y > 0 && viewportSize.x > 0 && viewportSize.y > 0 ) {
			// Calculate zoom needed so the larger axis fits (prevents both axes from being loose)
			// This is the fitAll() level - content fits entirely but may have gaps on one axis
			float scaleX = viewportSize.x / canvasSize.x;
			float scaleY = viewportSize.y / canvasSize.y;
			float contentMinZoom = glm::min( scaleX, scaleY );

			// Use the larger of absolute minimum and content-based minimum
			effectiveMin = glm::max( effectiveMin, contentMinZoom );
		}
	}

	return effectiveMin;
}

void CanvasUi::setConstrainZoomToContent( bool constrain )
{
	bool wasConstrained = mConstrainZoomToContent;
	mConstrainZoomToContent = constrain;

	// Auto-disable pan outside when enabling zoom constraint (need controlled positioning)
	if( constrain )
		mAllowPanOutside = false;

	// If enabling constraint, fix zoom and position immediately
	// Only applies when allowPanOutside=false (when true, constraint is meaningless)
	if( constrain && ! wasConstrained && isBounded() && ! mAllowPanOutside ) {
		float effectiveMinZoom = getEffectiveMinZoom();
		if( mZoom < effectiveMinZoom ) {
			mZoom = effectiveMinZoom;
			mIsDirty = true;
		}
		// Always constrain position when enabling - this handles off-center content from previous allowPanOutside
		constrainPosition();
	}
}

void CanvasUi::setAllowPanOutside( bool allow )
{
	mAllowPanOutside = allow;

	// Auto-disable zoom constraint when enabling pan outside (makes constraint meaningless)
	if( allow )
		mConstrainZoomToContent = false;
}

} // namespace cinder
