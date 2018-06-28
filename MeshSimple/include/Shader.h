#pragma once
#include <base.h>

class Shader
{
public:
	void Use();
	void setMat4(const char* name, glm::mat4 mat);
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	~Shader();
private:
	GLuint Program;
};