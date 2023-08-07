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

#include"EBO.h"
#include"VAO.h"
#include"VBO.h"
#include"Vertices.h"
#include "Indices.h" 
#include"ShaderLoader.h"
#include"ShaderHelper.h"

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

//последнее положение курсора мыши
float lastX = 400.0f;
float lastY = 300.0f;
//поворот вокруг Х (вверх и вниз!)
float pitch = 0.0f;
//поворот аокруг Y (влево, вправо)
float yaw = -90.0f;
//вектор, указывающий направление, в котором смотрит камера (вперед)
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
float cameraSpeed = 0.05f;
//true когда левая кнопка нажата, false когда отпущена
bool mousePressed = false;
//обнаружение первого события движения мыши для исключения скачка в положении курсора
bool firstMouseMove = true;

//обновление положения и ориентации камеры в соответствии перемещения мыши
void MouseCallback(GLFWwindow* window, double xpos, double ypos) {

	//проверка, является ли текущее перемещение первым после нажатия кнопки мыши
	if (firstMouseMove) {
		lastX = xpos;
		lastY = ypos;
		firstMouseMove = false;
	}

	//вычисление положения курсора по сравнению с последним известным положением, значения инвертированы тк в GLFW координаты увеличиваются сверху вниз
	float xoffset = -xpos + lastX;
	float yoffset = -lastY + ypos;
	//обновление переменных до текущего положения курсора
	lastX = xpos;
	lastY = ypos;
	//чувствительность к перемещению камеры
	float sensitivity = 0.1f;
	glm::vec3 direction;

	//если нажата левая кнопка
	if (mousePressed) {
		//изменения положения курсора масштабируются
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		//обновление углов поворота камеры на основе изменения положения курсора
		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		//вычисление нового направления камеры на основе углов камеры
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		//вектор направления камеры обновляется и нромализуется
		cameraFront = glm::normalize(direction);
	}
}

//void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
//
//	if (button == GLFW_MOUSE_BUTTON_LEFT) {
//		if (action == GLFW_PRESS) {
//			mousePressed = true;
//		}
//		else if (action == GLFW_RELEASE) {
//			mousePressed = false;
//		}
//	}
//}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	mousePressed = (button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS);
}

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
	// 
	// 
	// 
	// 
	// 
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

	//загрузка и компиляция шейдеров
	GLuint shaderProgram1 = ShaderLoader::LoadShader("default.vert", "default.frag");

	VAO VAO1;
	VBO VBO1(verticesOriginal, sizeof(verticesOriginal));
	EBO EBO1(indices, sizeof(indices));

	GLuint texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GLuint shaderProgram2 = ShaderLoader::LoadShader("matrix.vert", "green.frag");

	ShaderHelper::PrepareProgram(shaderProgram2);

	auto locationMatrix = ShaderHelper::GetLocation(shaderProgram2, "sh_matrix");
	auto locationVariable = ShaderHelper::GetLocation(shaderProgram2, "colorVariable");

	glUseProgram(shaderProgram1);

	//определение начальной позиции камеры в 3Д пространстве (3 единицы вдоль оси z, смотрящей от экрана)
	glm::vec3 cameraPos = glm::vec3(1.0f, 0.0f, 3.0f);
	//определение вектора, который указывает наверх, камера смотрит по Y
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	//создание и передача матрицы в шейдер, все координаты вершин модели будут преобразованы из локального пространства в мировое и будут перемещены на 1 единицу от камеры
	//модельная матрица для вращения, перемещения, масштабирования объекта
	glm::mat4 model1 = glm::mat4(1.0f);
	model1 = glm::translate(model1, glm::vec3(0.0f, 0.0f, -1.0f));
	model1 = glm::rotate(model1, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	auto locationModel = ShaderHelper::GetLocation(shaderProgram1, "model");
	ShaderHelper::PassMatrix(glm::value_ptr(model1), locationModel);

	glm::mat4 model2 = glm::mat4(1.0f);
	model2 = glm::translate(model2, glm::vec3(2.0f, 0.0f, -1.0f));
	model2 = glm::rotate(model2, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ShaderHelper::PassMatrix(glm::value_ptr(model2), locationModel);

	glm::vec3 target = glm::vec3(1.0f, 0.0f, -1.0f);
	//матрица вида, смотрящая от определенной позиции на определенную точку, преобразование координат модели из мирового пространства в пространство камеры
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	//view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	auto locationView = ShaderHelper::GetLocation(shaderProgram1, "view");
	ShaderHelper::PassMatrix(glm::value_ptr(view), locationView);

	//матрица, определяющая как 3Д сцена будет проецироваться на 2Д экран
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	auto locationProjection = ShaderHelper::GetLocation(shaderProgram1, "projection");
	ShaderHelper::PassMatrix(glm::value_ptr(projection), locationProjection);

	glfwSetCursorPosCallback(window1, MouseCallback);
	glfwSetMouseButtonCallback(window1, MouseButtonCallback);

	//cube
	//ширина, высота, колияество каналов изображения
	int widthIm, heightIm, nrChannels;
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

	glm::vec3 cubePos = glm::vec3(-3.0f, -3.0f, -3.0f);

	glm::mat4 modelCube = glm::mat4(1.0f);
	modelCube = glm::translate(modelCube, cubePos);
	auto locationModelCube = ShaderHelper::GetLocation(shaderProgram1, "model");
	ShaderHelper::PassMatrix(glm::value_ptr(modelCube), locationModelCube);

	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	//основной цикл
	while (!glfwWindowShouldClose(window1) && !glfwWindowShouldClose(window2))
	{
		//захват нового кадра
		cap >> frame;
		if (frame.empty()) {
			break;
		}

		glfwMakeContextCurrent(window1);
		glUseProgram(shaderProgram1);


		if (glfwGetKey(window1, GLFW_KEY_UP) == GLFW_PRESS) {
			cameraPos.z -= cameraSpeed;
		}
		if (glfwGetKey(window1, GLFW_KEY_DOWN) == GLFW_PRESS) {
			cameraPos.z += cameraSpeed;
		}

		//генерация матрицы, которая смотрит от позиции камеры cameraPos вдоль вектора направления камеры cameraFront, последний аргумент - "верхний"
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		auto locationView = ShaderHelper::GetLocation(shaderProgram1, "view");
		ShaderHelper::PassMatrix(glm::value_ptr(view), locationView);


		VAO1.Bind();
		VBO1.Bind();
		VAO1.BindEBO(EBO1);

		VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
		VAO1.LinkAttrib(VBO1, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		//активация шейдерной программы
		//обновление текстуры
		glBindTexture(GL_TEXTURE_2D, texture1);
		//замена данных текущей текстуры данными из нового кадра
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
		//очистка буферов, цвет черный 
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glfwGetFramebufferSize(window1, &width, &height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//передача матрицы для каждого изображения для их ориентации в пространстве
		ShaderHelper::PassMatrix(glm::value_ptr(model1), locationModel);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		ShaderHelper::PassMatrix(glm::value_ptr(model2), locationModel);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		VAO2.Bind();
		//передача матрицы для ориентации в пространстве
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
		glUseProgram(shaderProgram2);

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
	
	glDeleteTextures(1, &texture1);
	glDeleteTextures(1, &texture2);
	glDeleteProgram(shaderProgram1);
	glDeleteProgram(shaderProgram2);

	cap.release();

	glfwTerminate();
	return 0;
}