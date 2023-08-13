#define GLEW_STATIC
#define STB_IMAGE_IMPLEMENTATION

#include<opencv2/opencv.hpp>
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<stb/stb_image.h>
//#include <opencv2/aruco.hpp>

#include"EBO.h"
#include"VAO.h"
#include"VBO.h"
#include"Vertices.h"
#include "Indices.h" 
#include"ShaderHelper.h"
#include"shaderClass.h"
#include"Camera.h"

int width, height;
//матрица для преобразования вершин
GLfloat matrix[16] = {
	0.75, 0.0, 0.0, 0.0,
	0.0, 0.75, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0
};

//коэффициент для преобразования изображения в шейдере
GLfloat color = 0.75;

int main() {

	//инициализация GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	//создание окна
	GLFWwindow* window1 = glfwCreateWindow(640, 480, "window1", nullptr, nullptr);
	if (window1 == nullptr) {
		std::cerr << "Failed to create window1" << std::endl;
		glfwTerminate();
		return -1;
	}

	GLFWwindow* window2 = glfwCreateWindow(640, 480, "window2", nullptr, window1);
	if (window2 == nullptr) {
		std::cerr << "Failed to create window2" << std::endl;
		glfwTerminate();
		return -1;
	}

	//function to free resources

	//установка контекста
	glfwMakeContextCurrent(window1);

	//инициализация GLEW
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return -1;
	}


	//открытие камеры с помощью opencv
	cv::VideoCapture cap(0);
	if (!cap.isOpened()) {
		std::cerr << "Failed to open camera" << std::endl;
		return -1;
	}

	//получение первого кадра для определения размера видео
	cv::Mat frame;
	cap >> frame;
	if (frame.empty()) {
		std::cerr << "Failed to capture video" << std::endl;
		return -1;
	}

	//создание текстуры opengl для отображения видео
	//создание имени для текстуры и сохранение его в переменной
	GLuint texture1;
	glGenTextures(1, &texture1);
	//используем нашу текстуру как текущую 2D текстуру
	glBindTexture(GL_TEXTURE_2D, texture1);
	//загружаем данные изображения в текущую текстуру, определеяем размеры изображения и последующие данные
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
	//задаем параметр фильтрации для текстуры. минимальный линейный фильтр -> OpenGL будет использовать линейную интерполяцию для выбора цвета текстуры, когда она уменьшается //?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//устанавливаем магнитный фильтр на линейный для линейной интерполяции при увеличении текстуры
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	/////////
	VAO fullScreenVAO;
	VBO fullScreenVBO1(verticesHalf1, sizeof(verticesHalf1));
	VBO fullScreenVBO2(verticesHalf2, sizeof(verticesHalf2));
	////////
	//загрузка и компиляция шейдеров

	VAO VAO1;
	VBO VBO1(verticesOriginal, sizeof(verticesOriginal));
	EBO EBO1(indices, sizeof(indices));

	GLuint texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	Shader shaderProgram2("matrix.vert", "green.frag");
	shaderProgram2.Activate();

	auto locationMatrix = glGetUniformLocation(shaderProgram2.ID, "sh_matrix");
	auto locationVariable = glGetUniformLocation(shaderProgram2.ID, "colorVariable");

	Shader shaderProgram1("default.vert", "default.frag");
	shaderProgram1.Activate();

	Camera camera(800, 800, glm::vec3(1.0f, 0.0f, 5.0f));
	camera.Matrix(shaderProgram1, "camMatrix");

	//создание и передача матрицы в шейдер, все координаты вершин модели будут преобразованы из локального пространства в мировое и будут перемещены на 1 единицу от камеры
	//модельная матрица для вращения, перемещения, масштабирования объекта
	glm::mat4 model1 = glm::mat4(1.0f);
	model1 = glm::translate(model1, glm::vec3(0.0f, 0.0f, 1.0f));
	model1 = glm::rotate(model1, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	auto locationModel = glGetUniformLocation(shaderProgram1.ID, "model");
	ShaderHelper::PassMatrix(glm::value_ptr(model1), locationModel);

	glm::mat4 model2 = glm::mat4(1.0f);
	model2 = glm::translate(model2, glm::vec3(2.0f, 0.0f, 1.0f));
	model2 = glm::rotate(model2, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ShaderHelper::PassMatrix(glm::value_ptr(model2), locationModel);

	//cube
	//ширина, высота, колияество каналов изображения
	int widthIm, heightIm, nrChannels;
	float rotationAngle = 0.0f;
	//загрузка изображения
	unsigned char* data = stbi_load("brick.png", &widthIm, &heightIm, &nrChannels, 0);
	GLuint textureCube;

	if (data) {
		glGenTextures(1, &textureCube);
		glBindTexture(GL_TEXTURE_2D, textureCube);

		//повторение по горизонтали
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//повторение по вертикали
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//фильтры
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (nrChannels == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthIm, heightIm, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else if (nrChannels == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthIm, heightIm, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		//генерация мипмапов для текстуры
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load the texture" << std::endl;
	}
	//освобождение памяти
	stbi_image_free(data);

	VAO VAO2;
	VBO VBO2(cubeVertices, sizeof(cubeVertices));
	EBO EBO2(indicesCube, sizeof(indicesCube));

	VAO2.Bind();
	VBO2.Bind();
	VAO2.BindEBO(EBO2);

	VAO2.LinkAttrib(VBO2, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
	VAO2.LinkAttrib(VBO2, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glm::vec3 cubePos = glm::vec3(1.0f, 0.0f, 1.0f);

	glm::mat4 modelCube = glm::mat4(1.0f);
	modelCube = glm::translate(modelCube, cubePos);
	//modelCube = glm::rotate(modelCube, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	auto locationModelCube = glGetUniformLocation(shaderProgram1.ID, "model");
	ShaderHelper::PassMatrix(glm::value_ptr(modelCube), locationModelCube);

	glEnable(GL_DEPTH_TEST);

	glm::mat4 staticCameraMatrix = glm::mat4(1.0f);
	auto locationCamMatrix = glGetUniformLocation(shaderProgram1.ID, "camMatrix");


	while (!glfwWindowShouldClose(window1) && !glfwWindowShouldClose(window2))
	{
		//захват нового кадра
		cap >> frame;
		if (frame.empty()) {
			break;
		}

		glfwMakeContextCurrent(window1);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		glm::mat4 originalCamMatrix = camera.getMatrix();
		camera.resetMatrix();
		camera.Matrix(shaderProgram1, "camMatrix");

		shaderProgram1.Activate();
		ShaderHelper::PassMatrix(glm::value_ptr(staticCameraMatrix), locationModel);
		fullScreenVAO.Bind();
		fullScreenVBO1.Bind();

		fullScreenVAO.LinkAttrib(fullScreenVBO1, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
		fullScreenVAO.LinkAttrib(fullScreenVBO1, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		glBindTexture(GL_TEXTURE_2D, texture1);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		fullScreenVBO1.Unbind();
		fullScreenVBO2.Bind();
		fullScreenVAO.LinkAttrib(fullScreenVBO2, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
		fullScreenVAO.LinkAttrib(fullScreenVBO2, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindTexture(GL_TEXTURE_2D, texture1);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		fullScreenVAO.Unbind();
		fullScreenVBO2.Unbind();

		camera.setMatrix(originalCamMatrix);
		camera.Matrix(shaderProgram1, "camMatrix");
		glClear(GL_DEPTH_BUFFER_BIT);
		//glEnable(GL_DEPTH_TEST);
		camera.Inputs(window1);
		camera.updateMatrix(45.0f, 0.1f, 100.0f);
		//camera.Matrix(shaderProgram1, "view");
		//camera.Matrix(shaderProgram1, "camMatrix");

		VAO1.Bind();
		VBO1.Bind();
		VAO1.BindEBO(EBO1);

		VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
		VAO1.LinkAttrib(VBO1, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		glEnable(GL_DEPTH_TEST);

		//активация шейдерной программы
		//обновление текстуры
		glBindTexture(GL_TEXTURE_2D, texture1);
		//замена данных текущей текстуры данными из нового кадра
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
		//очистка буферов, цвет черный 
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glfwGetFramebufferSize(window1, &width, &height);
		glViewport(0, 0, width, height);

		//передача матрицы для каждого изображения для их ориентации в пространстве
		ShaderHelper::PassMatrix(glm::value_ptr(model1), locationModel);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		ShaderHelper::PassMatrix(glm::value_ptr(model2), locationModel);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		VAO2.Bind();
		//передача матрицы для ориентации в пространстве
		modelCube = glm::rotate(modelCube, 0.05f, glm::vec3(0.0f, 1.0f, 0.0f));
		ShaderHelper::PassMatrix(glm::value_ptr(modelCube), locationModelCube);
		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureCube);
		//отрисовка куба, 36 вершин 
		glDrawArrays(GL_TRIANGLES, 0, 36);

		VAO2.Unbind();

		glfwSwapBuffers(window1);
		VAO1.Unbind();
		VBO1.Unbind();
		VAO1.UnbindEBO(EBO1);



		glfwMakeContextCurrent(window2);
		shaderProgram2.Activate();

		ShaderHelper::PassMatrix(matrix, locationMatrix);
		ShaderHelper::PassVariable(color, locationVariable);

		VAO1.Bind();
		VBO1.Bind();
		VAO1.BindEBO(EBO1);

		VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
		VAO1.LinkAttrib(VBO1, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		glBindTexture(GL_TEXTURE_2D, texture2);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glfwGetFramebufferSize(window2, &width, &height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window2);
		VAO1.Unbind();
		VBO1.Unbind();
		VAO1.UnbindEBO(EBO1);

		glfwPollEvents();
	}

	//освобождаем ресурсы
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	
	fullScreenVAO.Delete();
	fullScreenVBO1.Delete();
	fullScreenVBO2.Delete();
	fullScreenVBO2.Delete();

	glDeleteTextures(1, &texture1);
	glDeleteTextures(1, &texture2);
	shaderProgram1.Delete();
	shaderProgram2.Delete();

	cap.release();

	glfwTerminate();
	return 0;
}