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
// Assignment 4
// ==========================================================================

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <iterator>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/ext.hpp"
// #include <unistd.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
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

string scene1FileName = "scenes/scene1.txt";
string scene2FileName = "scenes/scene2.txt";
string scene3FileName = "scenes/scene3.txt";

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*geometry->elementCount, vertices, GL_STATIC_DRAW);

	// create another one for storing our colours
	glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*geometry->elementCount, colours, GL_STATIC_DRAW);

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
	glDrawArrays(GL_POINTS, 0, geometry->elementCount);

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
// Math Functions

float findMagnitude(vec3 v) {
    return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

vec3 crossProduct(vec3 a, vec3 b) {
    return vec3(a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x);
}

float dotProduct(vec3 a, vec3 b) {
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

float findDiscriminant(float a, float b, float c) {
    return pow(b, 2) -  a * c;
}

// --------------------------------------------------------------------------
// Support Classes

class Ray {
public:
    vec3 origin;
    vec3 direction;
    Ray(vec3 o, vec3 d): origin(o), direction(d){}
    void normalize() {
        float mag = findMagnitude(direction);
        direction.x /= mag;
        direction.y /= mag;
        direction.z /= mag;
    }
};

class Light {
public:
	vec3 position;
    Light(vec3 p): position(p) {}
};

class Shape {
public:
    virtual float intersect(Ray r) = 0;
	virtual vec3 getColour(){
		return this->getColour();
	}
    virtual ~Shape() {}
};

class Sphere: public Shape {
public:
    vec3 center;
    float radius;
    vec3 colour;
    Sphere(vec3 c, float r, vec3 co): center(c), radius(r), colour(co) {}
    float intersect(Ray r) {
        vec3 originToCenter = r.origin - center;

        float a = dotProduct(r.direction, r.direction);
        float b = dotProduct(r.direction, originToCenter);
        float c = dotProduct(originToCenter, originToCenter) - pow(radius, 2);

        float discriminant = findDiscriminant(a, b, c);
        if (discriminant < 0) {
            return INFINITY;
        }

        float t;
        float t0 = (-b + sqrt(discriminant)) /  a;
        float t1 = (-b - sqrt(discriminant)) / a;

        (t0 < t1) ? t = t0 : t = t1;
        if (t < 0) {
            return INFINITY;
        }

        return t;
    }

	vec3 getColour(){
		return colour;
	}
};

class Plane: public Shape {
public:
    vec3 normal;
    vec3 pointQ;
    vec3 colour;
    Plane(vec3 n, vec3 q, vec3 c): normal(n), pointQ(q), colour(c) {}
    float intersect(Ray r) {
        float t = dot(pointQ, normal) / dot(r.direction, normal);
        if (t > 0) {
            return t;
        }

        return INFINITY;
    }

	vec3 getColour(){
		return colour;
	}
};

class Triangle: public Shape {
public:
    vec3 pointA;
    vec3 pointB;
    vec3 pointC;
    vec3 colour;
    Triangle(vec3 a, vec3 b, vec3 c, vec3 co):
        pointA(a), pointB(b), pointC(c), colour(co) {}
    float intersect(Ray r) {
		vec3 ve = r.origin;
		vec3 vd = r.direction;
		vec3 pa = pointA;
		vec3 pb = pointB;
		vec3 pc = pointC;

		float a = pa.x - pb.x;
		float b = pa.y - pb.y;
		float c = pa.z - pb.z;
		float d = pa.x - pc.x;
		float e = pa.y - pc.y;
		float f = pa.z - pc.z;
		float g = vd.x;
		float h = vd.y;
		float i = vd.z;
		float j = pa.x - ve.x;
		float k = pa.y - ve.y;
		float l = pa.z - ve.z;

		float M = a * (e * i - h * f) + b * (g * f - d * i) + c * (d * h - e * g);

		float t = -(f * (a * k - j * b) + e * (j * c - a * l) + d * (b * l - k * c)) / M;
		float u = (i * (a * k - j * b) + h * (j * c - a * l) + g * (b * l - k * c)) / M;
		float v = (j * (e * i - h * f) + k * (g * f - d * i) + l * (d * h - e * g)) / M;

		if (t < 0 || u < 0 || u > 1 || v < 0 || (u + v) > 1) {
            return INFINITY;
        }

		return t;
    }

	vec3 getColour(){
		return colour;
	}
};

// --------------------------------------------------------------------------
// Support Functions

vec3 pixelColour(Ray r, vector<Shape*> shapes) {
    vec3 colour;
    float closestPoint = INFINITY;

    for(int k = 0; k < shapes.size(); k++){
        float t = shapes.at(k)->intersect(r);
        if(t < closestPoint){
            closestPoint = t;
            colour = shapes.at(k)->getColour();
        }
    }
    return colour;
}

void generateRays(vector<vec2>* points, vector<vec3>* colours, vector<Shape*> s, int width, int height, float f) {
    points->clear();
    colours-> clear();

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            float x = -width / 2 + i + 0.5;
            float y = height / 2 - j + 0.5;
            float z = -f;

            Ray r = Ray(vec3(0, 0, 0), vec3(x, y, z));
            r.normalize();

            vec3 colour = pixelColour(r, s);

            points->push_back(vec2(x/(width/2), y/(height/2)));
            colours->push_back(colour);
        }
    }
}

// --------------------------------------------------------------------------
// File Parsing Functions

void parseFile(string filename, vector<Shape*>* shapes) {
    ifstream f (filename);

    string line;
    vec3 colour;

    while(getline(f, line)) {
        if(line.find("light") != string::npos && line.find("#") == string::npos) {
           vec3 position;
           getline(f, line);
           sscanf(line.c_str(), "%f %f %f", &position.x, &position.y, &position.z);
        } else if (line.find("sphere") != string::npos && line.find("#") == string::npos) {
            vec3 center;
            float radius;

            // Get the next 3 lines for center of sphere, radius, and colour
            getline(f, line);
            sscanf(line.c_str(), "%f %f %f", &center.x, &center.y, &center.z);
            getline(f, line);
            sscanf(line.c_str(), "%f", &radius);
            getline(f, line);
            sscanf(line.c_str(), "%f %f %f", &colour.x, &colour.y, &colour.z);

            shapes->push_back(new Sphere(center, radius, colour));
        } else if (line.find("triangle") != string::npos && line.find("#") == string::npos) {
            vec3 pointA;
            vec3 pointB;
            vec3 pointC;

            getline(f, line);
            sscanf(line.c_str(), "%f %f %f", &pointA.x, &pointA.y, &pointA.z);
            getline(f, line);
            sscanf(line.c_str(), "%f %f %f", &pointB.x, &pointB.y, &pointB.z);
            getline(f, line);
            sscanf(line.c_str(), "%f %f %f", &pointC.x, &pointC.y, &pointC.z);
            getline(f, line);
            sscanf(line.c_str(), "%f %f %f", &colour.x, &colour.y, &colour.z);

            shapes->push_back(new Triangle(pointA, pointB, pointC, colour));
        } else if (line.find("plane") != string::npos && line.find("#") == string::npos) {
            vec3 normal;
            vec3 pointQ;

            getline(f, line);
            sscanf(line.c_str(), "%f %f %f", &normal.x, &normal.y, &normal.z);
            getline(f, line);
            sscanf(line.c_str(), "%f %f %f", &pointQ.x, &pointQ.y, &pointQ.z);
            getline(f, line);
            sscanf(line.c_str(), "%f %f %f", &colour.x, &colour.y, &colour.z);

            shapes->push_back(new Plane(normal, pointQ, colour));
        }
    }
    f.close();
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
    float depth = 470.0f;
	window = glfwCreateWindow(width, height, "CPSC 453 Assignment 4", 0, 0);
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

    vector<Shape*> scene1Shapes;
    parseFile(scene1FileName, &scene1Shapes);

    vector<vec2> points;
    vector<vec3> colours;

	// call function to create and fill buffers with geometry data
	Geometry geometry;
	if (!InitializeVAO(&geometry))
		cout << "Program failed to intialize geometry!" << endl;

	if(!LoadGeometry(&geometry, points.data(), colours.data(), points.size()))
		cout << "Failed to load geometry" << endl;

	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window))
	{
        generateRays(&points, &colours, scene1Shapes, width, height, depth);
        LoadGeometry(&geometry, points.data(), colours.data(), points.size());
		// call function to draw our scene
		RenderScene(&geometry, program);

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
