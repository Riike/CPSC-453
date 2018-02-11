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
in vec2 TexCoord;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

uniform sampler2D image;
uniform int effect;
uniform vec2 imgDimensions;

void main(void)
{
    // Colour and texture variables
    vec3 lum1 = vec3(0.333, 0.333, 0.333);
    vec3 lum2 = vec3(0.299, 0.587, 0.114);
    vec3 lum3 = vec3(0.213, 0.715, 0.072);
    vec3 sepia = vec3(0.44, 0.26, 0.08);

    vec4 origColour = texture(image, TexCoord);
    float greyscale1 = dot(lum1, origColour.rgb);
    float greyscale2 = dot(lum2, origColour.rgb);
    float greyscale3 = dot(lum3, origColour.rgb);
    vec3 sepiaTone = vec3(greyscale2) * sepia;

    // Edge Effect Variables
    float w = 1.0 / imgDimensions.x;
    float h = 1.0 / imgDimensions.y;

    vec2 kernel3[9];
    vec2 kernel5[25];
    vec2 kernel7[49];

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

    float gaussian3[3] = float[3](0.2, 0.6, 0.2);
    float gaussian5[5] = float[5](0.06, 0.24, 0.4, 0.24, 0.06);
    float gaussian7[7] = float[7](0.03, 0.11, 0.22, 0.28, 0.22, 0.11, 0.03);

    float gaussMat3[9];
    float gaussMat5[25];
    float gaussMat7[49];

    int k = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            kernel3[k] = vec2(j * w - w, h - i * h);
            gaussMat3[k] = gaussian3[i] * gaussian3[j];
            k++;
        }
    }

    k = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            kernel5[k] = vec2(j * w - 2 * w, 2 * h - i * h);
            gaussMat5[k] = gaussian5[i] * gaussian5[j];
            k++;
        }
    }

    k = 0;
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            kernel7[k] = vec2(j * w - 3 * w, 3 * h - i * h);
            gaussMat7[k] = gaussian7[i] * gaussian7[j];
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
        case 8:
            for (int i = 0; i < 9; i++) {
                appliedFilter += texture(image, TexCoord + kernel3[i]) * gaussMat3[i];
            }
            FragmentColour = appliedFilter;
            break;
        case 9:
            for (int i = 0; i < 25; i++) {
                appliedFilter += texture(image, TexCoord + kernel5[i]) * gaussMat5[i];
            }
            FragmentColour = appliedFilter;
            break;
        case 10:
            for (int i = 0; i < 49; i++) {
                appliedFilter += texture(image, TexCoord + kernel7[i]) * gaussMat7[i];
            }
            FragmentColour = appliedFilter;
            break;
    }
}
