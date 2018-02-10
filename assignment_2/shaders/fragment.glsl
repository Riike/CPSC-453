// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// References used:
// https://github.com/mattdesl/lwjgl-basics/wiki/ShaderLesson3
// https://gist.github.com/Hebali/6ebfc66106459aacee6a9fac029d0115
// https://learnopengl.com
// ==========================================================================
#version 410

// interpolated colour received from vertex stage
in vec3 Colour;
in vec2 TexCoord;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

uniform sampler2D image;
uniform int effect;
uniform vec2 imgDimensions;

const vec3 lum1 = vec3(0.333, 0.333, 0.333);
const vec3 lum2 = vec3(0.299, 0.587, 0.114);
const vec3 lum3 = vec3(0.213, 0.715, 0.072);
const vec3 sepia = vec3(0.44, 0.26, 0.08);

void main(void)
{
    // Colour and texture variables
    vec4 origColour = texture(image, TexCoord);
    float greyscale1 = dot(lum1, origColour.rgb);
    float greyscale2 = dot(lum2, origColour.rgb);
    float greyscale3 = dot(lum3, origColour.rgb);
    vec3 sepiaTone = vec3(greyscale2) * sepia;

    // Edge Effect Variables
    float w = 1.0 / imgDimensions.x;
    float h = 1.0 / imgDimensions.y;

    vec2 kernel3[9];
    vec4 appliedFilter = vec4(0.0);

    float verticalSobel[9] = float[9](1.0f, 0.0f, -1.0f,
                                      2.0f, 0.0f, -2.0f,
                                      1.0f, 0.0f, -1.0f);

    float horizontalSobel[9] = float[9](-1.0f, -2.0f, -1.0f,
                                        0.0f, 0.0f, 0.0f,
                                        1.0f, 2.0f, 1.0f);

    float unsharpMask[9] = float[9](0.0f, -1.0f, 0.0f,
                                    -1.0f, 5.0f, -1.0f,
                                    0.0f, -1.0f, 0.0f);

    int k = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            kernel3[k] = vec2(j * w - w, h - i * h);
            k++;
        }
    }

    // Effects
    switch(effect) {
        case 0:
            FragmentColour = origColour; break;
        case 1:
            FragmentColour = vec4(greyscale1); break;
        case 2:
            FragmentColour = vec4(greyscale2); break;
        case 3:
            FragmentColour = vec4(greyscale3); break;
        case 4:
            origColour.rgb = mix(sepiaTone, origColour.rgb, 0.75);
            FragmentColour =  origColour;
            break;
        case 5:
            for (int i = 0; i < 9; i++) {
                appliedFilter += texture(image, TexCoord + kernel3[i]) * verticalSobel[i];
            }
            FragmentColour = appliedFilter;
            break;
        case 6:
            for (int i = 0; i < 9; i++) {
                appliedFilter += texture(image, TexCoord + kernel3[i]) * horizontalSobel[i];
            }
            FragmentColour = appliedFilter;
            break;
        case 7:
            for (int i = 0; i < 9; i++) {
                appliedFilter += texture(image, TexCoord + kernel3[i]) * unsharpMask[i];
            }
            FragmentColour = appliedFilter;
            break;
    }
}
