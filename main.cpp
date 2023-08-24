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
#include"WindowManager.h"
#include"TextureManager.h"
#include"ArucoMarkerManager.h"
#include"GLDebug.h"

#define GL_CALL(x) x; GLDebug::CheckError(#x, __LINE__);

int width{ 640 };
int height{ 480 };

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

	//копирование матрицы вращения в матрицу модели
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			model[j][i] = (float)rotationMatrix(i, j);
		}
	}

	return model;
}

void processMarkersAndDrawCubes(cv::Mat& frame, ArucoMarkerManager& arucoManager, GLuint& textureCube, GLint& locationModelCube, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs, VAO& VAO) {
	std::vector<int> ids;
	std::vector<cv::Vec3d> rvecs, tvecs;
	std::vector<std::vector<cv::Point2f>> corners;

	cv::aruco::detectMarkers(frame, arucoManager.getDictionary(), corners, ids);

	if (ids.size() > 0) {
		cv::aruco::estimatePoseSingleMarkers(corners, 0.06, cameraMatrix, distCoeffs, rvecs, tvecs);
		for (int i = 0; i < ids.size(); i++) {
			cv::Point2f textPosition = corners[i][0];
			std::string idStr = std::to_string(ids[i]);
			cv::putText(frame, idStr, textPosition, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 2);
			arucoManager.drawAxis(frame, rvecs[i], tvecs[i], 0.1);
			arucoManager.drawCube(frame, rvecs[i], tvecs[i], 0.05);
			glm::mat4 markerModel = convertRodriguesToMat4(rvecs[i], tvecs[i]);

			ShaderHelper::PassMatrix(glm::value_ptr(markerModel), locationModelCube);
			VAO.Bind();
			glBindTexture(GL_TEXTURE_2D, textureCube);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			VAO.Unbind();
		}
	}
}

int main() {

	cv::Mat cameraMatrix;
	cv::Mat distCoeffs;
	glfwWindowHint(GLFW_DEPTH_BITS, 24);

	WindowManager window1(640, 480, "window1", nullptr);

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

	std::vector<int> ids;
	std::vector<std::vector<cv::Point2f>> corners;
	std::vector<cv::Vec3d> rvecs, tvecs;

	arucoManager.detectAndEstimatePose(frame, ids, corners,0.05, rvecs, tvecs);

	for (int i = 0; i < ids.size(); i++) {
		arucoManager.drawAxis(frame, rvecs[i], tvecs[i], 0.05);
	}

	Shader shaderProgram1("default.vert", "default.frag");
	shaderProgram1.Activate();

	TextureManager textureManager;

	GLuint texture1 = textureManager.LoadTextureFromFrame(frame);
	GLuint texture2 = textureManager.LoadTextureFromFrame(frame);

	VAO fullScreenVAO1;
	VBO fullScreenVBO1(verticesFullScreenTest, sizeof(verticesFullScreenTest));

	VAO fullScreenVAO2;
	VBO fullScreenVBO2(verticesFullScreenRight, sizeof(verticesFullScreenRight));

	//загрузка и компиляция шейдеров

	VAO VAOCube;
	VBO VBOCube(cubeVerticesMarker, sizeof(cubeVerticesMarker));
	EBO EBOCube(indicesCube, sizeof(indicesCube));

	VAOCube.Bind();
	VBOCube.Bind();
	VAOCube.BindEBO(EBOCube);

	VAOCube.LinkAttrib(VBOCube, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
	VAOCube.LinkAttrib(VBOCube, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));


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
	GLuint textureCube = textureManager.LoadTextureFromFile("brick.png");

	//ширина, высота, колияество каналов изображения
	float rotationAngle = 0.0f;

	fullScreenVAO1.Bind();
	fullScreenVBO1.Bind();
	fullScreenVAO1.LinkAttrib(fullScreenVBO1, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
	fullScreenVAO1.LinkAttrib(fullScreenVBO1, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	fullScreenVBO1.Unbind();
	fullScreenVAO1.Unbind();

	fullScreenVAO2.Bind();
	fullScreenVBO2.Bind();
	fullScreenVAO2.LinkAttrib(fullScreenVBO2, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
	fullScreenVAO2.LinkAttrib(fullScreenVBO2, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	fullScreenVBO2.Unbind();
	fullScreenVAO2.Unbind();

	glm::vec3 cubePos = glm::vec3(1.0f, 0.0f, 1.0f);

	glm::mat4 modelCube = glm::mat4(1.0f);
	modelCube = glm::translate(modelCube, cubePos);
	modelCube = glm::rotate(modelCube, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));

	auto locationModelCube = glGetUniformLocation(shaderProgram1.ID, "model");
	ShaderHelper::PassMatrix(glm::value_ptr(modelCube), locationModelCube);


	glm::mat4 staticCameraMatrix = glm::mat4(1.0f);
	auto locationCamMatrix = glGetUniformLocation(shaderProgram1.ID, "camMatrix");

	double lastTime = glfwGetTime();
	int nbFrames = 0;

	GL_CALL(glEnable(GL_DEPTH_TEST));


	while (!window1.Close())
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

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glViewport(0, 0, width, height);
		//shaderProgram1.Activate();
		//camera.resetMatrix();
		//ShaderHelper::PassMatrix(glm::value_ptr(staticCameraMatrix), locationModel);
		//camera.Matrix(shaderProgram1, "camMatrix");
		//glDisable(GL_DEPTH_TEST);
		//fullScreenVAO1.Bind();
		//fullScreenVBO1.Bind();
		//GL_CALL(glBindTexture(GL_TEXTURE_2D, texture1));
		//GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
		//fullScreenVAO1.Unbind();
		//fullScreenVBO1.Unbind();
		//glEnable(GL_DEPTH_TEST);
		//textureManager.UpdateTexture(texture1, frame);
		//processMarkersAndDrawCubes(frame, arucoManager, textureCube, locationModelCube, cameraMatrix, distCoeffs, VAO2);


		//glm::mat4 originalCamMatrix = camera.getMatrix();


		//camera.setMatrix(originalCamMatrix);
		//camera.Matrix(shaderProgram1, "camMatrix");
		//glClear(GL_DEPTH_BUFFER_BIT);
		//camera.Inputs(window1.GetWindow());
		//camera.updateMatrix(45.0f, 0.1f, 100.0f);



		//window1.SwapBuffers();
		//window1.PollEvents();

		////////////////////////////////////////////
		//window1.MakeContextCurrent();
		//GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		//arucoManager.detectAndEstimatePose(frame, ids, corners, 0.05, rvecs, tvecs);
		//if (ids.size() > 0) {
		//	for (int i = 0; i < ids.size(); i++) {
		//		arucoManager.drawAxis(frame, rvecs[i], tvecs[i], 0.1);
		//		arucoManager.drawCube(frame, rvecs[i], tvecs[i], 0.05);
		//	}
		//}
		//textureManager.UpdateTexture(texture1, frame);  // Обновляем текстуру OpenGL с новым изображением

		//// 2. Рендеринг с помощью OpenGL
		//glViewport(0, 0, width, height);
		//shaderProgram1.Activate();

		//glm::mat4 originalCamMatrix = camera.getMatrix();
		//camera.resetMatrix();
		//ShaderHelper::PassMatrix(glm::value_ptr(staticCameraMatrix), locationModel);
		//camera.Matrix(shaderProgram1, "camMatrix");

		//// Рендеринг изображения на весь экран
		//GL_CALL(glDisable(GL_DEPTH_TEST));
		//fullScreenVAO1.Bind();
		//GL_CALL(glBindTexture(GL_TEXTURE_2D, texture1));
		//GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
		//fullScreenVAO1.Unbind();

		//// Рендеринг 3D-кубов
		//GL_CALL(glEnable(GL_DEPTH_TEST));
		//camera.setMatrix(originalCamMatrix);
		//camera.Matrix(shaderProgram1, "camMatrix");
		//GL_CALL(glClear(GL_DEPTH_BUFFER_BIT));
		//camera.Inputs(window1.GetWindow());
		//camera.updateMatrix(45.0f, 0.1f, 100.0f);
		//processMarkersAndDrawCubes(frame, arucoManager, textureCube, locationModelCube, cameraMatrix, distCoeffs, VAOCube);

		//window1.SwapBuffers();
		//window1.PollEvents();
		////////////////////////////////////////////////////

				window1.MakeContextCurrent();
		GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		glViewport(0, 0, width, height);
		shaderProgram1.Activate();
		glm::mat4 originalCamMatrix = camera.getMatrix();
		camera.resetMatrix();
		ShaderHelper::PassMatrix(glm::value_ptr(staticCameraMatrix), locationModel);
		camera.Matrix(shaderProgram1, "camMatrix");
		GL_CALL(glDisable(GL_DEPTH_TEST));
		fullScreenVAO1.Bind();
		fullScreenVBO1.Bind();
		GL_CALL(glBindTexture(GL_TEXTURE_2D, texture1));
		GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
		fullScreenVAO1.Unbind();
		fullScreenVBO1.Unbind();
		GL_CALL(glEnable(GL_DEPTH_TEST));
		textureManager.UpdateTexture(texture1, frame);
		camera.setMatrix(originalCamMatrix);
		camera.Matrix(shaderProgram1, "camMatrix");
		GL_CALL(glClear(GL_DEPTH_BUFFER_BIT));
		camera.Inputs(window1.GetWindow());
		camera.updateMatrix(45.0f, 0.1f, 100.0f);
		processMarkersAndDrawCubes(frame, arucoManager, textureCube, locationModelCube, cameraMatrix, distCoeffs, VAOCube);

		window1.SwapBuffers();
		window1.PollEvents();
	}

	//освобождаем ресурсы

	fullScreenVAO1.Delete();
	fullScreenVBO1.Delete();
	fullScreenVAO2.Delete();
	fullScreenVBO2.Delete();

	textureManager.DeleteTexture(texture1);
	textureManager.DeleteTexture(texture2);
	textureManager.DeleteTexture(textureCube);
	shaderProgram1.Delete();

	cap.release();

	glfwTerminate();
	return 0;
}