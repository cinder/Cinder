#pragma once

#include "flint/Vector.h"
#include <vector>

// Mode: A&B   A|B   A-B   B-A
std::vector<std::vector<fli::Vec2f> > calculateIntersection( const std::vector<std::vector<fli::Vec2f> > &subject, const std::vector<std::vector<fli::Vec2f> > &clip, int mode );