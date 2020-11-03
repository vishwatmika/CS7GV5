//Some Windows Headers (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>

// OpenGL Libraries
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm\ext\matrix_clip_space.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

// C++ standard libraries
#include <fstream>
#include <iostream>
#include <sstream>
#include <string> 

// Custom Libraries
#include "loader.h"
#include "shader.h"
#include "maths_funcs.h" //Anton's math class

// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;
GLuint shaderProgramID;


// VAO Variables
unsigned int object1Vao = 0;

// ViewPort Dimensions
const int width = 800.0;
const int height = 600.0;

// Shader Variables
// Vertex Coordenates and Normals
GLuint vertexPositionLocation;
GLuint vertexNormalsLocation;

// Light Position
GLuint lightPositionLocation;
vec3 lightPositionDirection = vec3(0, 0, 1);
vec3 lightPosition = vec3(0, 50, 100);

// View Position
GLuint viewPositionLocation;

// Ambient and Specular Lighting Strength
GLuint ambientStrengthLocation;
float ambientStrength = 1.0f;
GLuint specularStrengthLocation;
float specularStrength = 0.5f;

//angles for rotations
GLfloat rotate_x_angle = 0.0f;
GLfloat rotate_y_angle = 0.0f;
GLfloat rotate_z_angle = 0.0f;
GLfloat qx, qy, qz = 0.0f;
glm::vec3 axis;

//mouse/Key movement
float x_mouse;
float y_mouse;
float z_mouse = 1.0f;
float x_pos, y_pos, z_pos = 0.0f;

class ProjectionMatrices {
public:
	mat4 projection;
	mat4 view;
	mat4 model;
	mat4 ortho;
};

// Model Load Variables
LoadObj Object1("plane2.obj");
ProjectionMatrices model1, model2;

#pragma region VBO_FUNCTIONS
void generateObjectBuffer() {
	GLuint vp_vbo = 0;

	vertexPositionLocation = glGetAttribLocation(shaderProgramID, "vertexPosition");
	vertexNormalsLocation = glGetAttribLocation(shaderProgramID, "vertexNormals");
	// Uniform variables in shader
	lightPositionLocation = glGetUniformLocation(shaderProgramID, "lightPos");
	viewPositionLocation = glGetUniformLocation(shaderProgramID, "viewPos");
	ambientStrengthLocation = glGetUniformLocation(shaderProgramID, "ambientStr");
	specularStrengthLocation = glGetUniformLocation(shaderProgramID, "specularStr");

	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * Object1.getNumVertices() * sizeof(float), Object1.getVertices(), GL_STATIC_DRAW);
	//glBufferData (GL_ARRAY_BUFFER, 3 * teapot_vertex_count * sizeof (float), teapot_vertex_points, GL_STATIC_DRAW);
	GLuint vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * Object1.getNumVertices() * sizeof(float), Object1.getNormals(), GL_STATIC_DRAW);
	//glBufferData (GL_ARRAY_BUFFER, 3 * teapot_vertex_count * sizeof (float), teapot_normals, GL_STATIC_DRAW);

	glGenVertexArrays(1, &object1Vao);
	glBindVertexArray(object1Vao);

	glEnableVertexAttribArray(vertexPositionLocation);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(vertexNormalsLocation);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(vertexNormalsLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}
#pragma endregion VBO_FUNCTIONS


void display() {
	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	//Declare your uniform variables that will be used in your shader
	int model_location = glGetUniformLocation(shaderProgramID, "model");
	int view_mat_location = glGetUniformLocation(shaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID, "proj");
	int ortho_mat_location = glGetUniformLocation(shaderProgramID, "ortho");


	// Main Viewport
	glViewport(0, 0, width, height);
	// Moving Model based on updated values
	model1.view = translate(identity_mat4(), vec3(-x_mouse + x_pos, y_mouse + y_pos, z_mouse + z_pos));
	model2.view = translate(identity_mat4(), vec3(x_mouse + x_pos, -y_mouse + y_pos, z_mouse + z_pos));
	
	glm::mat4 local;
	local = glm::eulerAngleYXZ(rotate_y_angle, rotate_x_angle, rotate_z_angle);
	
	//local = rotate_y_deg(local, rotate_y_angle);
	//local = rotate_x_deg(local, rotate_x_angle);
	//local = rotate_z_deg(local, rotate_z_angle);

	//quaternions
	versor local_quat = versor();
	local_quat = quat_from_axis_deg(qy, axis.x, axis.y, axis.z);
	mat4 quat = quat_to_mat4(local_quat);

	
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, model1.projection.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, model1.view.m);
	glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(local));
	//glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(local_quat));
	glUniformMatrix4fv(ortho_mat_location, 1, GL_FALSE, model1.ortho.m);
	glDrawArrays(GL_TRIANGLES, 0, Object1.getNumVertices());

	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, model2.projection.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, model2.view.m);
	glUniformMatrix4fv(model_location, 1, GL_FALSE, quat.m);
	glUniformMatrix4fv(ortho_mat_location, 1, GL_FALSE, model2.ortho.m);
	glDrawArrays(GL_TRIANGLES, 0, Object1.getNumVertices());


	glUniform3f(lightPositionLocation, lightPosition.v[0], lightPosition.v[1], lightPosition.v[2]);
	//glUniform3f(viewPositionLocation, 0, -10, 50);
	glUniform3f(viewPositionLocation, x_mouse, y_mouse, z_mouse);
	glUniform1f(ambientStrengthLocation, ambientStrength);
	glUniform1f(specularStrengthLocation, specularStrength);
	glutSwapBuffers();
}

void updateScene() {

	// Wait until at least 16ms passed since start of last frame (Effectively caps framerate at ~60fps)
	static DWORD  last_time = 0;
	DWORD  curr_time = timeGetTime();
	float  delta = (curr_time - last_time) * 0.001f;
	if (delta > 0.03f)
		delta = 0.03f;
	last_time = curr_time;
	// Camera moving
	model1.model = look_at(vec3(x_mouse, y_mouse, z_mouse), vec3(0.0f, 0.0f, 0.0f), vec3(0, 1, 0));
	model2.model = look_at(vec3(x_mouse, y_mouse, z_mouse), vec3(0.0f, 0.0f, 0.0f), vec3(0, 1, 0));
	glutPostRedisplay();
}

void keyPress(unsigned char key, int xmouse, int ymouse) {
	//std::cout << "Keypress: " << key << std::endl;
	switch (key) {
	//changing rotation angles.
	case('x'):
		rotate_x_angle += 0.05f;
		break;
	case('y'):
		rotate_y_angle += 0.05f;
		break;
	case('z'):
		rotate_z_angle += 0.05f;
		break;
	case('s'):
		rotate_x_angle -= 0.05f;
		break;
	case('h'):
		rotate_y_angle -= 0.05f;
		break;
	case('a'):
		rotate_z_angle -= 0.05f;
		break;
	case('r'):
		rotate_y_angle = 0.0f;
		rotate_y_angle = 0.0f;
		rotate_z_angle = 0.0f;
	case('j'):
		qy += 0.5f;
		axis = glm::vec3(1.0f, 0.0f, 0.0f);
		break;
	case('k'):
		qy += 0.5f;
		axis = glm::vec3(0.0f, 1.0f, 0.0f);
		break;
	case('l'):
		qy += 0.5f;
		axis = glm::vec3(0.0f, 0.0f, 1.0f);
		break;
	}
};

void mouseMove(int x, int y) {
	// Fix x_mouse coordinates calculation to only take the first viewport
	x_mouse = (float)-(x - width / 2) / (width / 2);
	y_mouse = (float)-(y - height / 2) / (height / 2);
};

void mouseWheel(int key, int wheeldir, int x, int y) {
	if (wheeldir == 1)
	{
		z_mouse -= 0.2f;
	}
	else if (wheeldir == -1) {
		z_mouse += 0.2f;
	}
};
void specialKeypress(int key, int x, int y) {
	switch (key) {
	case(GLUT_KEY_UP):
		y_pos += 0.02;
		break;
	case(GLUT_KEY_DOWN):
		y_pos -= 0.02;
		break;
	case(GLUT_KEY_LEFT):
		x_pos -= 0.02;
		break;
	case(GLUT_KEY_RIGHT):
		x_pos += 0.02;
		break;
	}
};

void init()
{
	// Set up the shaders
	shaderProgramID = Shader::CompileShaders();

	// load mesh into a vertex buffer array
	generateObjectBuffer();
}

int main(int argc, char** argv) {
	// Creating model initial values
	// Model 1 View
	model1.projection = perspective(90.0, (float)(width) / (float)height, 0.1, 100.0);
	model1.view = translate(identity_mat4(), vec3(0.0, 0.0, -2.0));
	model1.model = identity_mat4();
	model1.ortho = identity_mat4();

	model2.projection = perspective(90.0, (float)(width) / (float)height, 0.1, 100.0);
	model2.view = translate(identity_mat4(), vec3(0.0, 1.0, -2.0));
	model2.model = identity_mat4();
	model2.ortho = identity_mat4();

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Viewport");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keyPress);
	glutSpecialFunc(specialKeypress);
	glutPassiveMotionFunc(mouseMove);
	glutMouseWheelFunc(mouseWheel);

	// A call to glewInit() must be done after glut is initialized!
	glewExperimental = GL_TRUE; //for non-lab machines, this line gives better modern GL support
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	// Set up your objects and shaders
	init();

	// Begin infinite event loop
	glutMainLoop();

	return 0;
}