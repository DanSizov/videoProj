#include "VAO.h"

VAO::VAO() {
	glGenVertexArrays(1, &ID);
}

//устанавливаем связь с VBO и атрибутами вершин
void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) {
	VBO.Bind();
	//указываем OpenGL как интерпритировать данные вершин в VBO: первые 3 значения каждой вершины интерпритируются как координаты вершины, последние 2 значения как координаты текстуры
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}

void VAO::BindEBO(EBO& EBO) {
	EBO.Bind();
}

void VAO::UnbindEBO(EBO& EBO) {
	EBO.Unbind();
}

void VAO::Bind() {
	glBindVertexArray(ID);
}

void VAO::Unbind() {
	glBindVertexArray(0);
}

void VAO::Delete() {
	glDeleteVertexArrays(1, &ID);
}