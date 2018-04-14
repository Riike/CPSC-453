// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

in vec2 texCoord;
in vec3 normalCoord;

// interpolated colour received from vertex stage
// uniform vec3 Colour;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

uniform sampler2D sunTexture;
uniform sampler2D earthTexture;
uniform sampler2D moonTexture;
uniform sampler2D starTexture;

uniform int nodeType;
uniform mat4 earthWorld;
uniform mat4 moonWorld;

void main(void)
{
    vec3 earthLightV = normalize(vec3(0, 0, 0) - vec3(earthWorld[3].x, earthWorld[3].y, earthWorld[3].z));
    vec3 moonLightV = normalize(vec3(0, 0, 0) - vec3(moonWorld[3].x, moonWorld[3].y, moonWorld[3].z));

	if(nodeType == 0)
        FragmentColour = vec4(texture(sunTexture, texCoord).rgb, 1);
	if(nodeType == 1)
        FragmentColour = vec4(texture(earthTexture, texCoord).rgb * dot(normalCoord, earthLightV), 1);
	if(nodeType == 2)
        FragmentColour = vec4(texture(moonTexture, texCoord).rgb * dot(normalCoord, moonLightV), 1);
	if(nodeType == 3)
        FragmentColour = vec4(texture(starTexture, texCoord).rgb, 1);
}
