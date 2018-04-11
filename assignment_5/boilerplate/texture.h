#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// --------------------------------------------------------------------------
// Functions to set up OpenGL buffers for storing textures

struct MyTexture
{
	GLuint textureID;	//Handle for OpenGL texture object
	GLenum target;		//Type of texture eg:: GL_TEXTURE_2D or GL_TEXTURE_RECTANGLE
	int width;			
	int height;

	// initialize object names to zero (OpenGL reserved value)
	MyTexture();
};

//Function to create a texture from an image file
//Does several things:
//	Uses stb_image to extract bytes from a file
//	Creates OpenGL handle for texture object
//	Sets default behaviour texture, wrapping behaviour, etc...
//		You may want to change this depending on your needs
//	Loads bytes into texture object
// ARGS:
//	texture - Properties of created texture is returned here
//	filename - Name of image file to create texture from
//	target - Type of texture generated, eg GL_TEXTURE_2D and GL_TEXTURE_RECTANGLE
bool InitializeTexture(MyTexture* texture, const char* filename, GLenum target = GL_TEXTURE_2D);

// deallocate texture-related objects
void DestroyTexture(MyTexture *texture);