// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// References used:
// https://learnopengl.com
// ==========================================================================
#version 410

// location indices for these attributes correspond to those specified in the
// InitializeGeometry() function of the main program
layout(location = 0) in vec2 VertexPosition;
layout(location = 1) in vec2 VertexTexCoord;

// output to be interpolated between vertices and passed to the fragment stage
out vec2 TexCoord;

uniform mat4 transform;

void main()
{
    // assign vertex position without modification
    gl_Position = transform * vec4(VertexPosition, 0.0, 1.0);

    TexCoord = VertexTexCoord;
}
