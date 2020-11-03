#pragma region HEADERS
//Some Windows Headers (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>

// OpenGL Libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm\ext\matrix_clip_space.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <AntTweakBar.h>

// C++ standard libraries
#include <fstream>
#include <iostream>
#include <sstream>
#include <string> 
#include <dos.h>
#include <Eigen/Dense>
#include <Eigen/Sparse>

// Custom Libraries
#include "loader.h"
#include "shader.h"
#include "obj_manip.h"

// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
using namespace std;

#pragma endregion HEADERS

#pragma region VARIABLES
GLFWwindow* window;
GLuint shaderProgramID;
GLuint sphereShaderID;

// VAO Variables
unsigned int object1Vao = 0;
unsigned int sphereVao = 0;

// ViewPort Dimensions
const int width = 800.0;
const int height = 600.0;

// Shader Variables
// Vertex Coordenates and Normals
GLuint vertexPositionLocation, vertexPositionLocation2;
GLuint vertexNormalsLocation, vertexNormalsLocation2;

// Light Position
GLuint lightPositionLocation, lightPositionLocation2;
glm::vec3 lightPositionDirection = glm::vec3(0, 0, 1);
glm::vec3 lightPosition = glm::vec3(0, 50, 100);

// View Position
GLuint viewPositionLocation, viewPositionLocation2;

// Ambient and Specular Lighting Strength
GLuint ambientStrengthLocation, ambientStrengthLocation2;
float ambientStrength = 1.0f;
GLuint specularStrengthLocation, specularStrengthLocation2;
float specularStrength = 0.5f;

//MVP
glm::mat4 ortho;
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat4 modelview;

//angles for rotations
GLfloat rotate_x_angle = 0.0f;
GLfloat rotate_y_angle = 0.0f;
GLfloat rotate_z_angle = 0.0f;
GLfloat qx, qy, qz = 0.0f;
glm::vec3 axis;

//mouse/Key movement
double x, y;
float x_mouse;
float y_mouse;
float z_mouse = 1.0f;
float x_pos, y_pos, z_pos = 0.0f;

//vertex picking
glm::vec3 modifier_vertex;
int vertex_pos = 0;
glm::vec2 pos = glm::vec2(400, 300);
glm::vec3 new_vertex;
glm::vec3 manip;
bool lbutton_down;
bool selection;

//direct manipulation
int constraints = 0;
vector<int> cons_pos;
vector<glm::vec3> constraint_vertices;

//blendshape weights
GLfloat w1, w2, w3, w4, w5, w6;
std::vector<std::vector<float>> weights;
int weight_index;

//file manipulation for changed vertices
ObjManip newObj("obj/high/neutral.obj", "obj/high/neutral2.obj");

// Model Load Variables
LoadObj Object1, Object2, Object3, Object4, Object5, Object6, Object7, Object8, Object9, Object10, Object11;
LoadObj sphere;
#pragma endregion VARIABLES

//Generate Object Buffers
void generateObjectBuffer() {
	//std::cout << "Buffer Created!!!" << endl;
	// Uniform variables in shader
	lightPositionLocation = glGetUniformLocation(shaderProgramID, "lightPos");
	viewPositionLocation = glGetUniformLocation(shaderProgramID, "viewPos");
	ambientStrengthLocation = glGetUniformLocation(shaderProgramID, "ambientStr");
	specularStrengthLocation = glGetUniformLocation(shaderProgramID, "specularStr");

	//vertices and normals for different blendshapes
	vertexPositionLocation = glGetAttribLocation(shaderProgramID, "vertexPosition");
	vertexNormalsLocation = glGetAttribLocation(shaderProgramID, "vertexNormals");
	
	GLuint vp_vbo = 0;
	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * Object1.getNumVertices() * sizeof(float), Object1.getVertices(), GL_STATIC_DRAW);

	GLuint vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * Object1.getNumVertices() * sizeof(float), Object1.getNormals(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &object1Vao);
	glBindVertexArray(object1Vao);

	glEnableVertexAttribArray(vertexPositionLocation);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(vertexNormalsLocation);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(vertexNormalsLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// Uniform variables in shader
	lightPositionLocation2 = glGetUniformLocation(sphereShaderID, "lightPos");
	viewPositionLocation2 = glGetUniformLocation(sphereShaderID, "viewPos");
	ambientStrengthLocation2 = glGetUniformLocation(sphereShaderID, "ambientStr");
	specularStrengthLocation2 = glGetUniformLocation(sphereShaderID, "specularStr");

	//vertices and normals for different blendshapes
	vertexPositionLocation2 = glGetAttribLocation(sphereShaderID, "vertexPosition");
	vertexNormalsLocation2 = glGetAttribLocation(sphereShaderID, "vertexNormals");
	GLuint svp_vbo = 0;
	glGenBuffers(1, &svp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, svp_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * sphere.getNumVertices() * sizeof(float), sphere.getVertices(), GL_STATIC_DRAW);

	GLuint svn_vbo = 0;
	glGenBuffers(1, &svn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, svn_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * sphere.getNumVertices() * sizeof(float), sphere.getNormals(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &sphereVao);
	glBindVertexArray(sphereVao);

	glEnableVertexAttribArray(vertexPositionLocation2);
	glBindBuffer(GL_ARRAY_BUFFER, svp_vbo);
	glVertexAttribPointer(vertexPositionLocation2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(vertexNormalsLocation2);
	glBindBuffer(GL_ARRAY_BUFFER, svn_vbo);
	glVertexAttribPointer(vertexNormalsLocation2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

}

void display(void) {
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(shaderProgramID);

	//Declare your uniform variables that will be used in your shader
	int model_location = glGetUniformLocation(shaderProgramID, "model");
	int view_mat_location = glGetUniformLocation(shaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID, "proj");
	int ortho_mat_location = glGetUniformLocation(shaderProgramID, "ortho");

	glUniform3f(lightPositionLocation, lightPosition.x, lightPosition.y, lightPosition.z);
	glUniform3f(viewPositionLocation, 0, 0, 50);
	glUniform1f(ambientStrengthLocation, ambientStrength);
	glUniform1f(specularStrengthLocation, specularStrength);

	// Matrices 
	ortho = glm::mat4(1.0);
	view = glm::lookAt(glm::vec3(0, 15, 80), glm::vec3(0.0, 15, 0), glm::vec3(0, 1, 0));
	projection = glm::perspective(-56.0f, (float)(width) / (float)height, 0.1f, 100.0f);
	model = glm::mat4(1.0);// = identity_mat4();
	model = glm::translate(model, glm::vec3(0, 0, 0));
	model = glm::rotate(model, rotate_x_angle, glm::vec3(1, 0, 0));
	model = glm::rotate(model, rotate_y_angle, glm::vec3(0, 1, 0));
	model = glm::rotate(model, rotate_z_angle, glm::vec3(0, 0, 1));

	modelview = model * view;

	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(ortho_mat_location, 1, GL_FALSE, glm::value_ptr(ortho));

	glBindVertexArray(object1Vao);
	glDrawArrays(GL_TRIANGLES, 0, Object1.getNumVertices());
	glBindVertexArray(0);
	glDeleteShader(shaderProgramID);

	//sphere
	glUseProgram(sphereShaderID);

	//Declare your uniform variables that will be used in your shader
	int model_location2 = glGetUniformLocation(sphereShaderID, "model");
	int view_mat_location2 = glGetUniformLocation(sphereShaderID, "view");
	int proj_mat_location2 = glGetUniformLocation(sphereShaderID, "proj");
	int ortho_mat_location2 = glGetUniformLocation(sphereShaderID, "ortho");

	glUniform3f(lightPositionLocation, lightPosition.x, lightPosition.y, lightPosition.z);
	glUniform3f(viewPositionLocation, 0, 0, 50);
	glUniform1f(ambientStrengthLocation, ambientStrength);
	glUniform1f(specularStrengthLocation, specularStrength);
	
	glm::mat4 ortho2 = glm::mat4(1.0);
	glm::mat4 model2 = glm::mat4(1.0);// = identity_mat4();
	model2 = glm::translate(model2, glm::vec3(2*manip.x,2*manip.y, 0.0));
	//model2 = glm::rotate(model2, z_mouse, glm::vec3(0, 0, 1));
	glm::mat4 view2 = glm::lookAt(glm::vec3(x_pos, y_pos, 55), glm::vec3(x_pos, y_pos, 0.0), glm::vec3(0, 1, 0));
	glm::mat4 projection2 = glm::perspective(90.0f, (float)(width) / (float)height, 0.1f, 100.0f);

	glUniformMatrix4fv(proj_mat_location2, 1, GL_FALSE, glm::value_ptr(projection2));
	glUniformMatrix4fv(view_mat_location2, 1, GL_FALSE, glm::value_ptr(view2));
	glUniformMatrix4fv(model_location2, 1, GL_FALSE, glm::value_ptr(model2));
	glUniformMatrix4fv(ortho_mat_location2, 1, GL_FALSE, glm::value_ptr(ortho2));

	/*if (lbutton_down) 
	{
		glBindVertexArray(sphereVao);
		glDrawArrays(GL_TRIANGLES, 0, sphere.getNumVertices());
		glBindVertexArray(0);
		glDeleteShader(sphereShaderID);
	}*/
}

void animation_playback()
{
	std::ifstream ifs("blendshape_animation.txt", std::ifstream::in);
	std::string line;
	float key;
	while (ifs.good() && !ifs.eof() && std::getline(ifs, line))
	{
		std::vector<float> w;
		std::stringstream stringstream(line);
		for (int i = 0; i < 6; i++)
		{
			stringstream >> key >> std::ws;
			w.push_back(key);
		}
		weights.push_back(w);
	}
}

void init()
{
	// Set up the shaders
	shaderProgramID = Shader::CompileShaders();
	sphereShaderID = Shader::CompileSphereShaders();

	animation_playback();
	// load mesh into a vertex buffer array
	generateObjectBuffer();
}

//function for selecting the manipulator vertex
void select_vertex()
{
	// Reference: https://www.gamedev.net/forums/topic/613123-glreadpixels-reading-z-value/
	// Converting 2D to 3D coordinates
	GLfloat xpos, ypos, zpos;

	double x, y;
	glfwGetCursorPos(window, &x, &y);
	if ((x > 252) && (x < 547) && (y > 57) && (y < 479))
	{
		std::cout << "2D Position: " << x << " , " << y << std::endl;
		xpos = (float)x;
		ypos = height - y - 1;
		glReadPixels(xpos, ypos, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zpos);
		//std::cout << "3D Position: " << xpos << " , " << ypos << " , " << zpos << std::endl;

		modifier_vertex = glm::unProject(glm::vec3(xpos, ypos, zpos), modelview, projection, glm::vec4(0.0f, 0.0f, width, height));
		std::cout << "World Coordinates: " << modifier_vertex.x << " , " << modifier_vertex.y << " , " << modifier_vertex.z << std::endl;
		manip = modifier_vertex;

		// finding the near value of vertex selected
		std::vector<vertex> allVertexvalues = Object1.getAllVertices();
		int i;
		float difference = 100;
		for (i = 0; i < allVertexvalues.size(); i++) {
			float temp = sqrt(((modifier_vertex.x - allVertexvalues[i].v[0]) * (modifier_vertex.x - allVertexvalues[i].v[0])) +
				((modifier_vertex.y - allVertexvalues[i].v[1]) * (modifier_vertex.y - allVertexvalues[i].v[1])) +
				((modifier_vertex.z - allVertexvalues[i].v[2]) * (modifier_vertex.z - allVertexvalues[i].v[2])));
			if (temp < difference) {
				difference = temp;
				vertex_pos = i;
			}
		}
		std::cout << "Position: " << vertex_pos + 1 << std::endl;
		modifier_vertex = glm::vec3(allVertexvalues[vertex_pos].v[0], allVertexvalues[vertex_pos].v[1], allVertexvalues[vertex_pos].v[2]);
		if (constraints < 3)
		{
			cons_pos.push_back(vertex_pos);
			constraint_vertices.push_back(modifier_vertex);
				constraints++;
		}
		std::cout << "Selected Vertex: " << modifier_vertex.x << " , " << modifier_vertex.y << " , " << modifier_vertex.z << std::endl;
		std::cout << "------------------------------------------------------" << std::endl;
		selection = true;
	}
}

//for changing the vertex value according to the manipulator
void change_vertex(double x, double y)
{
	GLfloat xpos, ypos, zpos;

	std::cout << "2D Position: " << x << " , " << y << std::endl;
	xpos = (float)x;
	ypos = height - y - 1;
	glReadPixels(xpos, ypos, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zpos);
	std::cout << "3D Position: " << xpos << " , " << ypos << " , " << zpos << std::endl;

	new_vertex = glm::unProject(glm::vec3(xpos, ypos, zpos), modelview, projection, glm::vec4(0.0f, 0.0f, width, height));
	std::cout << "World Coordinates: " << new_vertex.x << " , " << new_vertex.y << " , " << new_vertex.z << std::endl;
	manip = new_vertex;
	constraint_vertices[2] = new_vertex;

	std::cout << "Changing vertex" << endl;
	std::cout << "------------------------------------------------------" << std::endl;

	Object1.modify(vertex_pos, new_vertex);
	generateObjectBuffer();
	display();
	TwDraw();
	glfwSwapBuffers(window);

	//newObj.modifyObj(vertex_pos, new_vertex);
}

void my_direct_manip_method()
{
	unsigned int  num_rows = 9 ;
	unsigned int  num_cols = 11 ;

	float alpha = 0.01;
	float mu = 0.001;

	Eigen::MatrixXf A;
	Eigen::VectorXf b;

	//setup left-hand side
	Eigen::MatrixXf B(num_rows, num_cols); //the delta-blendshape matrix + additional weight normalization

	// fill the matrix B by acessing element-wise B(row,column)=??? 
	for (int i = 0, j=0; i < num_rows; i+3, j++)
	{
		B(i + 0, 0) = Object1.getAllVertices()[cons_pos[j]].v[0];
		B(i + 1, 0) = Object1.getAllVertices()[cons_pos[j]].v[1];
		B(i + 2, 0) = Object1.getAllVertices()[cons_pos[j]].v[2];

		B(i + 0, 1) = Object2.getAllVertices()[cons_pos[j]].v[0];
		B(i + 1, 1) = Object2.getAllVertices()[cons_pos[j]].v[1];
		B(i + 2, 1) = Object2.getAllVertices()[cons_pos[j]].v[2];

		B(i + 0, 2) = Object3.getAllVertices()[cons_pos[j]].v[0];
		B(i + 1, 2) = Object3.getAllVertices()[cons_pos[j]].v[1];
		B(i + 2, 2) = Object3.getAllVertices()[cons_pos[j]].v[2];

		B(i + 0, 3) = Object4.getAllVertices()[cons_pos[j]].v[0];
		B(i + 1, 3) = Object4.getAllVertices()[cons_pos[j]].v[1];
		B(i + 2, 3) = Object4.getAllVertices()[cons_pos[j]].v[2];

		B(i + 0, 4) = Object5.getAllVertices()[cons_pos[j]].v[0];
		B(i + 1, 4) = Object5.getAllVertices()[cons_pos[j]].v[1];
		B(i + 2, 4) = Object5.getAllVertices()[cons_pos[j]].v[2];

		B(i + 0, 5) = Object6.getAllVertices()[cons_pos[j]].v[0];
		B(i + 1, 5) = Object6.getAllVertices()[cons_pos[j]].v[1];
		B(i + 2, 5) = Object6.getAllVertices()[cons_pos[j]].v[2];

		B(i + 0, 6) = Object7.getAllVertices()[cons_pos[j]].v[0];
		B(i + 1, 6) = Object7.getAllVertices()[cons_pos[j]].v[1];
		B(i + 2, 6) = Object7.getAllVertices()[cons_pos[j]].v[2];

		B(i + 0, 7) = Object8.getAllVertices()[cons_pos[j]].v[0];
		B(i + 1, 7) = Object8.getAllVertices()[cons_pos[j]].v[1];
		B(i + 2, 7) = Object8.getAllVertices()[cons_pos[j]].v[2];

		B(i + 0, 8) = Object9.getAllVertices()[cons_pos[j]].v[0];
		B(i + 1, 8) = Object9.getAllVertices()[cons_pos[j]].v[1];
		B(i + 2, 8) = Object9.getAllVertices()[cons_pos[j]].v[2];

		B(i + 0, 9) = Object10.getAllVertices()[cons_pos[j]].v[0];
		B(i + 1, 9) = Object10.getAllVertices()[cons_pos[j]].v[1];
		B(i + 2, 9) = Object10.getAllVertices()[cons_pos[j]].v[2];

		B(i + 0, 10) = Object11.getAllVertices()[cons_pos[j]].v[0];
		B(i + 1, 10) = Object11.getAllVertices()[cons_pos[j]].v[1];
		B(i + 2, 10) = Object11.getAllVertices()[cons_pos[j]].v[2];
	}

	Eigen::VectorXf wt;
	wt(0) = 1;
	wt(1) = w1;
	wt(2) = w1;
	wt(3) = w2;
	wt(4) = w2;
	wt(5) = w3;
	wt(6) = w4;
	wt(7) = w4;
	wt(8) = w4;
	wt(9) = w5;
	wt(10) = w6;

	// setup right-hand side
	
	// fill the vector b by acessing element-wise b(row)=??? 
	b(0) = Object1.getAllVertices()[cons_pos[2]].v[0] - constraint_vertices[2].x;
	b(1) = Object1.getAllVertices()[cons_pos[2]].v[1] - constraint_vertices[2].y;
	b(2) = Object1.getAllVertices()[cons_pos[2]].v[2] - constraint_vertices[2].z;

	b(3) = constraint_vertices[0].x - constraint_vertices[2].x;
	b(4) = constraint_vertices[0].y - constraint_vertices[2].y;
	b(5) = constraint_vertices[0].z - constraint_vertices[2].z;

	b(6) = constraint_vertices[1].x - constraint_vertices[2].x;
	b(7) = constraint_vertices[1].y - constraint_vertices[2].y;
	b(8) = constraint_vertices[1].z - constraint_vertices[2].z;

	//std::cout << b;

	//solve the least-squares problem A * w = b with A=BtB and b= Btb
	//the function B.transpose() returns the transpose of a matrix
	A = B.transpose() * B + (alpha + mu) * Eigen::MatrixXf::Identity(num_cols, num_cols);
	b = B.transpose() * b + alpha * wt;
	Eigen::LDLT<Eigen::MatrixXf> solver(A);
	Eigen::VectorXf w = solver.solve(b);

	//copy back the weights my_weights[row] = w(row)
	w1 = wt(1);
	w2 = wt(3);
	w3 = wt(5);
	w4 = wt(7);
	w5 = wt(9);
	w6 = wt(10);
}	

//Call back functions
inline void TwEventMouseButtonGLFW3(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		lbutton_down = true;
		select_vertex();
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		select_vertex();
		std::cout << constraint_vertices[constraints - 1].x << " " <<
					constraint_vertices[constraints - 1].y << " " <<
					constraint_vertices[constraints - 1].z << " " << endl;
	}
	TwEventMouseButtonGLFW(button, action);
}
inline void TwEventMousePosGLFW3(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && selection)
	{
		lbutton_down = true;
		change_vertex(xpos, ypos);
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		change_vertex(xpos, ypos);
		my_direct_manip_method();
		Object1.blendMesh(Object2.returnfloatVertices(), Object3.returnfloatVertices(), w1,
			Object4.returnfloatVertices(), Object5.returnfloatVertices(), w2,
			Object6.returnfloatVertices(), Object7.returnfloatVertices(), w3,
			Object8.returnfloatVertices(), Object9.returnfloatVertices(), w4,
			Object10.returnfloatVertices(), w5,
			Object11.returnfloatVertices(), w6);
		generateObjectBuffer();
		display();
		TwDraw();
		glfwSwapBuffers(window);
	}
	TwMouseMotion(int(xpos), int(ypos));
}
inline void TwEventKeyGLFW3(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
			//changing rotation angles.
		case(GLFW_KEY_X):
			rotate_x_angle += 0.05f;
			break;
		case(GLFW_KEY_Y):
			rotate_y_angle += 0.05f;
			break;				
		case(GLFW_KEY_Z):		
			rotate_z_angle += 0.05f;
			break;				
		case(GLFW_KEY_S):		
			rotate_x_angle -= 0.05f;
			break;				
		case(GLFW_KEY_H):		
			rotate_y_angle -= 0.05f;
			break;				
		case(GLFW_KEY_A):		
			rotate_z_angle -= 0.05f;
			break;
		case(GLFW_KEY_R):
			rotate_y_angle = 0.0f;
			rotate_y_angle = 0.0f;
			rotate_z_angle = 0.0f;
		case(GLFW_KEY_J):
			z_pos += 0.2f;
			break;
		case(GLFW_KEY_K):
			z_pos -= 0.2f;
			break;
		case(GLFW_KEY_UP):
			y_pos += 0.2;
			break;
		case(GLFW_KEY_DOWN):
			y_pos -= 0.2;
			break;
		case(GLFW_KEY_LEFT):
			x_pos -= 0.2;
			break;
		case(GLFW_KEY_RIGHT):
			x_pos += 0.2;
			break;
		case(GLFW_KEY_V):
			cout << w1 << " " << w2 << " " << " " << w3 << " " << w4 << " " << w5 << endl;
			break;
		case(GLFW_KEY_P):
			for (weight_index = 0; weight_index < weights.size(); weight_index++)
			{
				std::cout << weights[weight_index][0] << " " << weights[weight_index][1] << " " << weights[weight_index][3] << " " << weights[weight_index][4] << " " << weights[weight_index][5] << endl;
				Object1.blendMesh(Object2.returnfloatVertices(), Object3.returnfloatVertices(), weights[weight_index][0],
					Object4.returnfloatVertices(), Object5.returnfloatVertices(), weights[weight_index][1],
					Object6.returnfloatVertices(), Object7.returnfloatVertices(), weights[weight_index][2],
					Object8.returnfloatVertices(), Object9.returnfloatVertices(), weights[weight_index][3],
					Object10.returnfloatVertices(), weights[weight_index][4],
					Object11.returnfloatVertices(), weights[weight_index][5]);
				generateObjectBuffer();
				display();
				TwDraw();
				glfwSwapBuffers(window);
				Sleep(100);
			}
			break;
		}
	}
	TwEventKeyGLFW(key, action);
}
inline void TwWindowSizeGLFW3(GLFWwindow* window, int width, int height)
{
	TwWindowSize(width, height);
}

void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

//setting blendshape weights
void TW_CALL setValueCB(const void* value, void* clientData)
{
	*(float*)clientData = *(const float*)value;
	Object1.blendMesh(	Object2.returnfloatVertices(), Object3.returnfloatVertices(), w1,
						Object4.returnfloatVertices(), Object5.returnfloatVertices(), w2,
						Object6.returnfloatVertices(), Object7.returnfloatVertices(), w3,
						Object8.returnfloatVertices(), Object9.returnfloatVertices(), w4,
						Object10.returnfloatVertices(), w5,
						Object11.returnfloatVertices(), w6);
	generateObjectBuffer();
}

//getting value for the bar
void TW_CALL getValueCB(void* value, void* clientData)
{
	*(float*)value = *(float*)clientData;
}

int main(int argc, char** argv) {

	// Setting up objects
	Object1.LoadObject("obj/high/neutral2.obj");
	Object2.LoadObject("obj/high/Mery_l_smile.obj");
	Object3.LoadObject("obj/high/Mery_r_smile.obj");
	Object4.LoadObject("obj/high/Mery_l_sad.obj");
	Object5.LoadObject("obj/high/Mery_r_sad.obj");
	Object6.LoadObject("obj/high/Mery_l_brow_raise.obj");
	Object7.LoadObject("obj/high/Mery_r_brow_raise.obj");
	Object8.LoadObject("obj/high/Mery_l_eye_closed.obj");
	Object9.LoadObject("obj/high/Mery_r_eye_closed.obj");
	Object10.LoadObject("obj/high/Mery_kiss.obj");
	Object11.LoadObject("obj/high/Mery_jaw_open.obj");
	sphere.LoadObject("sphere.obj");
	

	//set GLFW and viewports
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) exit(EXIT_FAILURE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	
	// Set up the window
	window = glfwCreateWindow(width, height, "ViewPort", NULL, NULL);
	
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Initialize AntTweakBar
	TwInit(TW_OPENGL_CORE, NULL);

	// Pointer to a tweak bar
	TwBar* bar;
	bar = TwNewBar("TweakBar");
	TwWindowSize(800, 600);
	int wire = 0;
	float bgColor[] = { 0.1f, 0.2f, 0.4f };
	TwDefine(" GLOBAL help='Integrating AntTweakBar with GLFW and OpenGL.' "); // Message added to the help bar.
	TwAddVarRO(bar, "ObjRotation", TW_TYPE_QUAT4F, &model, "label='Object rotation' opened=true help='Change the object orientation.' ");
	TwAddVarRO(bar, "WorldCoords1", TW_TYPE_FLOAT, &modifier_vertex[0], " label='3D-x' ");
	TwAddVarRO(bar, "WorldCoords2", TW_TYPE_FLOAT, &modifier_vertex[1], " label='3D-y' ");
	TwAddVarRO(bar, "WorldCoords3", TW_TYPE_FLOAT, &modifier_vertex[2], " label='3D-z' ");
	TwAddVarCB(bar, "weight1", TW_TYPE_FLOAT, setValueCB, getValueCB, &w1, "min=0 max=1 step=0.1 label='Happy' ");
	TwAddVarCB(bar, "weight2", TW_TYPE_FLOAT, setValueCB, getValueCB, &w2, "min=0 max=1 step=0.1 label='Sad' ");
	TwAddVarCB(bar, "weight3", TW_TYPE_FLOAT, setValueCB, getValueCB, &w3, "min=0 max=1 step=0.1 label='Brow Raise' ");
	TwAddVarCB(bar, "weight4", TW_TYPE_FLOAT, setValueCB, getValueCB, &w4, "min=0 max=1 step=0.1 label='Eye Close'");
	TwAddVarCB(bar, "weight5", TW_TYPE_FLOAT, setValueCB, getValueCB, &w5, "min=0 max=1 step=0.1 label='Kiss' ");
	TwAddVarCB(bar, "weight6", TW_TYPE_FLOAT, setValueCB, getValueCB, &w6, "min=0 max=1 step=0.1 label='Jaw' ");
	
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// Set GLFW event callbacks
	glfwSetWindowSizeCallback(window, (GLFWwindowposfun)TwWindowSizeGLFW3);
	glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW3);
	glfwSetCursorPosCallback(window, (GLFWcursorposfun)TwEventMousePosGLFW3);
	glfwSetKeyCallback(window, (GLFWkeyfun)TwEventKeyGLFW3);

	// A call to glewInit() must be done after glut is initialized!
	glewExperimental = GL_TRUE; //for non-lab machines, this line gives better modern GL support
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	//set up buffers and shaders
	init();
	
	// Begin infinite event loop
	while (!glfwWindowShouldClose(window))
	{
		glfwWaitEvents();
		display();
		TwDraw();
		glfwSwapBuffers(window);
	}

	//close everything
	TwTerminate();
	glfwTerminate();
	return 0;
}