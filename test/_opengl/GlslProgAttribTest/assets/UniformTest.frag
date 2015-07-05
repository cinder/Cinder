precision highp float;

uniform float ciElapsedSeconds;

uniform vec4 uColors[3];

struct TestStruct {
	float value;
};

uniform TestStruct uTestStruct[3];

in vec2 TexCoord0;
out vec4 oColor;

void main() {
	oColor = uColors[int(TexCoord0.s)];

#if 1
	// test setting a struct member uniform by const index
	oColor.r += uTestStruct[0].value * sin( ciElapsedSeconds * 1.91 + 5.5 ) * 0.5;
	oColor.g += uTestStruct[1].value * sin( ciElapsedSeconds * 3.31 + 6.2 ) * 0.5;
	oColor.b += uTestStruct[2].value * sin( ciElapsedSeconds * 2.34 + 9.9 ) * 0.5;
#else
	// test setting a struct member uniform in a for loop
	for( int i = 0; i < 3; i++ ) {
		oColor[i] = uTestStruct[i].value * sin( ciElapsedSeconds * 1.91 + 5.5 ) * 0.5;
	}

	// comment out one of these, it should be the only color you see
	oColor.r = 0;
	oColor.g = 0;
//	oColor.b = 0;
#endif
}