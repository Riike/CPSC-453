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
// Modified to use as Assignment II
// Author: Henry Tran
// ==========================================================================

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <iterator>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "texture.h"
#include <vector>

using namespace std;
using namespace glm;
// --------------------------------------------------------------------------
// OpenGL utility and support function prototypes

void QueryGLVersion();
bool CheckGLErrors();

string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);

int img = 1;
bool newImg = false;

bool mouseClicked;
double initPosX, initPosY;
double offsetX = 0.0, offsetY = 0.0;
double prevPosX = 0.0, prevPosY = 0.0;
double xDist = 0.0, yDist = 0.0;
double magnification = 1.0;
float theta = 0.0f;
bool keyA = false, keyD = false;

int effect = 0;

// --------------------------------------------------------------------------
// Functions to set up OpenGL shader programs for rendering

// load, compile, and link shaders, returning true if successful
GLuint InitializeShaders()
{
	// load shader source from files
	string vertexSource = LoadSource("shaders/vertex.glsl");
	string fragmentSource = LoadSource("shaders/fragment.glsl");
	if (vertexSource.empty() || fragmentSource.empty()) return false;

	// compile shader source into shader objects
	GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	// link shader program
	GLuint program = LinkProgram(vertex, fragment);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	// check for OpenGL errors and return false if error occurred
	return program;
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

bool InitializeVAO(Geometry *geometry) {

	const GLuint VERTEX_INDEX = 0;
	const GLuint COLOUR_INDEX = 1;
	const GLuint TEXTURE_INDEX = 2;

	//Generate Vertex Buffer Objects
	// create an array buffer object for storing our vertices
	glGenBuffers(1, &geometry->vertexBuffer);

	// create another one for storing our colours
	glGenBuffers(1, &geometry->colourBuffer);

        // array buffer object for storing texture coordinates
        glGenBuffers(1, &geometry->textureBuffer);

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

        glBindBuffer(GL_ARRAY_BUFFER, geometry->textureBuffer);
        glVertexAttribPointer(
                TEXTURE_INDEX,
                2,
                GL_FLOAT,
                GL_FALSE,
                sizeof(vec2),
                0);
        glEnableVertexAttribArray(TEXTURE_INDEX);

	// unbind our buffers, resetting to default state
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return !CheckGLErrors();
}

void updateRotation() {
    if (keyA)
        theta += 5.0f;

    if (keyD)
        theta -= 5.0f;
}


void updateLocation(GLFWwindow* window, int winWidth, int winHeight) {

    double x, y;
    glfwGetCursorPos(window, &x, &y);

    if (mouseClicked) {
        xDist = x - initPosX + prevPosX;
        yDist = y - initPosY + prevPosY;
    } else {
        prevPosX = xDist;
        prevPosY = yDist;
    }

    offsetX = xDist / winWidth / 2;
    offsetY = -yDist / winHeight / 2;
}

void generateImage(vector<vec2>* points, vector<vec2>* texCoords, float width, float height) {

        points->clear();
        texCoords->clear();

        if (height >= width) {
            float normalizedW = width/height;
            //Triangle 1
            points->push_back(vec2(-normalizedW, 1.0f));
            points->push_back(vec2(normalizedW, 1.0f));
            points->push_back(vec2(-normalizedW, -1.0f));

            //Triangle 2
            points->push_back(vec2(normalizedW, -1.0f));
            points->push_back(vec2(normalizedW, 1.0f));
            points->push_back(vec2(-normalizedW, -1.0f));
        } else {
            float normalizedH = height/width;
            //Triangle 1
            points->push_back(vec2(-1.0f, normalizedH));
            points->push_back(vec2(1.0f, normalizedH));
            points->push_back(vec2(-1.0f, -normalizedH));

            //Triangle 2
            points->push_back(vec2(1.0f, -normalizedH));
            points->push_back(vec2(1.0f, normalizedH));
            points->push_back(vec2(-1.0f, -normalizedH));
        }

        texCoords->push_back(vec2(0.0f,1.0f));
        texCoords->push_back(vec2(1.0f,1.0f));
        texCoords->push_back(vec2(0.0f,0.0f));

        texCoords->push_back(vec2(1.0f,0.0f));
        texCoords->push_back(vec2(1.0f,1.0f));
        texCoords->push_back(vec2(0.0f,0.0f));
}


// create buffers and fill with geometry data, returning true if successful
bool LoadGeometry(Geometry *geometry, vec2 *vertices, vec3 *colours, vec2 *texCoords, int elementCount)
{
	geometry->elementCount = elementCount;

	// create an array buffer object for storing our vertices
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*geometry->elementCount, vertices, GL_STATIC_DRAW);

	// create another one for storing our colours
	glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*geometry->elementCount, colours, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, geometry->textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*geometry->elementCount, texCoords, GL_STATIC_DRAW);

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

void RenderScene(Geometry *geometry, GLuint program)
{
	// clear screen to a dark grey colour
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram(program);
	glBindVertexArray(geometry->vertexArray);
        // printf("%d\n", geometry->elementCount);
        glDrawArrays(GL_TRIANGLES, 0, geometry->elementCount);

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

        if (key == GLFW_KEY_RIGHT) {
            if (img == 5)
                img = 1;
            else
                img++;
            newImg = true;
        }

        if (key == GLFW_KEY_LEFT) {
            if (img == 1)
                img = 5;
            else
                img--;
            newImg = true;
        }

        if (key == GLFW_KEY_A)
            keyA = true;

        if (key == GLFW_KEY_D)
            keyD = true;

        if (key == GLFW_KEY_ENTER)
            effect = 0;

        if (key == GLFW_KEY_1)
            effect = 1;

        if (key == GLFW_KEY_2)
            effect = 2;

        if (key == GLFW_KEY_3)
            effect = 3;

        if (key == GLFW_KEY_4)
            effect = 4;

        if (key == GLFW_KEY_5)
            effect = 5;

        if (key == GLFW_KEY_6)
            effect = 6;

        if (key == GLFW_KEY_7)
            effect = 7;

        if (key == GLFW_KEY_8)
            effect = 8;

        if (key == GLFW_KEY_9)
            effect = 9;

        if (key == GLFW_KEY_0)
            effect = 10;
    }

    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_A)
            keyA = false;

        if (key == GLFW_KEY_D)
            keyD = false;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwGetCursorPos(window, &initPosX, &initPosY);
        printf("I am clicked on %f, %f\n", initPosX, initPosY);
        mouseClicked = true;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        printf("I am released\n");
        mouseClicked = false;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

    magnification += yoffset * 0.05;

    if (magnification < 0.05) {
        magnification = 0.05;
    }
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
	int width = 512, height = 512;
	window = glfwCreateWindow(width, height, "CPSC 453 Assignment II", 0, 0);
	if (!window) {
		cout << "Program failed to create GLFW window, TERMINATING" << endl;
		glfwTerminate();
		return -1;
	}

	// set keyboard callback function and make our context current (active)
	glfwSetKeyCallback(window, KeyCallback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetScrollCallback(window, scroll_callback);

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
	GLuint program = InitializeShaders();
	if (program == 0) {
		cout << "Program could not initialize shaders, TERMINATING" << endl;
		return -1;
	}

        vector<vec2> points;
        vector<vec3> colours;
        vector<vec2> texCoords;

        MyTexture texture;
        string fileName = "images/image1-mandrill.png";

        InitializeTexture(&texture, fileName.c_str(), GL_TEXTURE_2D);

	// call function to create and fill buffers with geometry data
	Geometry geometry;
	if (!InitializeVAO(&geometry))
		cout << "Program failed to intialize geometry!" << endl;

	if(!LoadGeometry(&geometry, points.data(),
                    colours.data(), texCoords.data(), points.size()))
		cout << "Failed to load geometry" << endl;

	// float timeElapsed = 0.f;
	// GLint timeLocation = glGetUniformLocation(program, "time");
        GLint transformLoc = glGetUniformLocation(program, "transform");
        GLint effectLoc = glGetUniformLocation(program, "effect");
        GLint dimensionsLoc = glGetUniformLocation(program, "imgDimensions");

	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window))
	{

                mat4 trans;
                trans = rotate(trans,
                        radians(theta), glm::vec3(0.0, 0.0, 1.0));
                trans = scale(trans,
                        vec3((float)magnification, (float)magnification, 0.0f));
                trans = translate(trans,
                        vec3((float)offsetX, (float)offsetY, 0.0f));

                glUseProgram(program);
                glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
                glUniform1i(effectLoc, effect);
                glUniform2f(dimensionsLoc, (float)texture.width, (float)texture.height);

                switch(img) {
                    case 1: fileName = "images/image1-mandrill.png"; break;
                    case 2: fileName = "images/image2-uclogo.png"; break;
                    case 3: fileName = "images/image3-aerial.jpg"; break;
                    case 4: fileName = "images/image4-thirsk.jpg"; break;
                    case 5: fileName = "images/image5-pattern.png"; break;
                }

                if (newImg) {
                    InitializeTexture(&texture, fileName.c_str(), GL_TEXTURE_2D);
                    newImg = false;
                }

                generateImage(&points, &texCoords, texture.width, texture.height);
                LoadGeometry(&geometry, points.data(),
                        colours.data(), texCoords.data(), points.size());

                updateLocation(window, width, height);
                updateRotation();

                glBindTexture(GL_TEXTURE_2D, texture.textureID);

		// call function to draw our scene
		RenderScene(&geometry, program);

		// timeElapsed += 0.01f;

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	// clean up allocated resources before exit
	DestroyGeometry(&geometry);
	glUseProgram(0);
	glDeleteProgram(program);
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
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
	// allocate program object name
	GLuint programObject = glCreateProgram();

	// attach provided shader objects to this program
	if (vertexShader)   glAttachShader(programObject, vertexShader);
	if (fragmentShader) glAttachShader(programObject, fragmentShader);

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
