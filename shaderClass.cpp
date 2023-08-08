#include"shaderClass.h"

std::string get_file_contents(const char* filename) {

	std::ifstream in(filename, std::ios::binary);
	if (in) {

		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile) {
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	//Эта строка создает вершинный шейдер. GL_VERTEX_SHADER — это константа, которая сообщает OpenGL, что мы создаем вершинный шейдер. Функция glCreateShader возвращает идентификатор шейдера, который сохраняется в vertexShader.
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//Эта строка устанавливает исходный код для шейдера. Параметры функции glShaderSource — это идентификатор шейдера, количество строк в исходном коде шейдера, исходный код шейдера и массив, содержащий длину каждой строки исходного кода.
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	//Эта строка компилирует шейдер. Если есть какие-либо ошибки компиляции, они могут быть получены и напечатаны с помощью других функций OpenGL.
	glCompileShader(vertexShader);
	compileErrors(vertexShader, "VERTEX");

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	compileErrors(fragmentShader, "FRAGMENT");
	//Этот код создает программу шейдеров, аттачит к ней вершинный и фрагментный шейдеры, связывает программу, а затем удаляет отдельные шейдеры, которые теперь уже не нужны. В итоге у нас есть скомпилированная и связанная программа шейдеров, 
	//которую можно использовать для рендеринга
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	compileErrors(ID, "PROGRAM");

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::Activate() {
	glUseProgram(ID);
}

void Shader::Delete() {
	glDeleteProgram(ID);
}

void Shader::compileErrors(unsigned int shader, const char* type) {
	GLint hasCompiled;
	char infoLog[1024];
	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for: " << type << "\n" << std::endl;
		}
	}
	else {
		glGetProgramiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for: " << type << "\n" << std::endl;
		}
	}
}