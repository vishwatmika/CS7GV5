#pragma once
#include <GL\glew.h>
#include <string>

class Shader
{
public:
	static std::string readShaderSource(const std::string& fileName);
	static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	static GLuint CompileShaders();
	static GLuint CompileSphereShaders();
};