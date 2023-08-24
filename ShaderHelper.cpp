#include "ShaderHelper.h"

void ShaderHelper::PrepareProgram(const GLuint& shaderProgram) {
	GLuint program = shaderProgram;
	glUseProgram(program);
}

GLint ShaderHelper::GetLocation(const GLuint& shaderProgram, const std::string& variable) {
	GLint location = glGetUniformLocation(shaderProgram, variable.c_str());
	return location;
}

void ShaderHelper::PassVariable(const GLint& variable, const GLint& location) {
	glUniform1f(location, variable);
}

void ShaderHelper::PassMatrix(const GLfloat* matrix, const GLint& location) {
	glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
}
