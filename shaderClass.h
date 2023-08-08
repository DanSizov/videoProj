#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<GL/glew.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

//��������������� ������� get_file_contents(const char* filename) ������������ ��� ������ ����������� ������ � �������� ����� ��������. ��� ������� ������������ � ������������ ������ Shader ��� ������ ��������� ���� �������� �� ������.
std::string get_file_contents(const char* filename);

class Shader {

public:

	//������������� ��������� ��������. �� �������� ��� ������ ��������� ��������.
	GLuint ID;

	//����������� ������. �� ��������� ���� � ������ � �������� ����� ���������� � ������������ ��������, ������� �������, ����������� ��, ��������� � ��������� �������� � ��������� �� ������������� � ID.
	Shader(const char* vertexFile, const char* fragmentFile);

	//����� ��� ��������� ��������� ��������. ��� ������ ����� ������, ����������������, �� �������� glUseProgram(ID), ����� ������������ ��������� ��������.
	void Activate();

	//����� ��� �������� ��������� ��������. ��� ������ ����� ������, ����������������, �� �������� glDeleteProgram(ID), ����� ������� ��������� ��������.
	void Delete();

private:
	void compileErrors(unsigned int shader, const char* type);
};

#endif // !SHADER_CLASS_H
