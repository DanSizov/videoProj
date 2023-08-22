#include "WindowManager.h"

WindowManager::WindowManager(const int& width, const int& height, const char* title, GLFWwindow* share) {
	if (!glfwInit()) {
		std::cerr << "Failed to inititalize GLFW" << std::endl;
		exit(-1);
	}

	window = glfwCreateWindow(width, height, title, nullptr, share);
	if (window == nullptr) {
		std::cerr << "Failed to create window" << std::endl;
		glfwTerminate();
		exit(-1);
	}

	this->width = width;
	this->height = height;

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		exit(-1);
	}
}

bool WindowManager::Close() const {
	return glfwWindowShouldClose(window);
}

void WindowManager::swapBuffers() {
	glfwSwapBuffers(window);
}

void WindowManager::pollEvents() {
	glfwPollEvents();
}

int WindowManager::getWidth() const { return this->width; }
int WindowManager::getHeight() const { return this->height; }
GLFWwindow* WindowManager::getWindow() const { return this->window; }

WindowManager::~WindowManager() {
	glfwTerminate();
}