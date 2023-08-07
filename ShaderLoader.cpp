#include "ShaderLoader.h"

void ShaderLoader::checkCompileErrors(GLuint shader, std::string type) {

	//�������� ���������� �������� ������
	GLint success;
	//�������� ��������� �� ������, ���� ����
	GLcharARB infoLog[1024];

	//���� ��� ������� �� PROGRAM, ����� ������ ����� glGetShaderiv ��� �������� ������� ���������� �������
	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {

			//����� ������ ��� ��������� ���������� �� ������
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
		}
	}
	else {

		//����������� �������� �� ��� �������� ���������� ��������� ��������
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog <<  std::endl;
		}
	}
}

GLuint ShaderLoader::LoadShader(const char* vertexPath, const char* fragmentPath) {

	std::string vertexCode;
	std::string fragmentCode;
	//������ ������ ��� ������ ����������� ������ ��������
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.open(vertexPath);
	fShaderFile.open(fragmentPath);
	std::stringstream vShaderStream, fShaderStream;

	//������ ����������� ������
	vShaderStream << vShaderFile.rdbuf();
	fShaderStream << fShaderFile.rdbuf();

	vShaderFile.close();
	fShaderFile.close();

	//�������������� std::stringstream -> std::string
	vertexCode = vShaderStream.str();
	fragmentCode = fShaderStream.str();

	//�������������� std::string -> const char*, ��������� ����� ��� ������ ����� ��� ���������� ��������
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	GLuint vertex, fragment;
	//�������� ���������� �������
	vertex = glCreateShader(GL_VERTEX_SHADER);
	//��� ������� ����������� � �������� �������
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	//���������� �������
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");

	//�������� ������������ �������
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	//������� �������������� -> ���������� �� ������ � ��������� ��������
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	//�������� ������������ �����, ������� ����� ���� �������� � GPU ��� ����������
	glLinkProgram(program);
	checkCompileErrors(program, "PROGRAM");

	//�������� �������� ��������� ��� ��� ������� � ���������
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return program;
}