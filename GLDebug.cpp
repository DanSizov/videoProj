#include "GLDebug.h"

void GLDebug::CheckError(const std::string& description) {
	GLenum error = glGetError();
	while (error != GL_NO_ERROR) {
		std::string errorStr;
		switch (error)
		{
		case GL_INVALID_ENUM: 
			errorStr = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:     
			errorStr = "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION: 
			errorStr = "GL_INVALID_OPERATION";
			break;
		case GL_OUT_OF_MEMORY:     
			errorStr = "GL_OUT_OF_MEMORY";
			break;
		default:                   
			errorStr = "UNKNOWN_ERROR";
			break;
		}
		std::cerr << description << ": " << errorStr << std::endl;
		error = glGetError();
	}
}
