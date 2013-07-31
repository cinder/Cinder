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

void BinPackerBase::clear()
{
    mNumPacked = 0;
    mRects.clear();
    mPacks.clear();
}

bool BinPackerBase::fits(Rect& rect1, const Rect& rect2) const
{
    // Check to see if rect1 fits in rect2
    if (rect1.w <= rect2.w && rect1.h <= rect2.h) {
        return true;
    } else {
        return false;
    }
}

void BinPackerBase::fill(int pack)
{
    assert(packIsValid(pack));

    int i = pack;

    // For each rect
    for (size_t j = 0; j < mRects.size(); ++j) {
        // If it's not already packed
        if (!mRects[j].packed) {
            // If it fits in the current working area
            if (fits(mRects[j], mPacks[i])) {
                // Store in lower-left of working area, split, and recurse
                ++mNumPacked;
                split(i, j);
                fill(mPacks[i].children[0]);
                fill(mPacks[i].children[1]);
                return;
            }
        }
    }
}

void BinPackerBase::split(int pack, int rect)
{
    assert(packIsValid(pack));
    assert(rectIsValid(rect));

    int i = pack;
    int j = rect;

    // Split the working area either horizontally or vertically with respect
    // to the rect we're storing, such that we get the largest possible child
    // area.

    Rect left = mPacks[i];
    Rect right = mPacks[i];
    Rect bottom = mPacks[i];
    Rect top = mPacks[i];

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
            mPacks.push_back(left);
            mPacks.push_back(right);
        } else {
            mPacks.push_back(right);
            mPacks.push_back(left);
        }
    } else {
        if (bottom.getArea() > top.getArea()) {
            mPacks.push_back(bottom);
            mPacks.push_back(top);
        } else {
            mPacks.push_back(top);
            mPacks.push_back(bottom);
        }
    }

    // This pack area now represents the rect we've just stored, so save the
    // relevant info to it, and assign children.
    mPacks[i].w = mRects[j].w;
    mPacks[i].h = mRects[j].h;
    mPacks[i].order = mRects[j].order;
    mPacks[i].children[0] = mPacks.size() - 2;
    mPacks[i].children[1] = mPacks.size() - 1;

    // Done with the rect
    mRects[j].packed = true;
}

bool BinPackerBase::rectIsValid(int i) const
{
    return i >= 0 && i < (int)mRects.size();
}

bool BinPackerBase::packIsValid(int i) const
{
    return i >= 0 && i < (int)mPacks.size();
}

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
    mPacks.push_back(Rect(mBinWidth, mBinHeight));
    fill(0);

	// check if all rects were packed
	if(mNumPacked < (int)mRects.size()) 
		throw BinPackerTooSmallExc();

	// return result
	std::vector<Area>	result;
	result.resize( rects.size() );

	for(unsigned i=0;i<mPacks.size();++i)
	{
		// skip empty bins
		if( mPacks[i].order < 0 ) continue;

		result[ mPacks[i].order ] = Area( mPacks[i].x, mPacks[i].y, mPacks[i].x + mPacks[i].w, mPacks[i].y + mPacks[i].h );
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

    // pack   
    mPacks.push_back(Rect(mBinWidth, mBinHeight));
    fill(0);

	// check if all rects were packed
	if(mNumPacked < (int)mRects.size()) 
		throw BinPackerTooSmallExc();

	// 
	for(unsigned i=0;i<mPacks.size();++i)
	{
		// skip empty bins
		if( mPacks[i].order < 0 ) continue;

		rects[ mPacks[i].order ]->x1 = mPacks[i].x;
		rects[ mPacks[i].order ]->y1 = mPacks[i].y;
		rects[ mPacks[i].order ]->x2 = mPacks[i].x + mPacks[i].w;
		rects[ mPacks[i].order ]->y2 = mPacks[i].y + mPacks[i].h;
	}
}

std::vector<BinnedArea> MultiBinPacker::pack( const std::vector<Area> &rects)
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

    // Pack
    while (mNumPacked < (int) mRects.size() ) 
	{
        int i = mPacks.size();
        mPacks.push_back(Rect(mBinWidth, mBinHeight));
        mBins.push_back(i);
        fill(i);
    }

	if(mNumPacked < (int) mRects.size()) 
		throw BinPackerTooSmallExc();	

	// return result
	std::vector<BinnedArea>	result;
	result.resize( rects.size() );

	for(unsigned j=0;j<mBins.size();++j)
	{
		// calculate size 
		unsigned lower = mBins[j];
		unsigned upper = mBins.size() > j+1 ? mBins[j+1] : mPacks.size();

		for(unsigned i=lower;i<upper;++i)
		{
			// skip empty bins
			if( mPacks[i].order < 0 ) continue;

			result[ mPacks[i].order ] = BinnedArea( mPacks[i].x, mPacks[i].y, mPacks[i].x + mPacks[i].w, mPacks[i].y + mPacks[i].h, j );
		}
	}

	return result;
}

void MultiBinPacker::clear()
{
    BinPackerBase::clear();
	mBins.clear();
}

} // namespace cinder