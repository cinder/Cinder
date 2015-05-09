#include "cinder/MatrixStack.h"

namespace cinder {

MatrixStack::MatrixStack()
{
	mStack.push_back( mat4() );
}

void MatrixStack::push()
{
	mStack.push_back(mStack.back());
}

void MatrixStack::push( const mat4 &matrix )
{
	mStack.push_back(mStack.back() * matrix);
}

void MatrixStack::pop()
{
	mStack.pop_back();
}

mat4& MatrixStack::top()
{
	return mStack.back();
}

}