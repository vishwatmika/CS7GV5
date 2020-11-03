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
#include "theta.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "maths_funcs.h" //Anton's math class

// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;
GLuint shaderProgramID;


// VAO Variables
unsigned int object1Vao = 0;
unsigned int object2Vao = 0;
unsigned int object3Vao = 0;
unsigned int skyboxVAO, skyboxVBO;
unsigned int cubemapTexture;

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
vec2 theta;
float alpha = 0;

//mouse/Key movement
float x_mouse;
float y_mouse;
float z_mouse = 1.0f;
float tx, ty, tz = 0.0f;
float x_pos, y_pos, angle;
vec2 pos = vec2(0.0, 0.0);
GLuint skyboxShader;

// Model Load Variables
LoadObj Object1("body.obj");
LoadObj Object2("u_arm.obj");
LoadObj Object3("l_arm.obj");

mat4 projection_box = perspective(90.0, (float)(width) / (float)height, 0.01, 10.0);
//glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)width / (float)height, 0.1f, 100.0f);

#pragma region SKYBOX_FUNCTIONS
//skybox loading function - https://learnopengl.com/Advanced-OpenGL/Cubemaps
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f

};

vector<std::string> faces
{
	"right.jpg",
	"left.jpg",
	"top.jpg",
	"bottom.jpg",
	"front.jpg",
	"back.jpg"
};
#pragma endregion SKYBOX_FUNCTIONS

#pragma region VBO_FUNCTIONS
void generateObjectBuffer() {
	vertexPositionLocation = glGetAttribLocation(shaderProgramID, "vertexPosition");
	vertexNormalsLocation = glGetAttribLocation(shaderProgramID, "vertexNormals");
	// Uniform variables in shader
	lightPositionLocation = glGetUniformLocation(shaderProgramID, "lightPos");
	viewPositionLocation = glGetUniformLocation(shaderProgramID, "viewPos");
	ambientStrengthLocation = glGetUniformLocation(shaderProgramID, "ambientStr");
	specularStrengthLocation = glGetUniformLocation(shaderProgramID, "specularStr");

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//Body VBOs
	GLuint vp_vbo_body = 0;
	glGenBuffers(1, &vp_vbo_body);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo_body);
	glBufferData(GL_ARRAY_BUFFER, 3 * Object1.getNumVertices() * sizeof(float), Object1.getVertices(), GL_STATIC_DRAW);
	
	GLuint vn_vbo_body = 0;
	glGenBuffers(1, &vn_vbo_body);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo_body);
	glBufferData(GL_ARRAY_BUFFER, 3 * Object1.getNumVertices() * sizeof(float), Object1.getNormals(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &object1Vao);
	glBindVertexArray(object1Vao);

	glEnableVertexAttribArray(vertexPositionLocation);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo_body);
	glVertexAttribPointer(vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(vertexNormalsLocation);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo_body);
	glVertexAttribPointer(vertexNormalsLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	//Upper Arm
	GLuint vp_vbo_uarm = 0;
	glGenBuffers(1, &vp_vbo_uarm);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo_uarm);
	glBufferData(GL_ARRAY_BUFFER, 3 * Object2.getNumVertices() * sizeof(float), Object2.getVertices(), GL_STATIC_DRAW);
	
	GLuint vn_vbo_uarm = 0;
	glGenBuffers(1, &vn_vbo_uarm);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo_uarm);
	glBufferData(GL_ARRAY_BUFFER, 3 * Object2.getNumVertices() * sizeof(float), Object2.getNormals(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &object2Vao);
	glBindVertexArray(object2Vao);

	glEnableVertexAttribArray(vertexPositionLocation);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo_uarm);
	glVertexAttribPointer(vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(vertexNormalsLocation);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo_uarm);
	glVertexAttribPointer(vertexNormalsLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	//Lower Arm
	GLuint vp_vbo_larm = 0;
	glGenBuffers(1, &vp_vbo_larm);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo_larm);
	glBufferData(GL_ARRAY_BUFFER, 3 * Object3.getNumVertices() * sizeof(float), Object3.getVertices(), GL_STATIC_DRAW);

	GLuint vn_vbo_larm = 0;
	glGenBuffers(1, &vn_vbo_larm);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo_larm);
	glBufferData(GL_ARRAY_BUFFER, 3 * Object3.getNumVertices() * sizeof(float), Object3.getNormals(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &object3Vao);
	glBindVertexArray(object3Vao);

	glEnableVertexAttribArray(vertexPositionLocation);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo_larm);
	glVertexAttribPointer(vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(vertexNormalsLocation);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo_larm);
	glVertexAttribPointer(vertexNormalsLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}
#pragma endregion VBO_FUNCTIONS

void display() {

	
	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST);// enable depth-testing
	//glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDepthFunc(GL_LEQUAL);
	glUseProgram(skyboxShader);
	glUniform1i(glGetUniformLocation(skyboxShader, "skybox"), 0);
	glActiveTexture(GL_TEXTURE0);

	//glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	glm::mat4 view_box = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0.1, 0, 0.1), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "view"), 1, GL_FALSE, glm::value_ptr(view_box));
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "projection"), 1, GL_FALSE, (projection_box.m));

	// skybox cube
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glBindVertexArray(skyboxVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);

	glUseProgram(shaderProgramID);
	glUniform1i(glGetUniformLocation(shaderProgramID, "skybox"), 0);
	//Declare your uniform variables that will be used in your shader
	int model_location = glGetUniformLocation(shaderProgramID, "model");
	int view_mat_location = glGetUniformLocation(shaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID, "proj");
	int ortho_mat_location = glGetUniformLocation(shaderProgramID, "ortho");
	glUniform1i(glGetUniformLocation(shaderProgramID, "skybox"), 0);

	mat4 model = identity_mat4();
	mat4 ortho = identity_mat4();

	model = rotate_x_deg(model, 90.0);
	//model = rotate_x_deg(model, rotate_x_angle);
	//model = rotate_y_deg(model, rotate_y_angle);
	//model = rotate_z_deg(model, rotate_z_angle);
	//model = translate(model, vec3(tx, ty, tz));
	glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	mat4 projection = perspective(50.0, (float)(width) / (float)height, 0.5, 50.0);

	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, (projection_box.m));
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(model_location, 1, GL_FALSE, (model.m));
	glUniformMatrix4fv(ortho_mat_location, 1, GL_FALSE, ortho.m);

	glBindVertexArray(object1Vao);
	glDrawArrays(GL_TRIANGLES, 0, Object1.getNumVertices());
	glBindVertexArray(0);

	mat4 model2 = identity_mat4();
	model2 = rotate_x_deg(model2, 90.0);
	model2 = rotate_z_deg(model2, (-theta.v[0]*50.3+alpha));			//converted into degrees
	model2 = translate(model2, vec3(0.6, 1.6, 0));
	//model2 = translate(model2, vec3(1, 3, 0));
	//model2 = model * model2;
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, (projection_box.m));
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(model_location, 1, GL_FALSE, (model2.m));
	glUniformMatrix4fv(ortho_mat_location, 1, GL_FALSE, ortho.m);

	glBindVertexArray(object2Vao);
	glDrawArrays(GL_TRIANGLES, 0, Object2.getNumVertices());
	glBindVertexArray(0);

	mat4 model3 = identity_mat4();
	model3 = rotate_y_deg(model3, (-theta.v[1]*55.0));
	model3 = translate(model3, vec3(0.46, -0.34, 0.88));
	model3 = model2 * model3;
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, (projection_box.m));
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(model_location, 1, GL_FALSE, (model3.m));
	//glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(local_quat));
	glUniformMatrix4fv(ortho_mat_location, 1, GL_FALSE, ortho.m);

	glBindVertexArray(object3Vao);
	glDrawArrays(GL_TRIANGLES, 0, Object3.getNumVertices());
	glBindVertexArray(0);

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
	//model1.model = look_at(vec3(x_mouse, y_mouse, z_mouse), vec3(0.0f, 0.0f, 0.0f), vec3(0, 1, 0));
	//model2.model = look_at(vec3(x_mouse, y_mouse, z_mouse), vec3(0.0f, 0.0f, 0.0f), vec3(0, 1, 0));
	glutPostRedisplay();
}

void keyPress(unsigned char key, int xmouse, int ymouse) {
	//std::cout << "Keypress: " << key << std::endl;
	switch (key) {
		//changing rotation angles.
	case('x'):
		rotate_x_angle += 0.5f;
		break;
	case('y'):
		rotate_y_angle += 0.05f;
		break;
	case('z'):
		rotate_z_angle += 0.5f;
		break;
	case('s'):
		rotate_x_angle -= 0.5f;
		break;
	case('h'):
		rotate_y_angle -= 0.5f;
		break;
	case('a'):
		rotate_z_angle -= 0.5f;
		break;
	case('m'):
		angle -= 0.5f;
		break;
	case('v'):
		std::cout << "x" << rotate_x_angle<<'\n';
		std::cout << "y" << rotate_y_angle<<'\n';
		std::cout << "z" << rotate_z_angle<<'\n';
		std::cout << "tx" << tx<<'\n';
		std::cout << "ty" << ty<<'\n';
		std::cout << "tz" << tz<<'\n';
	}
};

void mouseMove(int x, int y) {
	// Fix x_mouse coordinates calculation to only take the first viewport
	x_mouse = (float)(x/width);
	y_mouse = (float)-(y/height);
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
		ty += 0.02;
		//angle += 0.5f;
		break;
	case(GLUT_KEY_DOWN):
		ty -= 0.02;
		//angle -= 0.5f;
		break;
	case(GLUT_KEY_LEFT):
		tx -= 0.02;
		break;
	case(GLUT_KEY_RIGHT):
		tx += 0.02;
		break;
	case(GLUT_KEY_END):
		tz -= 0.02;
		break;
	case(GLUT_KEY_HOME):
		tz += 0.02;
		break;
	}
};

void mouseClick(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		std::cout <<'\n'<<x << " " << y <<" ";
		x_pos = x;
		y_pos = y;
		Angle obj1;
		theta = obj1.calcTheta(vec2(x_pos, y_pos));
		alpha = 160;
		std::cout << "theta: " << theta.v[0] << " " << theta.v[1];
		if (isnan(theta.v[0]) && isnan(theta.v[1]))
		{
			std::cout << " nan";
			theta.v[0] = 0;
			theta.v[1] = 0;
			alpha = 0;
		}
	}
};

void init()
{
	// Set up the shaders
	shaderProgramID = Shader::CompileShaders();
	skyboxShader = Shader::skyboxShader();
	cubemapTexture = loadCubemap(faces);

	// load mesh into a vertex buffer array
	generateObjectBuffer();
}

int main(int argc, char** argv) {

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glEnable(GL_DEPTH_TEST);
	glutInitWindowSize(width, height);
	glutCreateWindow("Viewport");
	
	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keyPress);
	glutSpecialFunc(specialKeypress);
	glutPassiveMotionFunc(mouseMove);
	glutMouseWheelFunc(mouseWheel);
	glutMouseFunc(mouseClick);

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