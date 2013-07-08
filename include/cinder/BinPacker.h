/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.
 
 Portions of this code (C) Paul Houx
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

#pragma once

#include "cinder/Area.h"
#include <vector>

namespace cinder {

class BinPacker
{
public:
	BinPacker() : mBinWidth(512), mBinHeight(512), mAllowRotation(false) {}
	BinPacker( int width, int height, bool allowRotation = false ) :
		mBinWidth(width), mBinHeight(height), mAllowRotation(allowRotation) {}

	//!
	BinPacker&	setSize( unsigned width, unsigned height ) { mBinWidth = width; mBinHeight = height; return *this; }
	BinPacker&	setSize( const Vec2i &size ) { mBinWidth = size.x; mBinHeight = size.y; return *this; }
	//!
	Vec2i		getSize() const { return Vec2i( mBinWidth, mBinHeight ); }
	//!
	int			getWidth() const { return mBinWidth; }
	//!
	int			getHeight() const { return mBinHeight; }

	//!
	BinPacker&	enableRotation( bool enabled = true ) { mAllowRotation = enabled; return *this; }
	//!
	bool		isRotationEnabled() const { return mAllowRotation; }

	//!
    std::vector<Area>	pack( const std::vector<Area> &rects );
	//! in-place
	void				pack( std::vector<Area*> &rects );

private:
    struct Rect
    {
        int  x;
        int  y;
        int  w;
        int  h;
        int  order;
        int  children[2];
        bool rotated;
        bool packed;

        Rect(int w, int h, int order = -1)
            : x(0), y(0), w(w), h(h), order(order), rotated(false), packed(false)
        {
            children[0] = -1;
            children[1] = -1;
        }

        Rect(int x, int y, int w, int h, int order = -1)
            : x(x), y(y), w(w), h(h), order(order), rotated(false), packed(false)
        {
            children[0] = -1;
            children[1] = -1;
        }
        
        int getArea() const {
            return w * h;
        }
        
        void rotate() {
            std::swap(w, h);
            rotated = !rotated;
        }
        
        bool operator<(const Rect& rhs) const {
            return getArea() < rhs.getArea();
        }
    };

private:
	int					mBinWidth;
	int					mBinHeight;
	bool				mAllowRotation;
    
    int					mNumPacked;
    std::vector<Rect>	mRects;
    std::vector<Rect>	mBins;

    void clear();
    void fill(int pack);
    void split(int pack, int rect);
    bool fits(Rect& rect1, const Rect& rect2);
    
    bool rectIsValid(int i) const;
    bool packIsValid(int i) const;
};

class BinPackerTooSmallExc : public std::exception {
 public:
	virtual const char* what() const throw() { return "BinPacker: Pack size is too small to fit all rectangles."; } 
};

} // namespace cinder
