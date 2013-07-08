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

#include "cinder/BinPacker.h"
#include <cassert>
#include <algorithm>

namespace cinder {

std::vector<Area> BinPacker::pack( const std::vector<Area> &rects)
{
    clear();

    // add rects to member array, and check to make sure none is too big
    for (size_t i = 0; i < rects.size(); ++i) {
		if (rects[i].getWidth() > mBinWidth || rects[i].getHeight() > mBinHeight) {
            throw BinPackerTooSmallExc();
        }
		mRects.push_back(Rect(rects[i].getWidth(), rects[i].getHeight(), i));
    }

    // sort from greatest to least area
    std::sort(mRects.rbegin(), mRects.rend());

    // pack   
    mBins.push_back(Rect(mBinWidth, mBinHeight));
    fill(0);

	// check if all rects were packed
	if(mNumPacked < (int)mRects.size()) 
		throw BinPackerTooSmallExc();

	// return result
	std::vector<Area>	result;
	result.resize( rects.size() );

	for(unsigned i=0;i<mBins.size();++i)
	{
		// skip empty bins
		if( mBins[i].order < 0 ) continue;

		if(mBins[i].rotated)
			result[ mBins[i].order ] = Area( mBins[i].x, mBins[i].y + mBins[i].h, mBins[i].x + mBins[i].w, mBins[i].y );
		else
			result[ mBins[i].order ] = Area( mBins[i].x, mBins[i].y, mBins[i].x + mBins[i].w, mBins[i].y + mBins[i].h );
	}

	return result;
}

void BinPacker::pack( std::vector<Area*> &rects)
{
    clear();

    // add rects to member array, and check to make sure none is too big
    for (size_t i = 0; i < rects.size(); ++i) {
		if (rects[i]->getWidth() > mBinWidth || rects[i]->getHeight() > mBinHeight) {
            throw BinPackerTooSmallExc();
        }
		mRects.push_back(Rect(rects[i]->getWidth(), rects[i]->getHeight(), i));
    }

    // sort from greatest to least area
    std::sort(mRects.rbegin(), mRects.rend());

	// disable rotation; subsequent calling of this in-place function would 
	// not be possible otherwise
	bool allowRotation = mAllowRotation;
	mAllowRotation = false;

    // pack   
    mBins.push_back(Rect(mBinWidth, mBinHeight));
    fill(0);

	// restore rotation
	mAllowRotation = allowRotation;

	// check if all rects were packed
	if(mNumPacked < (int)mRects.size()) 
		throw BinPackerTooSmallExc();

	// 
	for(unsigned i=0;i<mBins.size();++i)
	{
		// skip empty bins
		if( mBins[i].order < 0 ) continue;

		if(mBins[i].rotated) 
		{
			rects[ mBins[i].order ]->x1 = mBins[i].x;
			rects[ mBins[i].order ]->y1 = mBins[i].y + mBins[i].h;
			rects[ mBins[i].order ]->x2 = mBins[i].x + mBins[i].w;
			rects[ mBins[i].order ]->y2 = mBins[i].y;
		}
		else
		{
			rects[ mBins[i].order ]->x1 = mBins[i].x;
			rects[ mBins[i].order ]->y1 = mBins[i].y;
			rects[ mBins[i].order ]->x2 = mBins[i].x + mBins[i].w;
			rects[ mBins[i].order ]->y2 = mBins[i].y + mBins[i].h;
		}
	}
}

void BinPacker::clear()
{
    mNumPacked = 0;
    mRects.clear();
    mBins.clear();
}

void BinPacker::fill(int pack)
{
    assert(packIsValid(pack));

    int i = pack;

    // For each rect
    for (size_t j = 0; j < mRects.size(); ++j) {
        // If it's not already packed
        if (!mRects[j].packed) {
            // If it fits in the current working area
            if (fits(mRects[j], mBins[i])) {
                // Store in lower-left of working area, split, and recurse
                ++mNumPacked;
                split(i, j);
                fill(mBins[i].children[0]);
                fill(mBins[i].children[1]);
                return;
            }
        }
    }
}

void BinPacker::split(int pack, int rect)
{
    assert(packIsValid(pack));
    assert(rectIsValid(rect));

    int i = pack;
    int j = rect;

    // Split the working area either horizontally or vertically with respect
    // to the rect we're storing, such that we get the largest possible child
    // area.

    Rect left = mBins[i];
    Rect right = mBins[i];
    Rect bottom = mBins[i];
    Rect top = mBins[i];

    left.y += mRects[j].h;
    left.w = mRects[j].w;
    left.h -= mRects[j].h;
    right.x += mRects[j].w;
    right.w -= mRects[j].w;

    bottom.x += mRects[j].w;
    bottom.h = mRects[j].h;
    bottom.w -= mRects[j].w;
    top.y += mRects[j].h;
    top.h -= mRects[j].h;

    int maxLeftRightArea = left.getArea();
    if (right.getArea() > maxLeftRightArea) {
        maxLeftRightArea = right.getArea();
    }

    int maxBottomTopArea = bottom.getArea();
    if (top.getArea() > maxBottomTopArea) {
        maxBottomTopArea = top.getArea();
    }

    if (maxLeftRightArea > maxBottomTopArea) {
        if (left.getArea() > right.getArea()) {
            mBins.push_back(left);
            mBins.push_back(right);
        } else {
            mBins.push_back(right);
            mBins.push_back(left);
        }
    } else {
        if (bottom.getArea() > top.getArea()) {
            mBins.push_back(bottom);
            mBins.push_back(top);
        } else {
            mBins.push_back(top);
            mBins.push_back(bottom);
        }
    }

    // This pack area now represents the rect we've just stored, so save the
    // relevant info to it, and assign children.
    mBins[i].w = mRects[j].w;
    mBins[i].h = mRects[j].h;
    mBins[i].order = mRects[j].order;
    mBins[i].rotated = mRects[j].rotated;
    mBins[i].children[0] = mBins.size() - 2;
    mBins[i].children[1] = mBins.size() - 1;

    // Done with the rect
    mRects[j].packed = true;
}

bool BinPacker::fits(Rect& rect1, const Rect& rect2)
{
    // Check to see if rect1 fits in rect2, and rotate rect1 if that will
    // enable it to fit.

    if (rect1.w <= rect2.w && rect1.h <= rect2.h) {
        return true;
    } else if (mAllowRotation && rect1.h <= rect2.w && rect1.w <= rect2.h) {
        rect1.rotate();
        return true;
    } else {
        return false;
    }
}

bool BinPacker::rectIsValid(int i) const
{
    return i >= 0 && i < (int)mRects.size();
}

bool BinPacker::packIsValid(int i) const
{
    return i >= 0 && i < (int)mBins.size();
}

} // namespace cinder