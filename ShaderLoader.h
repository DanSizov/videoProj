#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include<GL/glew.h>
#include<fstream>
#include<sstream>
#include<iostream>

class ShaderLoader
{
public:
	//загрузка, компил€ци€ и св€зывание шейдеров
	static GLuint LoadShader(const char* vertexPath, const char* fragmentPath);

	//проверка наличи€ ошибок при компли€ции или св€зывании шейдеров
	static void checkCompileErrors(GLuint shader, std::string type);
};


#endif // !SHADER_LOADER_H


