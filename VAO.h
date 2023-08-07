#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include<GL/glew.h>
#include"VBO.h"
#include"EBO.h"

class VAO
{
public:
	GLuint ID;
	VAO();

	void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* effect);

	void BindEBO(EBO& EBO);
	void UnbindEBO(EBO& EBO);
	void Bind();
	void Unbind();
	void Delete();

};

#endif // !VAO_CLASS_H

