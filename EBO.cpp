#include "EBO.h"

EBO::EBO(GLuint* indices, GLsizeiptr size) {
	glGenBuffers(1, &ID); // создание одного буфера
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); // установка текущего буфера как активного
	//? в каких случаях нужно указывать GL_STATIC_DRAW, а в каких GL_DYNAMIC_DRAW 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW); // загружаем данные в буфер
}

void EBO::Bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void EBO::Unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::Delete() {
	glDeleteBuffers(1, &ID);
}