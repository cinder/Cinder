#include "cinder/BinPack.h"
#include "cinder/CinderMath.h"
#include <vector>

namespace cinder {

BinPack::~BinPack()
{ }

class SkylinePack : public BinPack
{
  public:
	SkylinePack(int width, int height) : mWidth(width), mHeight(height), mUsed(0)
	{
		Node node = { 0, 0, width };
		mNodes.push_back( node );
	}

	virtual ~SkylinePack() { }

  private:
	struct Region
	{
		int x, y, width, height;
	};

	struct Node
	{
		int x, y, width;
	};

	std::vector<Node> mNodes;
	int mWidth;
	int mHeight;
	int mUsed;

	int fit( size_t index, int width, int height )
	{
		Node& node = mNodes[index];
		int x = node.x, y, width_left = width;
		size_t i = index;

		if ( (x + width) > mWidth ) {
			return -1;
		}

		y = node.y;

		while ( width_left > 0 ) {
			Node& next = mNodes[i];
			y = math<int>::max( y, next.y );
			if ( (y + height) > mHeight ) {
				return -1;
			}
			width_left -= next.width;
			++i;
		}

		return y;
	}

	void merge()
	{
		size_t i;

		for ( i = 0; i < mNodes.size() - 1; ++i ) {
			Node& node = mNodes[i];
			Node& next = mNodes[i+1];

			if ( node.y == next.y ) {
				node.width += next.width;
				mNodes.erase( mNodes.begin() + i + 1 );
				--i;
			}
		}
	}

  public:
	virtual Area allocateArea( int width, int height )
	{
		int y, best_height, best_width, best_index;
		SkylinePack::Region region = { 0, 0, width, height };
		size_t i;

		best_height = INT_MAX;
		best_index  = -1;
		best_width = INT_MAX;
		for ( i = 0; i < mNodes.size(); ++i ) {
			y = fit( i, width, height );
			if( y >= 0 ) {
				Node& node = mNodes[i];
				if( ( y + height < best_height ) ||
						( y + height == best_height && node.width < best_width ) ) {
					best_height = y + height;
					best_index = i;
					best_width = node.width;
					region.x = node.x;
					region.y = y;
				}
			}
		}

		if ( best_index == -1 ) {
			return Area(-1, -1, -1, -1);
		}

		Node newNode;
		newNode.x	 = region.x;
		newNode.y	 = region.y + height;
		newNode.width = width;
		mNodes.insert( mNodes.begin() + best_index, newNode );

		for ( i = best_index + 1; i < mNodes.size(); ++i ) {
			Node& node = mNodes[i];
			Node& prev = mNodes[i-1];

			if ( node.x < (prev.x + prev.width) ) {
				int shrink = prev.x + prev.width - node.x;
				node.x	 += shrink;
				node.width -= shrink;
				if ( node.width <= 0 ) {
					mNodes.erase( mNodes.begin() + i );
					--i;
				}
				else {
					break;
				}
			}
			else {
				break;
			}
		}
		merge();
		mUsed += width * height;
		return Area( region.x, region.y, region.x + region.width, region.y + region.height );
	}

};

class BinaryTreePack : public BinPack
{
  public:
	BinaryTreePack(int width, int height)
	{
		mRoot = new Node(Area(0, 0, width, height));
	}

	virtual ~BinaryTreePack()
	{
		delete mRoot;
	}

	virtual Area allocateArea( int width, int height )
	{
		Node* node = findNode( mRoot, width, height );
		if ( node ) {
			node->split(width, height);
			return node->area;
		}

		return Area(-1, -1, -1, -1);
	}

  private:
	struct Node
	{
		Node* below;
		Node* right;
		bool  available;
		Area  area;

		Node(const Area& area) : below(0), right(0), available(true), area(area) { }

		~Node()
		{
			delete below;
			delete right;
		}

		void split(int width, int height)
		{
			available = false;
			below = new Node(Area(area.x1,         area.y1 + height, area.x2, area.y2));
			right = new Node(Area(area.x1 + width, area.y1,          area.x2, area.y2));
		}
	};

	Node* findNode( Node* root, int width, int height )
	{
		if ( !root->available ) {
			Node* found = findNode( root->right, width, height );
			return found ? found : findNode(root->below, width, height);
		}
		else if (width <= root->area.getWidth() && height <= root->area.getHeight()) {
			return root;
		}

		return NULL;
	}

	Node *mRoot;
};

BinPackRef BinPack::create( int width, int height, PackType type )
{
	BinPackRef packer;

	switch (type) {
	  case DEFAULT:
	  case SKYLINE:
		packer = BinPackRef(new SkylinePack(width, height));
		break;
	  case BINARY_TREE:
		packer = BinPackRef(new BinaryTreePack(width, height));
		break;
	  default:
		break;
	}

	return packer;
}

}  // namespace cinder
