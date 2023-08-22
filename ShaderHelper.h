#ifndef SHADER_HELPER_H
#define SHADER_HELEPR_H

#include<GL/glew.h>
#include<string>
#include <glm/fwd.hpp>

class ShaderHelper
{
public:
	static void PrepareProgram(const GLuint& shaderProgram);
	static GLint GetLocation(const GLuint& shaderProgram, const std::string& variable);
	static void PassVariable(const GLint& variable, const GLint& location);
	static void PassMatrix(const GLfloat* matrix, const GLint& location);
};


#endif // !SHADER_HELPER_H



