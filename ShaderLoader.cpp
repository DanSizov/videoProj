#include "ShaderLoader.h"

void ShaderLoader::checkCompileErrors(GLuint shader, std::string type) {

	//хранение результата проверки ошибок
	GLint success;
	//хранение сообщени€ об ошибке, если есть
	GLcharARB infoLog[1024];

	//если тип шейдера не PROGRAM, будет вызван метод glGetShaderiv дл€ проверки статуса компил€ции шейдера
	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {

			//вызов метода дл€ получени€ информации об ошибке
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
		}
	}
	else {

		//аналогичные действи€ но дл€ проверки св€зывани€ программы шейдеров
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
	//потоки файлов дл€ чтени€ содержимого файлов шейдеров
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.open(vertexPath);
	fShaderFile.open(fragmentPath);
	std::stringstream vShaderStream, fShaderStream;

	//чтение содержимого файлов
	vShaderStream << vShaderFile.rdbuf();
	fShaderStream << fShaderFile.rdbuf();

	vShaderFile.close();
	fShaderFile.close();

	//преобразование std::stringstream -> std::string
	vertexCode = vShaderStream.str();
	fragmentCode = fShaderStream.str();

	//преобразование std::string -> const char*, поскольку такой тип данных нужен дл€ компил€ции шейдеров
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	GLuint vertex, fragment;
	//создание вершинного щейдера
	vertex = glCreateShader(GL_VERTEX_SHADER);
	//код шейдера св€зываетс€ с объектом шейдера
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	//компил€ци€ шейдера
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");

	//создание фрагментного шейдера
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	//шейдеры скомпилированы -> св€зывание их вместе в программе шейдеров
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	//создание исполн€емого файла, который может быть загружен в GPU дл€ рендеринга
	glLinkProgram(program);
	checkCompileErrors(program, "PROGRAM");

	//удаление шейдеров поскольку они уже св€заны в программу
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return program;
}