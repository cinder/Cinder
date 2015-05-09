#include "cinder/MatrixStack.h"

namespace cinder {

MatrixStack::MatrixStack()
{
	mStack.push_back(Matrix44f());
}

void MatrixStack::push()
{
	mStack.push_back(mStack.back());
}

void MatrixStack::push(const Matrix44f &matrix)
{
	mStack.push_back(mStack.back() * matrix);
}

void MatrixStack::pop()
{
	mStack.pop_back();
}

Matrix44f &MatrixStack::top()
{
	return mStack.back();
}

}