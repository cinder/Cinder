#pragma once

#include "cinder/Area.h"

#include <vector>

namespace cinder {

class BinPacker
{
public:
	struct Pack {
		int32_t	id;
		int32_t	bin;
		Area	area;
	};

	BinPacker() : mBinWidth(512), mBinHeight(512), mMaxBinCount(1), mAllowRotation(true) {}
	BinPacker( int width, int height, int count = 1, bool allowRotation = false ) :
		mBinWidth(width), mBinHeight(height), mMaxBinCount(count), mAllowRotation(allowRotation) {}
		
	//! 
	BinPacker&	setMaxBinCount( unsigned count ) { mMaxBinCount = count; return *this; }
	//! 
	unsigned	getMaxBinCount() const { return mMaxBinCount; }

	//!
	unsigned	getBinCount() const { return m_bins.size(); }

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


    int pack( const std::vector<Area> &rects );

	std::vector<Pack> getPacks( unsigned bin = 0 ) const;
	std::vector<Area> getBin( unsigned bin = 0 ) const;

private:

	int		mMaxBinCount;
	int		mBinWidth;
	int		mBinHeight;
	bool	mAllowRotation;

    struct Rect
    {
        Rect(int w, int h)
            : x(0), y(0), w(w), h(h), ID(-1), rotated(false), packed(false)
        {
            children[0] = -1;
            children[1] = -1;
        }

        Rect(int x, int y, int w, int h, int ID = 1)
            : x(x), y(y), w(w), h(h), ID(ID), rotated(false), packed(false)
        {
            children[0] = -1;
            children[1] = -1;
        }
        
        int GetArea() const {
            return w * h;
        }
        
        void Rotate() {
            std::swap(w, h);
            rotated = !rotated;
        }
        
        bool operator<(const Rect& rect) const {
            return GetArea() < rect.GetArea();
        }

        int  x;
        int  y;
        int  w;
        int  h;
        int  ID;
        int  children[2];
        bool rotated;
        bool packed;
    };

    void clear();
    void fill(int pack);
    void split(int pack, int rect);
    bool fits(Rect& rect1, const Rect& rect2);
    void addPackToArray(int pack, std::vector<Pack>& array) const;
    
    bool rectIsValid(int i) const;
    bool packIsValid(int i) const;
    
    int               m_numPacked;
    std::vector<Rect> m_rects;
    std::vector<Rect> m_packs;
    std::vector<int>  m_bins;
};

class BinPackerTooSmallExc : public std::exception {
 public:
	virtual const char* what() const throw() { return "BinPacker: Pack size is too small to fit all rectangles."; } 
};

} // namespace cinder
