// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// interpolated colour received from vertex stage
in vec3 Colour;
in vec2 TexCoord;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

uniform sampler2D image;

void main(void)
{
    // write colour output without modification
    FragmentColour = texture(image, TexCoord);
}
