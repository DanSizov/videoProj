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

	static bool glfwInitialized;
	static int windowCount;

public:
	WindowManager(const int& width, const int& height, const char* title, GLFWwindow* share = nullptr);
	void MakeContextCurrent();
	bool Close() const;
	void SwapBuffers();
	void PollEvents();
	int GetWidth() const;
	int GetHeight() const;
	GLFWwindow* GetWindow() const;

	~WindowManager();
};

#endif // !WINDOW_MANAGER_H

