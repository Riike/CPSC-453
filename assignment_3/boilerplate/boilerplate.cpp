// ==========================================================================
// Barebones OpenGL Core Profile Boilerplate
//    using the GLFW windowing system (http://www.glfw.org)
//
// Loosely based on
//  - Chris Wellons' example (https://github.com/skeeto/opengl-demo) and
//  - Camilla Berglund's example (http://www.glfw.org/docs/latest/quick.html)
//
// Author:  Sonny Chan, University of Calgary
// Co-Authors:
//			Jeremy Hart, University of Calgary
//			John Hall, University of Calgary
// Date:    December 2015
// Assignment 3
// Modified by: Henry Tran
// ==========================================================================

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <iterator>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "texture.h"
#include "GlyphExtractor.h"

using namespace std;
using namespace glm;
// --------------------------------------------------------------------------
// OpenGL utility and support function prototypes

void QueryGLVersion();
bool CheckGLErrors();

string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint tcsShader, GLuint tesShader);

int scene = 1;
int font = 1;
int scrollFont = 1;
int degree = 3;

// --------------------------------------------------------------------------
// Functions to set up OpenGL shader programs for rendering

// load, compile, and link shaders, returning true if successful
void InitializeShaders(GLuint* program1, GLuint* program2)
{
	// load shader source from files
	string vertexSource = LoadSource("shaders/vertex.glsl");
	string fragmentSource = LoadSource("shaders/fragment.glsl");
	string tcsSource = LoadSource("shaders/tessControl.glsl");
	string tesSource = LoadSource("shaders/tessEval.glsl");

        string vertexSource2 = LoadSource("shaders/vertex2.glsl");
        string fragmentSource2 = LoadSource("shaders/fragment2.glsl");

	if (vertexSource.empty() || fragmentSource.empty()) return;
        if (vertexSource2.empty() || fragmentSource2.empty()) return;

	// compile shader source into shader objects
	GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	GLuint tcs = CompileShader(GL_TESS_CONTROL_SHADER, tcsSource);
	GLuint tes = CompileShader(GL_TESS_EVALUATION_SHADER, tesSource);

        GLuint vertex2 = CompileShader(GL_VERTEX_SHADER, vertexSource2);
        GLuint fragment2 = CompileShader(GL_FRAGMENT_SHADER, fragmentSource2);

	if (CheckGLErrors())
		return;

	// link shader program
        *program1 = LinkProgram(vertex, fragment, tcs, tes);
        *program2 = LinkProgram(vertex2, fragment2, 0, 0);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
        glDeleteShader(vertex2);
        glDeleteShader(fragment2);
	glDeleteShader(tcs);
	glDeleteShader(tes);
}

// --------------------------------------------------------------------------
// Functions to set up OpenGL buffers for storing geometry data

struct Geometry
{
	// OpenGL names for array buffer objects, vertex array object
	GLuint  vertexBuffer;
	GLuint  textureBuffer;
	GLuint  colourBuffer;
	GLuint  vertexArray;
	GLsizei elementCount;

	// initialize object names to zero (OpenGL reserved value)
	Geometry() : vertexBuffer(0), colourBuffer(0), vertexArray(0), elementCount(0)
	{}
};

bool InitializeVAO(Geometry *geometry){

	const GLuint VERTEX_INDEX = 0;
	const GLuint COLOUR_INDEX = 1;

	//Generate Vertex Buffer Objects
	// create an array buffer object for storing our vertices
	glGenBuffers(1, &geometry->vertexBuffer);

	// create another one for storing our colours
	glGenBuffers(1, &geometry->colourBuffer);

	//Set up Vertex Array Object
	// create a vertex array object encapsulating all our vertex attributes
	glGenVertexArrays(1, &geometry->vertexArray);
	glBindVertexArray(geometry->vertexArray);

	// associate the position array with the vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glVertexAttribPointer(
		VERTEX_INDEX,		//Attribute index
		2, 					//# of components
		GL_FLOAT, 			//Type of component
		GL_FALSE, 			//Should be normalized?
		sizeof(vec2),		//Stride - can use 0 if tightly packed
		0);					//Offset to first element
	glEnableVertexAttribArray(VERTEX_INDEX);

	// associate the colour array with the vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
	glVertexAttribPointer(
		COLOUR_INDEX,		//Attribute index
		3, 					//# of components
		GL_FLOAT, 			//Type of component
		GL_FALSE, 			//Should be normalized?
		sizeof(vec3), 		//Stride - can use 0 if tightly packed
		0);					//Offset to first element
	glEnableVertexAttribArray(COLOUR_INDEX);

	// unbind our buffers, resetting to default state
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return !CheckGLErrors();
}

// create buffers and fill with geometry data, returning true if successful
bool LoadGeometry(Geometry *geometry, vec2 *vertices, vec3 *colours, int elementCount)
{
	geometry->elementCount = elementCount;

	// create an array buffer object for storing our vertices
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*geometry->elementCount,
                     vertices, GL_STATIC_DRAW);

	// create another one for storing our colours
	glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*geometry->elementCount,
                     colours, GL_STATIC_DRAW);

	//Unbind buffer to reset to default state
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// check for OpenGL errors and return false if error occurred
	return !CheckGLErrors();
}

// deallocate geometry-related objects
void DestroyGeometry(Geometry *geometry)
{
	// unbind and destroy our vertex array object and associated buffers
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &geometry->vertexArray);
	glDeleteBuffers(1, &geometry->vertexBuffer);
	glDeleteBuffers(1, &geometry->colourBuffer);
}

// --------------------------------------------------------------------------
// Rendering function that draws our scene to the frame buffer

void RenderScene(Geometry *geometry, GLuint program1, GLuint program2)
{
	// clear screen to a dark grey colour
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
        glBindVertexArray(geometry->vertexArray);

        if (scene == 1) {
            glUseProgram(program2);
            glPointSize(8.0f);
            glDrawArrays(GL_POINTS, 0, geometry->elementCount);
            glDrawArrays(GL_LINE_STRIP, 0, geometry->elementCount);
        }

        glUseProgram(program1);
        GLint degreeLoc = glGetUniformLocation(program1, "degree");
        glPatchParameteri(GL_PATCH_VERTICES, degree);
        glUniform1i(degreeLoc, degree);
        glDrawArrays(GL_PATCHES, 0, geometry->elementCount);

	// reset state to default (no shader or geometry bound)
	glBindVertexArray(0);
	glUseProgram(0);

	// check for an report any OpenGL errors
	CheckGLErrors();
}

// --------------------------------------------------------------------------
// GLFW callback functions

// reports GLFW errors
void ErrorCallback(int error, const char* description)
{
	cout << "GLFW ERROR " << error << ":" << endl;
	cout << description << endl;
}

// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
	if (key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, GL_TRUE);

        if (key == GLFW_KEY_UP) {
            if (scene == 1) {
                if (degree == 3)
                   degree++;
                else
                    degree = 3;
            }

            if (scene == 2) {
                if (font == 3)
                    font = 1;
                else
                    font++;
            }
        }

        if (key == GLFW_KEY_DOWN) {
            if (scene == 1) {
                if (degree == 4)
                   degree--;
                else
                    degree = 4;
            }

            if (scene == 2) {
                if (font == 1)
                    font = 3;
                else
                    font--;
            }
        }

        if (key == GLFW_KEY_RIGHT) {
            if (scene == 3)
                scene = 1;
            else
                scene++;
        }

        if (key == GLFW_KEY_LEFT) {
            if (scene == 1)
                scene = 3;
            else
                scene--;
        }
    }
}

// --------------------------------------------------------------------------
// Functions to aid generating the bezier curves

void generateQuadratics(vector<vec2>* points, vector<vec3>* colours) {
    points->clear();
    colours->clear();

    points->push_back(vec2(0.4f, 0.4f));
    points->push_back(vec2(0.8f, -0.4f));
    points->push_back(vec2(0.0f, -0.4f));

    points->push_back(vec2(0.0f, -0.4f));
    points->push_back(vec2(-0.8f, -0.4f));
    points->push_back(vec2(-0.4f, 0.4f));

    points->push_back(vec2(-0.4f, 0.4f));
    points->push_back(vec2(0.0f, 0.4f));
    points->push_back(vec2(0.4f, 0.4f));

    points->push_back(vec2(0.48f, 0.2f));
    points->push_back(vec2(1.0f, 0.4f));
    points->push_back(vec2(0.52f, -0.16f));

    for (int i = 0; i < 4; i++) {
        colours->push_back(vec3(1, 0, 0));
        colours->push_back(vec3(0, 1, 0));
        colours->push_back(vec3(1, 0, 0));
    }
}

void generateCubics(vector<vec2>* points, vector<vec3>* colours) {
    points->clear();
    colours->clear();

    points->push_back(vec2(0.1f * 2 - 0.9f, 0.1f * 2 - 0.3f));
    points->push_back(vec2(0.4f * 2 - 0.9f, 0.0f * 2 - 0.3f));
    points->push_back(vec2(0.6f * 2 - 0.9f, 0.2f * 2 - 0.3f));
    points->push_back(vec2(0.9f * 2 - 0.9f, 0.1f * 2 - 0.3f));

    points->push_back(vec2(0.8f * 2 - 0.9f, 0.2f * 2 - 0.3f));
    points->push_back(vec2(0.0f * 2 - 0.9f, 0.8f * 2 - 0.3f));
    points->push_back(vec2(0.0f * 2 - 0.9f, -0.2f * 2 - 0.3f));
    points->push_back(vec2(0.8f * 2 - 0.9f, 0.4f * 2 - 0.3f));

    points->push_back(vec2(0.5f * 2 - 0.9f, 0.3f * 2 - 0.3f));
    points->push_back(vec2(0.3f * 2 - 0.9f, 0.2f * 2 - 0.3f));
    points->push_back(vec2(0.3f * 2 - 0.9f, 0.3f * 2 - 0.3f));
    points->push_back(vec2(0.5f * 2 - 0.9f, 0.2f * 2 - 0.3f));

    points->push_back(vec2(0.3f * 2 - 0.9f, 0.22f * 2 - 0.3f));
    points->push_back(vec2(0.35f * 2 - 0.9f, 0.27f * 2 - 0.3f));
    points->push_back(vec2(0.35f * 2 - 0.9f, 0.33f * 2 - 0.3f));
    points->push_back(vec2(0.3f * 2 - 0.9f, 0.38f * 2 - 0.3f));

    points->push_back(vec2(0.28f * 2 - 0.9f, 0.35f * 2 - 0.3f));
    points->push_back(vec2(0.24f * 2 - 0.9f, 0.38f * 2 - 0.3f));
    points->push_back(vec2(0.24f * 2 - 0.9f, 0.32f * 2 - 0.3f));
    points->push_back(vec2(0.28f * 2 - 0.9f, 0.35f * 2 - 0.3f));

    for (int i = 0; i < 5; i++) {
        colours->push_back(vec3(1, 0, 0));
        colours->push_back(vec3(0, 1, 0));
        colours->push_back(vec3(0, 1, 0));
        colours->push_back(vec3(1, 0, 0));
    }
}

// https://stackoverflow.com/questions/20446201
bool has_suffix(const string &str, const string &suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

float findMidpoint(float a, float b) {
    return (a + b) / 2.0f;
}

void generateLetter(MyGlyph glyph, vector<vec2>* points,
                    vector<vec3>* colours, float adv, float ratio) {
    float xBegin,yBegin;
    float xEnd, yEnd;
    float xMid, yMid;

    for (size_t i = 0; i < glyph.contours.size(); i++) {
        for (size_t j = 0; j < glyph.contours.at(i).size(); j++) {
            if (glyph.contours.at(i).at(j).degree == 1) {
                xBegin = glyph.contours.at(i).at(j).x[0] * ratio;
                yBegin = glyph.contours.at(i).at(j).y[0] * ratio;
                xEnd = glyph.contours.at(i).at(j).x[1] * ratio;
                yEnd = glyph.contours.at(i).at(j).y[1] * ratio;
                xMid = findMidpoint(xBegin, xEnd);
                yMid = findMidpoint(yBegin, yEnd);

                points->push_back(vec2(xBegin - adv, yBegin - 0.2f));
                points->push_back(vec2(xMid - adv, yMid - 0.2f));
                if (degree == 4)
                    points->push_back(vec2(xMid - adv, yMid - 0.2f));
                points->push_back(vec2(xEnd - adv, yEnd - 0.2f));

            } else {
                for (int k = 0; k < degree; k++) {
                    xBegin = glyph.contours.at(i).at(j).x[k] * ratio;
                    yBegin = glyph.contours.at(i).at(j).y[k] * ratio;
                    points->push_back(vec2(xBegin - adv, yBegin - 0.2f));
                }
            }

            for (int k = 0; k < degree; k++)
                colours->push_back(vec3(1.0f, 1.0f, 1.0f));
        }
    }
}

void generateNameLora(vector<vec2>* points, vector<vec3>* colours) {
    points->clear();
    colours->clear();

    degree = 3;

    GlyphExtractor ge;
    if (!ge.LoadFontFile("fonts/Lora-Regular.ttf"))
        return;

    generateLetter(ge.ExtractGlyph('H'), points, colours, 1.0f, 0.7);
    generateLetter(ge.ExtractGlyph('e'), points, colours, 0.5f, 0.7);
    generateLetter(ge.ExtractGlyph('n'), points, colours, 0.15f, 0.7);
    generateLetter(ge.ExtractGlyph('r'), points, colours, -0.275f, 0.7);
    generateLetter(ge.ExtractGlyph('y'), points, colours, -0.6f, 0.7);
}
void generateNameSourceSans(vector<vec2>* points, vector<vec3>* colours) {
    points->clear();
    colours->clear();

    degree = 4;

    GlyphExtractor ge;
    if (!ge.LoadFontFile("fonts/SourceSansPro-Regular.otf"))
        return;

    generateLetter(ge.ExtractGlyph('H'), points, colours, 1.0f, 0.85);
    generateLetter(ge.ExtractGlyph('e'), points, colours, 0.5f, 0.85);
    generateLetter(ge.ExtractGlyph('n'), points, colours, 0.125f, 0.85);
    generateLetter(ge.ExtractGlyph('r'), points, colours, -0.275f, 0.85);
    generateLetter(ge.ExtractGlyph('y'), points, colours, -0.6f, 0.85);
}
void generateNameInconsolata(vector<vec2>* points, vector<vec3>* colours) {
    points->clear();
    colours->clear();

    degree = 4;

    GlyphExtractor ge;
    if (!ge.LoadFontFile("fonts/Inconsolata.otf"))
        return;

    generateLetter(ge.ExtractGlyph('H'), points, colours, 1.0f, 0.85);
    generateLetter(ge.ExtractGlyph('e'), points, colours, 0.6f, 0.85);
    generateLetter(ge.ExtractGlyph('n'), points, colours, 0.2f, 0.85);
    generateLetter(ge.ExtractGlyph('r'), points, colours, -0.2f, 0.85);
    generateLetter(ge.ExtractGlyph('y'), points, colours, -0.6f, 0.85);
}

// ==========================================================================
// PROGRAM ENTRY POINT

int main(int argc, char *argv[])
{
	// initialize the GLFW windowing system
	if (!glfwInit()) {
		cout << "ERROR: GLFW failed to initialize, TERMINATING" << endl;
		return -1;
	}
	glfwSetErrorCallback(ErrorCallback);

	// attempt to create a window with an OpenGL 4.1 core profile context
	GLFWwindow *window = 0;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	int width = 512, height = 512;
	window = glfwCreateWindow(width, height, "CPSC 453 Assignment 3", 0, 0);
	if (!window) {
		cout << "Program failed to create GLFW window, TERMINATING" << endl;
		glfwTerminate();
		return -1;
	}

	// set keyboard callback function and make our context current (active)
	glfwSetKeyCallback(window, KeyCallback);
	glfwMakeContextCurrent(window);

	//Intialize GLAD
	if (!gladLoadGL())
	{
		cout << "GLAD init failed" << endl;
		return -1;
	}

	// query and print out information about our OpenGL environment
	QueryGLVersion();

	// call function to load and compile shader programs
        GLuint program1;
        GLuint program2;

        vector<vec2> points;
        vector<vec3> colours;

        InitializeShaders(&program1, &program2);

	if (program1 == 0 || program2 == 0) {
		cout << "Program could not initialize shaders, TERMINATING" << endl;
		return -1;
	}

	// call function to create and fill buffers with geometry data
	Geometry geometry;
	if (!InitializeVAO(&geometry))
		cout << "Program failed to intialize geometry!" << endl;

	if(!LoadGeometry(&geometry, points.data(), colours.data(), points.size()))
		cout << "Failed to load geometry" << endl;

	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window))
	{

            if (scene == 1) {
                if (degree == 3)
                    generateQuadratics(&points, &colours);
                else
                    generateCubics(&points, &colours);
            } else if (scene == 2) {
                if (font == 1)
                    generateNameLora(&points, &colours);
                if (font == 2)
                    generateNameSourceSans(&points, &colours);
                if (font == 3)
                    generateNameInconsolata(&points, &colours);
            }

            LoadGeometry(&geometry, points.data(), colours.data(), points.size());

	    // call function to draw our scene
            RenderScene(&geometry, program1, program2);

	    glfwSwapBuffers(window);

            glfwPollEvents();
	}


	// clean up allocated resources before exit
	DestroyGeometry(&geometry);
	glUseProgram(0);
	glDeleteProgram(program1);
        glDeleteProgram(program2);
	glfwDestroyWindow(window);
	glfwTerminate();

	cout << "Goodbye!" << endl;
	return 0;
}

// ==========================================================================
// SUPPORT FUNCTION DEFINITIONS

// --------------------------------------------------------------------------
// OpenGL utility functions

void QueryGLVersion()
{
	// query opengl version and renderer information
	string version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	string glslver = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

	cout << "OpenGL [ " << version << " ] "
		<< "with GLSL [ " << glslver << " ] "
		<< "on renderer [ " << renderer << " ]" << endl;
}

bool CheckGLErrors()
{
	bool error = false;
	for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
	{
		cout << "OpenGL ERROR:  ";
		switch (flag) {
		case GL_INVALID_ENUM:
			cout << "GL_INVALID_ENUM" << endl; break;
		case GL_INVALID_VALUE:
			cout << "GL_INVALID_VALUE" << endl; break;
		case GL_INVALID_OPERATION:
			cout << "GL_INVALID_OPERATION" << endl; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
		case GL_OUT_OF_MEMORY:
			cout << "GL_OUT_OF_MEMORY" << endl; break;
		default:
			cout << "[unknown error code]" << endl;
		}
		error = true;
	}
	return error;
}

// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
string LoadSource(const string &filename)
{
	string source;

	ifstream input(filename.c_str());
	if (input) {
		copy(istreambuf_iterator<char>(input),
			istreambuf_iterator<char>(),
			back_inserter(source));
		input.close();
	}
	else {
		cout << "ERROR: Could not load shader source from file "
			<< filename << endl;
	}

	return source;
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source)
{
	// allocate shader object name
	GLuint shaderObject = glCreateShader(shaderType);

	// try compiling the source as a shader of the given type
	const GLchar *source_ptr = source.c_str();
	glShaderSource(shaderObject, 1, &source_ptr, 0);
	glCompileShader(shaderObject);

	// retrieve compile status
	GLint status;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
		cout << "ERROR compiling shader:" << endl << endl;
		cout << source << endl;
		cout << info << endl;
	}

	return shaderObject;
}

// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint tcsShader, GLuint tesShader)
{
	// allocate program object name
	GLuint programObject = glCreateProgram();

	// attach provided shader objects to this program
	if (vertexShader)   glAttachShader(programObject, vertexShader);
	if (fragmentShader) glAttachShader(programObject, fragmentShader);
	if (tcsShader) glAttachShader(programObject, tcsShader);
	if (tesShader) glAttachShader(programObject, tesShader);

	// try linking the program with given attachments
	glLinkProgram(programObject);

	// retrieve link status
	GLint status;
	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
		cout << "ERROR linking shader program:" << endl;
		cout << info << endl;
	}

	return programObject;
}
