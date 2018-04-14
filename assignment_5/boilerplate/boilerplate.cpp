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
// ==========================================================================

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <iterator>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "texture.h"
#include "Camera.h"

using namespace std;
using namespace glm;
// --------------------------------------------------------------------------
// OpenGL utility and support function prototypes

#define PI_F 3.14159265359f

void QueryGLVersion();
bool CheckGLErrors();

string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);

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
    GLuint  normalBuffer;
	GLuint  colourBuffer;
	GLuint  vertexArray;
	GLsizei elementCount;

	// initialize object names to zero (OpenGL reserved value)
	Geometry() : vertexBuffer(0), normalBuffer(0), colourBuffer(0), vertexArray(0), elementCount(0)
	{}
};

bool InitializeVAO(Geometry *geometry){

	const GLuint VERTEX_INDEX = 0;
	const GLuint TEXTURE_INDEX = 1;
	const GLuint NORMAL_INDEX = 2;

	//Generate Vertex Buffer Objects
	// create an array buffer object for storing our vertices
	glGenBuffers(1, &geometry->vertexBuffer);
	glGenBuffers(1, &geometry->textureBuffer);
	glGenBuffers(1, &geometry->normalBuffer);

	//Set up Vertex Array Object
	// create a vertex array object encapsulating all our vertex attributes
	glGenVertexArrays(1, &geometry->vertexArray);
	glBindVertexArray(geometry->vertexArray);

	// associate the position array with the vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glVertexAttribPointer(
		VERTEX_INDEX,		//Attribute index
		3, 					//# of components
		GL_FLOAT, 			//Type of component
		GL_FALSE, 			//Should be normalized?
		sizeof(vec3),		//Stride - can use 0 if tightly packed
		0);					//Offset to first element
	glEnableVertexAttribArray(VERTEX_INDEX);

    glBindBuffer(GL_ARRAY_BUFFER, geometry->textureBuffer);
    glVertexAttribPointer(
            TEXTURE_INDEX,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vec2),
            0);
    glEnableVertexAttribArray(TEXTURE_INDEX);

	glBindBuffer(GL_ARRAY_BUFFER, geometry->normalBuffer);
	glVertexAttribPointer(
		NORMAL_INDEX,		//Attribute index
		3, 					//# of components
		GL_FLOAT, 			//Type of component
		GL_FALSE, 			//Should be normalized?
		sizeof(vec3),		//Stride - can use 0 if tightly packed
		0);					//Offset to first element
	glEnableVertexAttribArray(NORMAL_INDEX);

	// unbind our buffers, resetting to default state
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return !CheckGLErrors();
}

// create buffers and fill with geometry data, returning true if successful
bool LoadGeometry(Geometry *geometry, vec3 *vertices, vec2 *texCoords, vec3 *normals, int elementCount)
{
	geometry->elementCount = elementCount;

	// create an array buffer object for storing our vertices
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*geometry->elementCount, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, geometry->textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*geometry->elementCount, texCoords, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, geometry->normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*geometry->elementCount, normals, GL_STATIC_DRAW);

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
}

// --------------------------------------------------------------------------
// Rendering function that draws our scene to the frame buffer

void RenderScene(Geometry *geometry, GLuint program, Camera* camera, mat4 modelMatrix, mat4 perspectiveMatrix, GLenum rendermode, int nodeType)
{
	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram(program);

	mat4 modelViewProjection = perspectiveMatrix * camera->viewMatrix() * modelMatrix;
	GLint uniformLocation = glGetUniformLocation(program, "modelViewProjection");
	glUniformMatrix4fv(uniformLocation, 1, false, glm::value_ptr(modelViewProjection));

	uniformLocation = glGetUniformLocation(program, "model");
	glUniformMatrix4fv(uniformLocation, 1, false, glm::value_ptr(modelMatrix));

	uniformLocation = glGetUniformLocation(program, "nodeType");
	glUniform1i(uniformLocation, nodeType);

	glBindVertexArray(geometry->vertexArray);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(rendermode, 0, geometry->elementCount);

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
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

// --------------------------------------------------------------------------
// Support Functions

vec3 generateParametricPoint(float u, float v, float r) {
    return vec3(cos(u) * sin(v) * r, cos(v) * r, sin(u) * sin(v) * r);
}

void generateSphere(vector<vec3>* points, vector<vec2>* uvs, vector<vec3>* normals, float radius) {

    points->clear();
    uvs->clear();
    normals->clear();

    float u, v, un, vn = 0.0f;

    int uNum = 30;
    int vNum = 15;

    float ustep = 2 * PI_F / uNum;
    float vstep = PI_F / vNum;

    for(int i = 0; i < uNum; i++) {
        for (int j = 0; j < vNum; j++) {

            u = i * ustep;
            v = j * vstep;

            if (i + 1 == uNum) {
                un = 2 * PI_F;
            } else {
                un = (i + 1) * ustep;
            }

            if (j + 1 == vNum) {
                vn = PI_F;
            } else {
                vn = (j + 1) * vstep;
            }

            vec3 p0 = generateParametricPoint(u, v, radius);
            vec3 p1 = generateParametricPoint(u, vn, radius);
            vec3 p2 = generateParametricPoint(un, v, radius);
            vec3 p3 = generateParametricPoint(un, vn, radius);

            points->push_back(p0);
            points->push_back(p2);
            points->push_back(p1);

            points->push_back(p3);
            points->push_back(p1);
            points->push_back(p2);

            uvs->push_back(vec2(u / (2 * PI_F), v / PI_F));
            uvs->push_back(vec2(un / (2 * PI_F), v / PI_F));
            uvs->push_back(vec2(u / (2 * PI_F), vn / PI_F));

            uvs->push_back(vec2(un / (2 * PI_F), vn / PI_F));
            uvs->push_back(vec2(u / (2 * PI_F), vn / PI_F));
            uvs->push_back(vec2(un / (2 * PI_F), v / PI_F));

            normals->push_back(normalize(p0));
            normals->push_back(normalize(p2));
            normals->push_back(normalize(p1));

            normals->push_back(normalize(p3));
            normals->push_back(normalize(p1));
            normals->push_back(normalize(p2));
        }
    }
}

class Node {
public:
    mat4 localMatrix;
    mat4 worldMatrix;
    vector<Node* > children;
    Node* parent;
    ~Node() {}
    Node() {
        parent = NULL;
        localMatrix = mat4(1.0f);
        worldMatrix = mat4(1.0f);
    }

    void addChild(Node* child) {
        children.push_back(child);
        child->parent = this;
    }

    void updateWorldMatrix(Node* p) {
        if (p == NULL) {
            worldMatrix = localMatrix;
        } else {
            worldMatrix = localMatrix * p->worldMatrix;
        }

        for (Node* i : children) {
            i->updateWorldMatrix(i->parent);
        }
    }
};


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
	window = glfwCreateWindow(width, height, "CPSC 453 Assignment 5", 0, 0);
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
	GLuint program = InitializeShaders();
	if (program == 0) {
		cout << "Program could not initialize shaders, TERMINATING" << endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

    vector<vec3> points;
    vector<vec2> uvs;
    vector<vec3> normals;

    //Fill in with Perspective Matrix
	mat4 perspectiveMatrix = glm::perspective(PI_F * 0.4f, float(width) / float(height), 0.01f, 10.f);

	Geometry geometry;
	Camera cam;
	vec2 lastCursorPos;

    generateSphere(&points, &uvs, &normals, 0.5f);

	// call function to create and fill buffers with geometry data
	if (!InitializeVAO(&geometry))
		cout << "Program failed to intialize geometry!" << endl;

	if(!LoadGeometry(&geometry, points.data(), uvs.data(), normals.data(), points.size()))
		cout << "Failed to load geometry" << endl;

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    Node sun;
    sun.localMatrix = translate(sun.localMatrix, vec3(0, 0, 0));

    Node earth;
    earth.localMatrix = translate(earth.localMatrix, vec3(2, 0, 0));
    earth.localMatrix = scale(earth.localMatrix, vec3(0.5, 0.5, 0.5));

    Node moon;
    moon.localMatrix = translate(moon.localMatrix, vec3(2, 0, 0));
    moon.localMatrix = scale(moon.localMatrix, vec3(0.25, 0.25, 0.25));

    Node stars;
    stars.localMatrix = translate(stars.localMatrix, vec3(0, 0, 2));
    stars.localMatrix = scale(stars.localMatrix, vec3(10, 10, 10));

    sun.addChild(&earth);
    sun.addChild(&stars);
    earth.addChild(&moon);

    cout << "Sun Local: " << to_string(sun.localMatrix) << endl;
    cout << "Sun World: " << to_string(sun.worldMatrix) << endl;

    cout << "Earth Local: " << to_string(earth.localMatrix) << endl;
    cout << "Earth World: " << to_string(earth.worldMatrix) << endl;

    cout << "Moon Local: " << to_string(moon.localMatrix) << endl;
    cout << "Moon World: " << to_string(moon.worldMatrix) << endl;

    sun.updateWorldMatrix(NULL);

    cout << "AFTER UPDATING" << endl;

    cout << "Sun Local: " << to_string(sun.localMatrix) << endl;
    cout << "Sun World: " << to_string(sun.worldMatrix) << endl;

    cout << "Earth Local: " << to_string(earth.localMatrix) << endl;
    cout << "Earth World: " << to_string(earth.worldMatrix) << endl;

    cout << "Moon Local: " << to_string(moon.localMatrix) << endl;
    cout << "Moon World: " << to_string(moon.worldMatrix) << endl;

    MyTexture sunTexture;
    string sunFile = "textures/sun.jpg";

    MyTexture earthTexture;
    string earthFile = "textures/earth.jpg";

    MyTexture moonTexture;
    string moonFile = "textures/moon.jpg";

    MyTexture starTexture;
    string starFile = "textures/starfield.jpg";

    InitializeTexture(&sunTexture, sunFile.c_str(), GL_TEXTURE_2D);
    InitializeTexture(&earthTexture, earthFile.c_str(), GL_TEXTURE_2D);
    InitializeTexture(&moonTexture, moonFile.c_str(), GL_TEXTURE_2D);
    InitializeTexture(&starTexture, starFile.c_str(), GL_TEXTURE_2D);

    glUseProgram(program);

    GLint sunLoc = glGetUniformLocation(program, "sunTexture");
    glUniform1i(sunLoc, 0);

    GLint earthLoc = glGetUniformLocation(program, "earthTexture");
    glUniform1i(earthLoc, 1);

    GLint moonLoc = glGetUniformLocation(program, "moonTexture");
    glUniform1i(moonLoc, 2);

    GLint starLoc = glGetUniformLocation(program, "starTexture");
    glUniform1i(starLoc, 3);

    GLint loc = glGetUniformLocation(program, "earthWorld");
    glUniformMatrix4fv(loc, 1, false, &earth.worldMatrix[0][0]);

    loc = glGetUniformLocation(program, "moonWorld");
    glUniformMatrix4fv(loc, 1, false, &moon.worldMatrix[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sunTexture.textureID);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, earthTexture.textureID);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, moonTexture.textureID);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, starTexture.textureID);

	float cursorSensitivity = PI_F/200.f;	//PI/hundred pixels
	float movementSpeed = 0.01f;

	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window))
	{
		////////////////////////
		//Camera interaction
		////////////////////////
		//Translation
		vec3 movement(0.f);

		if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			movement.z += 1.f;
		if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			movement.z -= 1.f;

		cam.move(movement * movementSpeed);


		//Rotation
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		vec2 cursorPos (xpos, ypos);
		vec2 cursorChange = cursorPos - lastCursorPos;

		if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
            cam.rotateSpherical(-cursorChange.x * cursorSensitivity, -cursorChange.y * cursorSensitivity);
		}

		lastCursorPos = cursorPos;

		///////////
		//Drawing
		//////////

		// clear screen to a dark grey colour
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderScene(&geometry, program, &cam, sun.worldMatrix, perspectiveMatrix, GL_TRIANGLES, 0);
		RenderScene(&geometry, program, &cam, earth.worldMatrix, perspectiveMatrix, GL_TRIANGLES, 1);
		RenderScene(&geometry, program, &cam, moon.worldMatrix, perspectiveMatrix, GL_TRIANGLES, 2);
		RenderScene(&geometry, program, &cam, stars.worldMatrix, perspectiveMatrix, GL_TRIANGLES, 3);

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
