#version 410
layout(isolines) in;

in vec3 teColour[];
//in gl_in[];

out vec3 Colour;

void main()
{
	float u = gl_TessCoord.x;

	vec3 startColour = teColour[0];
	vec3 endColour = teColour[1];

	float b0 = 1.0-u;
	float b1 = u;

	gl_Position = b0*gl_in[0].gl_Position +
					b1*gl_in[1].gl_Position;

	Colour = b0*startColour + b1*endColour;

}
