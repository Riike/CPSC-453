// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

in vec2 texCoord;

// interpolated colour received from vertex stage
// uniform vec3 Colour;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

uniform sampler2D sunTexture;
uniform sampler2D earthTexture;
uniform sampler2D moonTexture;
uniform int nodeType;

void main(void)
{
	if(nodeType == 0)
        FragmentColour = vec4(texture(sunTexture, texCoord).rgb, 1);
	if(nodeType == 1)
        FragmentColour = vec4(texture(earthTexture, texCoord).rgb, 1);
	if(nodeType == 2)
        FragmentColour = vec4(texture(moonTexture, texCoord).rgb, 1);
}
