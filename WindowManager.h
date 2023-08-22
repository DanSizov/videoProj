#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<iostream>

class WindowManager
{
private:
	
	GLFWwindow* window;
	int width;
	int height;

public:
	WindowManager(const int& width, const int& height, const char* title, GLFWwindow* share = nullptr);
	bool Close() const;
	void swapBuffers();
	void pollEvents();
	int getWidth() const;
	int getHeight() const;
	GLFWwindow* getWindow() const;

	~WindowManager();
};

#endif // !WINDOW_MANAGER_H

