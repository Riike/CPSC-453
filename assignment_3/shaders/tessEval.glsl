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

uniform int degree;

#define PI 3.14159265359

void main()
{
	//gl_TessCoord.x will parameterize the segments of the line from 0 to 1
	//gl_TessCoord.y will parameterize the number of lines from 0 to 1
	float b0 = gl_TessCoord.x;
        float b1 = 1.0f - b0;

	vec3 startColour = teColour[0];
	vec3 endColour = teColour[1];
        vec2 position;

	vec2 p0 = gl_in[0].gl_Position.xy;
	vec2 p1 = gl_in[1].gl_Position.xy;
        vec2 p2 = gl_in[2].gl_Position.xy;
        vec2 p3 = gl_in[3].gl_Position.xy;

	//Equations for bezier bernstein form
        if (degree == 3)
            position = pow(b1, 2) * p0 + 2 * b0 * b1 * p1 + pow(b0, 2) * p2;
        else
            position = pow(b1, 3) * p0 + 3 * b0 * pow(b1, 2) * p1 +
                       3 * pow(b0, 2) * b1 * p2 + pow(b0, 3) * p3;

        gl_Position = vec4(position, 0, 1);

        Colour = vec3(1, 1, 1);
}
