#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<GL/glew.h>
#include<glm/glm.hpp>

#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

std::string get_file_contents(const char* filename);

class Shader {

public:

	GLuint ID;
	Shader(const char* vertexFile, const char* fragmentFile);
	void Activate();
	void Delete();
	void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
	void compileErrors(unsigned int shader, const char* type);
};

#endif // !SHADER_CLASS_H
