#pragma once

#include "cinder/Matrix.h"

#include <vector>

namespace cinder {

class MatrixStack
{
   public:
	MatrixStack();
	void push();
	void push(const mat4 &matrix);
	void pop();
	mat4 &top();

  //private:
	std::vector<mat4> mStack;
};

}