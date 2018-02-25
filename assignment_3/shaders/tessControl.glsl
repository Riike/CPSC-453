#version 410
/**
*Tessellation Control Shader
*	Determines level of subdivision for the generated patch
*	as well as determining which information is passed on
*	to the next shader stage
*	Run once for every vertex in input patch
*/

//This variable must match the patch size set in c++ program with
//glPatchParameteri(GL_PATCH_VERTICES_VERTICES, n)
layout(vertices=2) out;

//Number of elements equal to patch size
in vec3 tcColour[];	//From vertex shader
out vec3 teColour[];	//To fragment shader

//Variables which are implicitly included in every tess control shader
//Struct containing gl_Position, gl_PointSize, and something else you'll probably never use
//in gl_in[];
//Structs containing the same information which can be written to to send to Tess Eval shader
//out gl_out[];

void main()
{
	//gl_InvocationID says which vertex in the patch you are processing
	if(gl_InvocationID == 0)
	{
		gl_TessLevelOuter[0] = 1;	//Determines number of lines
		gl_TessLevelOuter[1] = 64;	//Determines number of segments in line
	}

	//Passing information along to tessEval.glsl
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	teColour[gl_InvocationID] = tcColour[gl_InvocationID];
}
