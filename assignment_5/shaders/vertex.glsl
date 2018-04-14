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
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

out vec2 texCoord;
out vec3 normalCoord;

uniform mat4 modelViewProjection;
uniform mat4 model;

void main()
{
    // assign vertex position without modification
    gl_Position = modelViewProjection * vec4(VertexPosition, 1.0);
    texCoord = uv;
    normalCoord = normalize((model * vec4(normal, 0.f)).xyz);
}
