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

//! CanvasUi provides smooth pan and zoom interaction for 2D content, similar to image editors,
//! drawing apps, or any application where users need to navigate around 2D content that may be
//! larger than the screen or require detailed inspection.
//!
//! CanvasUi manages transformations between three coordinate spaces:
//! - **Content space**: Your content's coordinate system (arbitrary units)
//! - **Viewport space**: Rectangular region within window where content is displayed
//! - **Window space**: Full application window coordinates (pixels from top-left)
//!
//! Content can be bounded (defined limits) or unbounded (infinite space). Default interactions
//! include mouse drag panning, mouse wheel zooming, and keyboard shortcuts.

#pragma once

#include "cinder/Matrix.h"
#include "cinder/Rect.h"
#include "cinder/Signals.h"
#include "cinder/app/KeyEvent.h"
#include "cinder/app/MouseEvent.h"
#include "cinder/app/Window.h"
#include <optional>

namespace cinder {

//! 2D canvas with pan/zoom interaction controls
//!
//! Provides a complete 2D navigation system with smooth animations, configurable
//! mouse/keyboard controls, and support for both bounded and unbounded content.
class CI_API CanvasUi {
  public:
	// Hotkey system (uses Cinder's KeyEvent system)
	struct HotkeyBinding {
		int			 key;		// KeyEvent key code (KEY_0, KEY_PLUS, etc.)
		unsigned int modifiers; // KeyEvent modifier flags (CTRL_DOWN, SHIFT_DOWN, etc.)
		HotkeyBinding( int k, unsigned int m )
			: key( k )
			, modifiers( m )
		{
		}
	};

	// Mouse interaction system (uses Cinder's MouseEvent system)
	struct MouseButtonBinding {
		unsigned int button;	// MouseEvent button flags (LEFT_DOWN, RIGHT_DOWN, MIDDLE_DOWN)
		unsigned int modifiers; // MouseEvent modifier flags (CTRL_DOWN, SHIFT_DOWN, etc.)
		MouseButtonBinding( unsigned int b, unsigned int m )
			: button( b )
			, modifiers( m )
		{
		}
	};

	//! Default constructor, unbounded canvas, no signal connections
	CanvasUi();
	//! Initiates connections to \a window, unbounded canvas
	CanvasUi( const app::WindowRef& window, int signalPriority = -1 );
	//! Initiates connections to \a window, sets content bounds to \a bounds
	CanvasUi( const app::WindowRef& window, const Rectf& bounds, int signalPriority = -1 );
	~CanvasUi();

	// Non-movable, non-copyable
	CanvasUi( CanvasUi&& ) = delete;
	CanvasUi& operator=( CanvasUi&& ) = delete;
	CanvasUi( const CanvasUi& ) = delete;
	CanvasUi& operator=( const CanvasUi& ) = delete;

	//! Set content bounds \a bounds for bounded mode
	void setContentBounds( const Rectf& bounds );
	//! Enable unbounded mode (infinite content space)
	void setUnbounded() { mCanvasBounds = std::nullopt; }
	//! Get current content bounds (nullopt if unbounded)
	const std::optional<Rectf>& getContentBounds() const { return mCanvasBounds; }
	//! Check if content is bounded
	bool isBounded() const { return mCanvasBounds.has_value(); }


	//! Get current model matrix (content to window transform)
	const mat4& getModelMatrix() const;
	//! Get inverse model matrix (window to content transform)
	const mat4& getInverseModelMatrix() const;
	//! Get current position offset in viewport coordinates
	vec2 getPosition() const { return mPosition; }
	//! Get current zoom factor
	float getZoom() const { return mZoom; }

	//! Convert window coordinates to content coordinates
	vec2 toContent( const vec2& posWindow ) const;
	//! Convert content coordinates to window coordinates
	vec2 toWindow( const vec2& posContent ) const;

	//! Get visible content area of current viewport
	Rectf getVisibleRect() const;
	//! Get visible content area for specific viewport rectangle \a rectViewport
	Rectf getVisibleRect( const Rectf& rectViewport ) const;

	//! Pan by \a deltaViewport pixels in viewport coordinates (useful for keyboard navigation)
	void panBy( const vec2& deltaViewport );
	//! Pan to \a posViewport position in viewport coordinates
	void panToViewportPos( const vec2& posViewport, bool disableAnimation = false );
	//! Pan to center \a posContent in the viewport
	void panToContentPos( const vec2& posContent, bool disableAnimation = false );
	//! Zoom by \a factor around viewport center
	void zoomBy( float factor, bool disableAnimation = false );
	//! Zoom by \a factor around \a anchorWindow point (window coordinates)
	void zoomBy( float factor, const vec2& anchorWindow, bool disableAnimation = false );
	//! Zoom to specific \a zoom level around viewport center
	void zoomTo( float zoom, bool disableAnimation = false );
	//! Zoom to specific \a zoom level around \a anchorWindow point (window coordinates)
	void zoomTo( float zoom, const vec2& anchorWindow, bool disableAnimation = false );

	//! Fit all content in viewport (requires bounded content)
	void fitAll( bool disableAnimation = false );
	//! Fit content width to viewport width (requires bounded content)
	void fitWidth( bool disableAnimation = false );
	//! Fit content height to viewport height (requires bounded content)
	void fitHeight( bool disableAnimation = false );
	//! Fit and center rectangle \a contentRect in the viewport
	void fitRect( const Rectf& contentRect, bool disableAnimation = false );
	//! Zoom in by one step
	void zoomIn( bool disableAnimation = false );
	//! Zoom out by one step
	void zoomOut( bool disableAnimation = false );
	//! Set zoom to 1.0 and offset to 0.0
	void viewOnetoOne( bool disableAnimation = false );

	//! Set custom viewport \a viewport rectangle in window coordinates
	void setCustomViewport( const Rectf& viewport );
	//! Disable custom viewport (use full window)
	void disableCustomViewport();
	//! Check if custom viewport is active
	bool hasCustomViewport() const { return mHasCustomViewport; }
	//! Get current viewport rectangle
	const Rectf& getViewport() const { return mViewport; }

	//! Connect to \a window for automatic event handling
	void connect( const app::WindowRef& window, int signalPriority = 0 );
	//! Disconnect from window events
	void disconnect();
	//! Check if connected to a window
	bool isConnected() const { return mWindow != nullptr; }

	//! Enable or disable all interactions
	void enable( bool enabled = true ) { mEnabled = enabled; }
	//! Disable all interactions
	void disable() { mEnabled = false; }
	//! Check if interactions are enabled
	bool isEnabled() const { return mEnabled; }
	//! Enable or disable keyboard shortcuts
	void setKeyboardEnabled( bool enabled ) { mKeyboardEnabled = enabled; }
	//! Check if keyboard shortcuts are enabled
	bool isKeyboardEnabled() const { return mKeyboardEnabled; }
	//! Enable or disable mouse wheel zooming
	void setMouseWheelEnabled( bool enabled ) { mMouseWheelEnabled = enabled; }
	//! Check if mouse wheel zooming is enabled
	bool isMouseWheelEnabled() const { return mMouseWheelEnabled; }

	//! Set mouse \a buttons for panning
	void setPanMouseButtons( const std::vector<MouseButtonBinding>& buttons );

	//! Set mouse wheel zoom \a multiplier (relative to zoom factor)
	void setMouseWheelMultiplier( float multiplier ) { mWheelMultiplier = multiplier; }
	//! Get mouse wheel zoom multiplier
	float getMouseWheelMultiplier() const { return mWheelMultiplier; }
	//! Set mouse wheel direction inverted
	void setMouseWheelInverted( bool inverted ) { mWheelInverted = inverted; }
	//! Check if mouse wheel is inverted
	bool isMouseWheelInverted() const { return mWheelInverted; }
	//! Enable zoom-to-cursor behavior
	void setZoomToCursor( bool enabled ) { mZoomToCursor = enabled; }
	//! Check if zoom-to-cursor is enabled
	bool isZoomToCursor() const { return mZoomToCursor; }
	//! Enable or disable animations (when disabled, overrides all other animation settings)
	void setAnimationEnabled( bool enabled ) { mAnimationEnabled = enabled; }
	//! Check if animations are enabled
	bool isAnimationEnabled() const { return mAnimationEnabled; }
	//! Enable animations for mouse pan operations (only effective when animations are enabled)
	void setAnimateMousePan( bool enabled ) { mAnimateMousePan = enabled; }
	//! Check if mouse pan animations are enabled (returns false if animations are disabled)
	bool hasAnimateMousePan() const { return mAnimationEnabled && mAnimateMousePan; }

	//! Set animation \a duration in seconds
	void setAnimationDuration( float duration ) { mAnimationDuration = glm::clamp( duration, 0.1f, 2.0f ); }
	//! Get animation duration
	float getAnimationDuration() const { return mAnimationDuration; }
	//! Set animation \a easing curve power
	void setAnimationEasing( float easing ) { mAnimationEasing = glm::clamp( easing, 1.0f, 5.0f ); }
	//! Get animation easing curve power
	float getAnimationEasing() const { return mAnimationEasing; }

	//! Set zoom limits between \a minZoom and \a maxZoom
	void setZoomLimits( float minZoom, float maxZoom )
	{
		mMinZoom = minZoom;
		mMaxZoom = maxZoom;
	}
	//! Get minimum zoom limit
	float getMinZoom() const { return mMinZoom; }
	//! Get maximum zoom limit
	float getMaxZoom() const { return mMaxZoom; }
	//! Set zoom \a factor for zoom in/out (0.1 = 10% change per step). Default is 0.2.
	void setZoomFactor( float factor ) { mZoomFactor = factor; }
	//! Get zoom factor
	float getZoomFactor() const { return mZoomFactor; }

	//! Set \a hotkeys for zoom in
	void setHotkeysZoomIn( const std::vector<HotkeyBinding>& hotkeys );
	//! Set \a hotkeys for zoom out
	void setHotkeysZoomOut( const std::vector<HotkeyBinding>& hotkeys );
	//! Set \a hotkeys for reset zoom, fitting to window for bounded and equivalent to one-to-one for unbounded
	void setHotkeysReset( const std::vector<HotkeyBinding>& hotkeys );
	//! Set \a hotkeys for setting zoom and pan to 1:1
	void setHotkeysOneToOne( const std::vector<HotkeyBinding>& hotkeys );
	
	//! Allow panning content outside viewport bounds (auto-disables constrainZoomToContent when enabled). Default is \c false.
	void setAllowPanOutside( bool allow );
	//! Check if content can be panned outside viewport bounds. Default is \c false
	bool getAllowPanOutside() const { return mAllowPanOutside; }
	//! Prevent zooming out beyond fitAll() level (auto-disables allowPanOutside when enabled, ignored for unbounded content). Default is \c false.
	void setConstrainZoomToContent( bool constrain );
	//! Return if zoom is constrained to content. Default is \c false.
	bool getConstrainZoomToContent() const { return mConstrainZoomToContent; }

	//! Manual event handling (alternative to connect())
	void mouseDown( app::MouseEvent& event );
	void mouseUp( app::MouseEvent& event );
	void mouseDrag( app::MouseEvent& event );
	void mouseWheel( app::MouseEvent& event );
	void keyDown( app::KeyEvent& event );
	void resize( const ivec2& newSize );

  private:
	// Transform state
	vec2				 mPosition{ 0, 0 }; // Position offset in viewport-local coordinates (relative to viewport origin)
	float				 mZoom{ 1.0f };		// Current zoom scale factor
	std::optional<Rectf> mCanvasBounds;		// Optional content bounds for constrained panning

	// Viewport state
	Rectf mViewport{ 0, 0, 1, 1 };	   // Current viewport in window coordinates
	bool  mHasCustomViewport{ false }; // Whether custom viewport is set, otherwise uses full Window
	ivec2 mWindowSize{ 1, 1 };		   // Full window size

	// Cached transform matrices
	mutable mat4 mModelMatrix;
	mutable mat4 mInverseModelMatrix;
	mutable bool mIsDirty{ true };

	// Configuration
	bool mEnabled{ true };
	bool mKeyboardEnabled{ true };
	bool mMouseWheelEnabled{ true };
	bool mAllowPanOutside{ false };
	bool mConstrainZoomToContent{ false };

	// Mouse wheel settings
	float mWheelMultiplier{ 1.0f }; // Multiplier relative to zoom factor (1.0 = same as zoom factor)
	bool  mWheelInverted{ false };
	bool  mZoomToCursor{ true };
	bool  mAnimationEnabled{ true }; // Animation enabled by default for buttons/hotkeys
	bool  mAnimateMousePan{ false }; // Mouse pan animation disabled by default for responsiveness

	// Zoom settings
	float mMinZoom{ 0.01f };
	float mMaxZoom{ 100.0f };
	float mZoomFactor{ 0.2f }; // Default 20% zoom change per step

	std::vector<HotkeyBinding>		mHotkeysZoomIn;
	std::vector<HotkeyBinding>		mHotkeysZoomOut;
	std::vector<HotkeyBinding>		mHotkeysReset;
	std::vector<HotkeyBinding>		mHotkeysOneToOne;
	std::vector<MouseButtonBinding> mPanMouseButtons;

	// Animation state
	bool   mAnimating{ false };
	float  mAnimationDuration{ 0.3f };		   // Duration of animation in seconds
	float  mAnimationEasing{ 3.0f };		   // Easing power for cubic ease-out (1.0 = linear, higher = more curved)
	float  mZoomAnimationStart{ 1.0f };		   // Starting zoom level
	float  mZoomAnimationTarget{ 1.0f };	   // Target zoom level
	vec2   mPositionAnimationStart{ 0.0f };	   // Starting position
	vec2   mPositionAnimationTarget{ 0.0f };   // Target position
	vec2   mZoomAnimationAnchor{ 0.0f };	   // Anchor point for zoom animation
	vec2   mZoomAnimationContentPoint{ 0.0f }; // Content point under anchor at start
	double mAnimationStartTime{ 0.0 };		   // When animation started
	bool   mAnimatingZoom{ false };			   // Whether zoom is being animated
	bool   mAnimatingPosition{ false };		   // Whether position is being animated

	// Interaction state
	bool  mIsDragging{ false };
	ivec2 mDragStartPosWindow;
	vec2  mDragStartPosition;
	int	  mDragInitiator{ 0 }; // Which button started the drag (LEFT_DOWN, RIGHT_DOWN, or MIDDLE_DOWN)

	// Connection state
	app::WindowRef					 mWindow;
	std::vector<signals::Connection> mConnections;


	// Internal methods
	void  fitWithCentering( float newZoom, bool disableAnimation = false );
	void  zoomByStep( float factor );											// Zoom from center with constraints
	float getEffectiveMinZoom() const;											// Get minimum zoom considering content constraint
	void  zoomByStep( float factor, const vec2& anchorViewport );				// Zoom from specified anchor in viewport-local coordinates
	void  setZoomAndPosition( float newZoom, const vec2& newPositionViewport ); // Helper for zoom presets, position in viewport-local coordinates
	void  updateMatrices() const;
	void  constrainPosition();
	vec2  calculateConstrainedPosition( const vec2& desiredPos ) const;
	vec2  calculateConstrainedPosition( const vec2& desiredPos, float zoom ) const;
	void  updateViewportSize();
	void  updateAnimation();
	void  startZoomAnimation( float zoomFactor, const vec2& anchorViewport );					 // Anchor in viewport-local coordinates
	void  startPositionAnimation( const vec2& targetPositionViewport );							 // Target position in viewport-local coordinates
	void  startZoomAndPositionAnimation( float targetZoom, const vec2& targetPositionViewport ); // Target position in viewport-local coordinates
	bool  matchesHotkey( const app::KeyEvent& event, const std::vector<HotkeyBinding>& hotkeys ) const;
	bool  matchesPanMouseButton( const app::MouseEvent& event, const std::vector<MouseButtonBinding>& buttons ) const;
};

} // namespace cinder
