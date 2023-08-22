#define GLEW_STATIC

#include<opencv2/opencv.hpp>
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<opencv2/calib3d.hpp>
#include<opencv2/aruco.hpp>
#include<filesystem>

#include"EBO.h"
#include"VAO.h"
#include"VBO.h"
#include"Vertices.h"
#include "Indices.h" 
#include"ShaderHelper.h"
#include"ShaderClass.h"
#include"Camera.h"
#include"DrawClass.h"
#include"WindowManager.h"
#include"TextureManager.h"
#include"ArucoMarkerManager.h"

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

//преобразование векторов вращения и трансляции в матрицу модели 4х4
glm::mat4 convertRodriguesToMat4(const cv::Vec3d& rvec, const cv::Vec3d& tvec) {
	//преобразование Rodrigues в матрицу вращения
	cv::Matx33d rotationMatrix;
	cv::Rodrigues(rvec, rotationMatrix);

	glm::mat4 model = glm::mat4(1.0f);
	//добавление вектора трансляции в матрицу модели
	model[3][0] = tvec[0];
	model[3][1] = tvec[1];
	model[3][2] = tvec[2];

	cv::Matx33d opencvToGL = cv::Matx33d(
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0
	);
	rotationMatrix = opencvToGL * rotationMatrix;

	//инициализация матрицы модели

	//копирование матрицы вращения в матрицу модели
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			model[j][i] = (float)rotationMatrix(i, j);
		}
	}

	return model;
}

int main() {


	cv::Mat cameraMatrix;
	cv::Mat distCoeffs;
	WindowManager window1(640, 480, "window1", nullptr);

	//WindowManager window2(640, 480, "window2", window1.getWindow());

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

	cv::FileStorage fs1("camera_parameters.yml", cv::FileStorage::READ);
	fs1["cameraMatrix"] >> cameraMatrix;
	fs1["distCoeffs"] >> distCoeffs;
	fs1.release();

	ArucoMarkerManager arucoManager(cameraMatrix, distCoeffs, cv::aruco::DICT_6X6_250);

	//идентификаторы маркеров и их углов
	std::vector<int> ids;
	//маркеры и их углы
	std::vector<std::vector<cv::Point2f>> corners;
	//поиск маркеров

	std::vector<cv::Vec3d> rvecs, tvecs;
	arucoManager.detectAndEstimatePose(frame, ids, corners,0.05, rvecs, tvecs);

	for (int i = 0; i < ids.size(); i++) {
		arucoManager.drawAxis(frame, rvecs[i], tvecs[i], 0.05);
	}

	TextureManager textureManager;

	GLuint texture1 = textureManager.loadTextureFromFrame(frame);
	GLuint texture2 = textureManager.loadTextureFromFrame(frame);

	VAO fullScreenVAO;
	VBO fullScreenVBO1(verticesHalf1, sizeof(verticesHalf1));
	VBO fullScreenVBO2(verticesHalf2, sizeof(verticesHalf2));
	//загрузка и компиляция шейдеров

	VAO VAO1;
	VBO VBO1(verticesOriginal, sizeof(verticesOriginal));
	EBO EBO1(indices, sizeof(indices));

	Shader shaderProgram2("matrix.vert", "green.frag");
	shaderProgram2.Activate();

	auto locationMatrix = glGetUniformLocation(shaderProgram2.ID, "sh_matrix");
	auto locationVariable = glGetUniformLocation(shaderProgram2.ID, "colorVariable");

	Shader shaderProgram1("default.vert", "default.frag");
	shaderProgram1.Activate();

	Camera camera(800, 800, glm::vec3(1.0f, 0.0f, 5.0f));
	camera.Matrix(shaderProgram1, "camMatrix");

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
	GLuint textureCube = textureManager.loadTextureFromFile("brick.png");

	//ширина, высота, колияество каналов изображения
	float rotationAngle = 0.0f;

	VAO VAO2;
	VBO VBO2(cubeVerticesMarker, sizeof(cubeVerticesMarker));
	EBO EBO2(indicesCube, sizeof(indicesCube));

	VAO2.Bind();
	VBO2.Bind();
	VAO2.BindEBO(EBO2);

	VAO2.LinkAttrib(VBO2, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
	VAO2.LinkAttrib(VBO2, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glm::vec3 cubePos = glm::vec3(1.0f, 0.0f, 1.0f);

	glm::mat4 modelCube = glm::mat4(1.0f);
	modelCube = glm::translate(modelCube, cubePos);
	modelCube = glm::rotate(modelCube, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));

	auto locationModelCube = glGetUniformLocation(shaderProgram1.ID, "model");
	ShaderHelper::PassMatrix(glm::value_ptr(modelCube), locationModelCube);

	glEnable(GL_DEPTH_TEST);

	glm::mat4 staticCameraMatrix = glm::mat4(1.0f);
	auto locationCamMatrix = glGetUniformLocation(shaderProgram1.ID, "camMatrix");

	double lastTime = glfwGetTime();
	int nbFrames = 0;

	//while (!glfwWindowShouldClose(window1) && !glfwWindowShouldClose(window2))
	while (!glfwWindowShouldClose(window1.getWindow()))
	{

		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) {
			std::cout << "FPS: " << nbFrames << std::endl;
			nbFrames = 0;
			lastTime += 1.0;
		}

		//захват нового кадра
		cap >> frame;
		if (frame.empty()) {
			break;
		}

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

		camera.Inputs(window1.getWindow());
		camera.updateMatrix(45.0f, 0.1f, 100.0f);
		//camera.Matrix(shaderProgram1, "view");
		//camera.Matrix(shaderProgram1, "camMatrix");

		//VAO1.Bind();
		//VBO1.Bind();
		//VAO1.BindEBO(EBO1);

		//VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
		//VAO1.LinkAttrib(VBO1, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		glEnable(GL_DEPTH_TEST);

		textureManager.updateTexture(texture1, frame);

		//очистка буферов, цвет черный 
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glfwGetFramebufferSize(window1.getWindow(), &width, &height);
		glViewport(0, 0, width, height);

		//передача матрицы для каждого изображения для их ориентации в пространстве
		//ShaderHelper::PassMatrix(glm::value_ptr(model1), locationModel);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//ShaderHelper::PassMatrix(glm::value_ptr(model2), locationModel);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//VAO1.Unbind();
		//VBO1.Unbind();
		//VAO1.UnbindEBO(EBO1);

		try {
			
			cv::aruco::detectMarkers(frame, arucoManager.getDictionary(), corners, ids);

			if (ids.size() > 0) {
				// Оценка позы маркеров, 6 см - размер стороны квадрата маркера
				cv::aruco::estimatePoseSingleMarkers(corners, 0.06, cameraMatrix, distCoeffs, rvecs, tvecs);
				for (int i = 0; i < ids.size(); i++) {

					// Вычисляем координаты для размещения текста (например, центр первого угла маркера)
					cv::Point2f textPosition = corners[i][0];
					// Преобразуем ID маркера в строку
					std::string idStr = std::to_string(ids[i]);
					// Рисуем ID на изображении
					cv::putText(frame, idStr, textPosition, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 2);
					// Рисуем оси на маркере
					arucoManager.drawAxis(frame, rvecs[i], tvecs[i], 0.1);
					cv::imwrite("debug_image.jpg", frame);
					arucoManager.drawCube(frame, rvecs[i], tvecs[i], 0.05);
					glm::mat4 markerModel = convertRodriguesToMat4(rvecs[i], tvecs[i]);

					ShaderHelper::PassMatrix(glm::value_ptr(markerModel), locationModelCube);
					VAO2.Bind();
					glBindTexture(GL_TEXTURE_2D, textureCube);
					glDrawArrays(GL_TRIANGLES, 0, 36);
					VAO2.Unbind();
				}

				textureManager.updateTexture(texture1, frame);

				shaderProgram1.Activate();
				ShaderHelper::PassMatrix(glm::value_ptr(staticCameraMatrix), locationModel);
				fullScreenVAO.Bind();
				fullScreenVBO1.Bind();

				for (int i = 0; i < ids.size(); i++) {			
					glm::mat4 markerModel = convertRodriguesToMat4(rvecs[i], tvecs[i]);

					ShaderHelper::PassMatrix(glm::value_ptr(markerModel), locationModelCube);
					VAO2.Bind();
					glBindTexture(GL_TEXTURE_2D, textureCube);
					glDrawArrays(GL_TRIANGLES, 0, 36);
					VAO2.Unbind();

				}

				fullScreenVAO.Unbind();
				fullScreenVBO1.Unbind();
			}

			cv::waitKey(1);


		}
		catch (const std::exception& e) {
			std::cerr << "openCV exception: " << e.what() << std::endl;
		}

		window1.swapBuffers();
		window1.pollEvents();

	}

	//освобождаем ресурсы
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();


	fullScreenVAO.Delete();
	fullScreenVBO1.Delete();
	fullScreenVBO2.Delete();
	fullScreenVBO2.Delete();

	textureManager.deleteTexture(texture1);
	textureManager.deleteTexture(texture2);
	textureManager.deleteTexture(textureCube);
	shaderProgram1.Delete();
	shaderProgram2.Delete();

	cap.release();

	glfwTerminate();
	return 0;
}