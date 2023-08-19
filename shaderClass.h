#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<GL/glew.h>
#include<glm/glm.hpp>

#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

//Вспомогательная функция get_file_contents(const char* filename) используется для чтения содержимого файлов с исходным кодом шейдеров. Эта функция используется в конструкторе класса Shader для чтения исходного кода шейдеров из файлов.
std::string get_file_contents(const char* filename);

class Shader {

public:

	//идентификатор программы шейдеров. Он уникален для каждой программы шейдеров.
	GLuint ID;

	//конструктор класса. Он принимает пути к файлам с исходным кодом вершинного и фрагментного шейдеров, создает шейдеры, компилирует их, связывает в программу шейдеров и сохраняет ее идентификатор в ID.
	Shader(const char* vertexFile, const char* fragmentFile);

	//метод для активации программы шейдеров. При вызове этого метода, предположительно, вы вызовете glUseProgram(ID), чтобы активировать программу шейдеров.
	void Activate();

	//метод для удаления программы шейдеров. При вызове этого метода, предположительно, вы вызовете glDeleteProgram(ID), чтобы удалить программу шейдеров.
	void Delete();

	void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
	void compileErrors(unsigned int shader, const char* type);
};

#endif // !SHADER_CLASS_H
