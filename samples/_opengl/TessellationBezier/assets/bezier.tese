#version 400

layout( isolines, equal_spacing ) in;

in vec4 TcsPosition[];
in vec4 TcsColor[];

out vec4 TesColor;

void main ()
{
	vec4 p0 = TcsPosition[0];
	vec4 p1 = TcsPosition[1];
	vec4 p2 = TcsPosition[2];
	vec4 p3 = TcsPosition[3];		

	// calculate bezier basis functions
	float u = gl_TessCoord.x;
	float iu = 1.0 - u;
	float b0 = iu * iu * iu;
	float b1 = 3.0 * u * iu * iu;
	float b2 = 3.0 * u * u * iu;
	float b3 = u * u * u;

	gl_Position = b0 * p0 + b1 * p1 + b2 * p2 + b3 * p3;
	
	TesColor = TcsColor[0];
}