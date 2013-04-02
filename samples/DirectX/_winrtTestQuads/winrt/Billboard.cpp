#include "Billboard.h"

Billboard::Billboard(cinder::Vec3f _position, float _xRotation, float _yRotation)
{
		xRotation = _xRotation;
		position = _position;
		xStep = _xRotation*.1f;
		yStep = _yRotation*.1f;
		life = 0;
}


void Billboard::Update()
{
	xRotation += xStep;
	yRotation += yStep;
}
