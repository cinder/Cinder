#include "cinder/BinPacker.h"
#include <cassert>
#include <algorithm>

namespace cinder {

int BinPacker::pack( const std::vector<Area> &rects)
{
    clear();

    // Add rects to member array, and check to make sure none is too big
    for (size_t i = 0; i < rects.size(); ++i) {
		if (rects[i].getWidth() > mBinWidth || rects[i].getHeight() > mBinHeight) {
            throw BinPackerTooSmallExc();
        }
		m_rects.push_back(Rect(0, 0, rects[i].getWidth(), rects[i].getHeight(), i));
    }

    // Sort from greatest to least area
    std::sort(m_rects.rbegin(), m_rects.rend());

    // Pack
    while (m_numPacked < (int) m_rects.size() && (int) m_bins.size() < mMaxBinCount ) 
	{
        int i = m_packs.size();
        m_packs.push_back(Rect(mBinWidth, mBinHeight));
        m_bins.push_back(i);
        fill(i);
    }

	if(m_numPacked < (int)m_rects.size()) 
		throw BinPackerTooSmallExc();

	return m_bins.size();
}

void BinPacker::clear()
{
    m_numPacked = 0;
    m_rects.clear();
    m_packs.clear();
    m_bins.clear();
}

void BinPacker::fill(int pack)
{
    assert(packIsValid(pack));

    int i = pack;

    // For each rect
    for (size_t j = 0; j < m_rects.size(); ++j) {
        // If it's not already packed
        if (!m_rects[j].packed) {
            // If it fits in the current working area
            if (fits(m_rects[j], m_packs[i])) {
                // Store in lower-left of working area, split, and recurse
                ++m_numPacked;
                split(i, j);
                fill(m_packs[i].children[0]);
                fill(m_packs[i].children[1]);
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

    Rect left = m_packs[i];
    Rect right = m_packs[i];
    Rect bottom = m_packs[i];
    Rect top = m_packs[i];

    left.y += m_rects[j].h;
    left.w = m_rects[j].w;
    left.h -= m_rects[j].h;
    right.x += m_rects[j].w;
    right.w -= m_rects[j].w;

    bottom.x += m_rects[j].w;
    bottom.h = m_rects[j].h;
    bottom.w -= m_rects[j].w;
    top.y += m_rects[j].h;
    top.h -= m_rects[j].h;

    int maxLeftRightArea = left.GetArea();
    if (right.GetArea() > maxLeftRightArea) {
        maxLeftRightArea = right.GetArea();
    }

    int maxBottomTopArea = bottom.GetArea();
    if (top.GetArea() > maxBottomTopArea) {
        maxBottomTopArea = top.GetArea();
    }

    if (maxLeftRightArea > maxBottomTopArea) {
        if (left.GetArea() > right.GetArea()) {
            m_packs.push_back(left);
            m_packs.push_back(right);
        } else {
            m_packs.push_back(right);
            m_packs.push_back(left);
        }
    } else {
        if (bottom.GetArea() > top.GetArea()) {
            m_packs.push_back(bottom);
            m_packs.push_back(top);
        } else {
            m_packs.push_back(top);
            m_packs.push_back(bottom);
        }
    }

    // This pack area now represents the rect we've just stored, so save the
    // relevant info to it, and assign children.
    m_packs[i].w = m_rects[j].w;
    m_packs[i].h = m_rects[j].h;
    m_packs[i].ID = m_rects[j].ID;
    m_packs[i].rotated = m_rects[j].rotated;
    m_packs[i].children[0] = m_packs.size() - 2;
    m_packs[i].children[1] = m_packs.size() - 1;

    // Done with the rect
    m_rects[j].packed = true;
}

bool BinPacker::fits(Rect& rect1, const Rect& rect2)
{
    // Check to see if rect1 fits in rect2, and rotate rect1 if that will
    // enable it to fit.

    if (rect1.w <= rect2.w && rect1.h <= rect2.h) {
        return true;
    } else if (mAllowRotation && rect1.h <= rect2.w && rect1.w <= rect2.h) {
        rect1.Rotate();
        return true;
    } else {
        return false;
    }
}

void BinPacker::addPackToArray(int pack, std::vector<Pack>& array) const
{
    assert(packIsValid(pack));

    int i = pack;
    if (m_packs[i].ID != -1) {
		Pack bin;
		bin.id = m_packs[i].ID;
		//bin.bin = 0;
		//while( m_bins[bin.bin] > pack ) { bin.bin++; }
		bin.area = Area(m_packs[i].x, m_packs[i].y, m_packs[i].x + m_packs[i].w, m_packs[i].y + m_packs[i].h);
		array.push_back( bin );

        if (m_packs[i].children[0] != -1) {
            addPackToArray(m_packs[i].children[0], array);
        }
        if (m_packs[i].children[1] != -1) {
            addPackToArray(m_packs[i].children[1], array);
        }
    }
}

bool BinPacker::rectIsValid(int i) const
{
    return i >= 0 && i < (int)m_rects.size();
}

bool BinPacker::packIsValid(int i) const
{
    return i >= 0 && i < (int)m_packs.size();
}


std::vector<BinPacker::Pack>	BinPacker::getPacks(unsigned bin) const 
{
	std::vector<Pack>	rects;

	if(bin >= m_bins.size())
		return rects;

	// Write out
    //packs.resize(m_bins.size());
    //for (size_t i = 0; i < m_bins.size(); ++i) {
	int pack = m_bins[bin];


    addPackToArray(pack, rects);
    //}

    /*// Check and make sure all rects were packed
    for (size_t i = 0; i < m_rects.size(); ++i) {
        if (!m_rects[i].packed) {
            assert(!"Not all rects were packed");
        }
    }*/

	return rects;
}

std::vector<Area>	BinPacker::getBin(unsigned bin) const 
{
	std::vector<Area> packs;

	if( bin >= m_bins.size() )
		return packs;

	unsigned mn = m_bins[bin];
	unsigned mx = m_packs.size();
	if( bin+1 < m_bins.size() ) 
		mx = m_bins[bin+1];

	for(unsigned i=mn;i<mx;++i) {
		int x = m_packs[i].x;
		int y = m_packs[i].y;
		packs.push_back( Area(x, y, x+m_packs[i].w, y+m_packs[i].h) );
	}

	return packs;
}

} // namespace cinder