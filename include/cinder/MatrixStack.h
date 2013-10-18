#pragma once

#include "cinder/Matrix.h"

#include <vector>

namespace cinder {

class MatrixStack
{
   public:
	MatrixStack();
	void push();
	void push(const Matrix44f &matrix);
	void pop();
	Matrix44f &top();

  //private:
	std::vector<Matrix44f> mStack;
};

}