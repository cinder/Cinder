#pragma once

#include "flint/Vector.h"
#include <vector>

std::vector<std::vector<fli::Vec2f> > loadPolygon( const std::string &path, float scale );
// Mode: A&B   A|B   A-B   A^B
std::vector<std::vector<fli::Vec2f> > calculateIntersection( const std::vector<std::vector<fli::Vec2f> > &subject, const std::vector<std::vector<fli::Vec2f> > &clip, int mode, std::vector<fli::Vec2f> *rawIntersections );