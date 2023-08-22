#include "ShaderHelper.h"

void ShaderHelper::PrepareProgram(const GLuint& shaderProgram) {
	//присваивание идентификатора шейдерной программы
	GLuint program = shaderProgram;
	//активация программы, привязка к текущему контексту
	glUseProgram(program);
}

GLint ShaderHelper::GetLocation(const GLuint& shaderProgram, const std::string& variable) {
	//получение позиции переменной в шейдерной программе, идентификация переменной в коде шейдера
	GLint location = glGetUniformLocation(shaderProgram, variable.c_str());
	return location;
}

void ShaderHelper::PassVariable(const GLint& variable, const GLint& location) {
	//установка значения переменной в активной шейдерной программе
	glUniform1f(location, variable);
}

void ShaderHelper::PassMatrix(const GLfloat* matrix, const GLint& location) {
	//установка значения переменной в активной шейдерной программе
	glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
}
