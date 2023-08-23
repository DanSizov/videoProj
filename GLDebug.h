#ifndef GLDEBUG_H
#define GLDEBUG_H

#include <GL/glew.h>
#include <iostream>
#include <string>

class GLDebug
{
public:
	static void CheckError(const std::string& description);
};



#endif // !GLDEBUG_H

