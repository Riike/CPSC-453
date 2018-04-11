// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// location indices for these attributes correspond to those specified in the
// InitializeGeometry() function of the main program
layout(location = 0) in vec3 VertexPosition;

uniform mat4 modelViewProjection;

void main()
{
    // assign vertex position without modification
    gl_Position = modelViewProjection*vec4(VertexPosition, 1.0);
}
