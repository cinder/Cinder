#pragma once

#include "cinder/Cinder.h"
#include "cinder/Area.h"

namespace cinder {

class BinPack;
typedef std::shared_ptr<BinPack> BinPackRef;

class BinPack
{
public:
	enum PackType {
		DEFAULT     = 0,
		SKYLINE     = 1,
		BINARY_TREE = 2,
	};

	static BinPackRef create( int width, int height, PackType type = DEFAULT );

	virtual Area allocateArea( int width, int height ) = 0;
	virtual ~BinPack();
};

}

