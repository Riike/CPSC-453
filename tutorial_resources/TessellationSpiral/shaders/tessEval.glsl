#version 410
/**
* Tessellation Evaluation Shader
*	Determines positions of points in tessellated patches
*	Receives input from gl_in, tcs out variables and gl_TessCoord
*	Run once for every vertex in the output patch
*/

//Type of patch being output
layout(isolines) in;

in vec3 teColour[];
//in gl_in[];

//Information being sent out to fragment shader
//Will be interpolated as if sent from vertex shader
out vec3 Colour;

#define PI 3.14159265359

void main()
{
	//gl_TessCoord.x will parameterize the segments of the line from 0 to 1
	//gl_TessCoord.y will parameterize the number of lines from 0 to 1
	float u = gl_TessCoord.x;

	vec3 startColour = teColour[0];
	vec3 endColour = teColour[1];

	vec2 p0 = gl_in[0].gl_Position.xy;
	vec2 p1 = gl_in[1].gl_Position.xy;

	//Forming basis from two points in line
	vec2 bx = p1-p0;
	//And line rotated 90 degrees
	vec2 by = vec2(bx.y, -bx.x);

	float cycles = 4.0;
	//Equation for spiral from first assignment
	vec2 position = u*cos(u*cycles*2.0*PI)*bx + u*sin(u*cycles*2.0*PI)*by + p0;

	gl_Position = vec4(position, 0, 1);

	Colour = (1-u)*startColour + u*endColour;

}
