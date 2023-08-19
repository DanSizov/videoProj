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
#include<opencv2/calib3d.hpp>
#include<opencv2/aruco.hpp>
#include<filesystem>

#include"EBO.h"
#include"VAO.h"
#include"VBO.h"
#include"Vertices.h"
#include "Indices.h" 
#include"ShaderHelper.h"
#include"shaderClass.h"
#include"Camera.h"

int width, height;
//������� ��� �������������� ������
GLfloat matrix[16] = {
	0.75, 0.0, 0.0, 0.0,
	0.0, 0.75, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0
};

//����������� ��� �������������� ����������� � �������
GLfloat color = 0.75;

//������������ ���������� ���� ��������� �� ��������� �����������
//image - �����������, �� ������� ����� ���������� ���
//cameraMatrix - ������� ���������� ���������� ������
//distCoeffs - ������������ ��������� ������
//rvec - ������ ��������, ������� ��������� �������� ����� ������� � �������
//tvec - ������ ��������, ������� ��������� �������� ����� ������� � �������
//length - ����� ���� ���������
void drawAxis(cv::Mat& image, cv::Mat& cameraMatrix, cv::Mat& distCoeffs, cv::Vec3d& rvec, cv::Vec3d& tvec, float length = 50) {
	//������, ���������� 4 �����: ������ ���� � 3 ����������
	std::vector<cv::Point3f> axisPoints;
	axisPoints.push_back(cv::Point3f(0, 0, 0));
	axisPoints.push_back(cv::Point3f(length, 0, 0));
	axisPoints.push_back(cv::Point3f(0, length, 0));
	axisPoints.push_back(cv::Point3f(0, 0, length));
	//������, ������� �������� ��������������� 2� ���������� �� ����������� ��� ������ �� 3� �����
	std::vector<cv::Point2f> imagePoints;
	//������������� 3� ����� �� 2� ����������� � ������ ���������� ������ � ������� �������
	cv::projectPoints(axisPoints, rvec, tvec, cameraMatrix, distCoeffs, imagePoints);

	//��������� ����� ����� ����������������� �������
	cv::line(image, imagePoints[0], imagePoints[1], cv::Scalar(0, 0, 255), 3); // X axis in red
	cv::line(image, imagePoints[0], imagePoints[2], cv::Scalar(0, 255, 0), 3); // Y axis in green
	cv::line(image, imagePoints[0], imagePoints[3], cv::Scalar(255, 0, 0), 3); // Z axis in blue
}

//�������������� �������� �������� � ���������� � ������� ������ 4�4
glm::mat4 convertRodriguesToMat4(const cv::Vec3d& rvec, const cv::Vec3d& tvec) {
	//�������������� Rodrigues � ������� ��������
	cv::Matx33d rotationMatrix;
	cv::Rodrigues(rvec, rotationMatrix);

	glm::mat4 model = glm::mat4(1.0f);
	//���������� ������� ���������� � ������� ������
	model[3][0] = tvec[0];
	model[3][1] = -tvec[1];
	model[3][2] = -tvec[2];

	cv::Matx33d opencvToGL = cv::Matx33d(
		1.0, 0.0, 0.0,
		0.0, -1.0, 0.0,
		0.0, 0.0, -1.0
	);
	rotationMatrix = opencvToGL * rotationMatrix;

	//������������� ������� ������

	//����������� ������� �������� � ������� ������
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			model[i][j] = (float)rotationMatrix(i, j);
		}
	}

	return model;
}

int main() {


	cv::Mat cameraMatrix, distCoeffs;


	//������������� GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	//�������� ����
	GLFWwindow* window1 = glfwCreateWindow(640, 480, "window1", nullptr, nullptr);
	if (window1 == nullptr) {
		std::cerr << "Failed to create window1" << std::endl;
		glfwTerminate();
		return -1;
	}

	//GLFWwindow* window2 = glfwCreateWindow(640, 480, "window2", nullptr, window1);
	//if (window2 == nullptr) {
	//	std::cerr << "Failed to create window2" << std::endl;
	//	glfwTerminate();
	//	return -1;
	//}

	//function to free resources

	//��������� ���������
	glfwMakeContextCurrent(window1);

	//������������� GLEW
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return -1;
	}


	//�������� ������ � ������� opencv
	cv::VideoCapture cap(0);
	if (!cap.isOpened()) {
		std::cerr << "Failed to open camera" << std::endl;
		return -1;
	}

	//��������� ������� ����� ��� ����������� ������� �����
	cv::Mat frame;
	cap >> frame;
	if (frame.empty()) {
		std::cerr << "Failed to capture video" << std::endl;
		return -1;
	}

	//������������� ������� ��� ����������� �������� �� �����������
	cv::Ptr<cv::aruco::Dictionary> dictionary = cv::makePtr<cv::aruco::Dictionary>(cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250));

	cv::FileStorage fs1("camera_parameters.yml", cv::FileStorage::READ);
	fs1["cameraMatrix"] >> cameraMatrix;
	fs1["distCoeffs"] >> distCoeffs;
	fs1.release();

	//�������������� �������� � �� �����
	std::vector<int> ids;
	//������� � �� ����
	std::vector<std::vector<cv::Point2f>> corners;
	//����� ��������
	//cv::aruco::detectMarkers(undistorted, dictionary, corners, ids);
	cv::aruco::detectMarkers(frame, dictionary, corners, ids);

	//����������� ������������ �������� � ������ �� ����
	if (ids.size() > 0) {
		//cv::aruco::drawDetectedMarkers(undistorted, corners, ids);
		cv::aruco::drawDetectedMarkers(frame, corners, ids);

		std::vector<cv::Vec3d> rvecs, tvecs;
		//cv::aruco::estimatePoseSingleMarkers(corners, squareSize, cameraMatrix, distCoeffs, rvecs, tvecs);
		cv::aruco::estimatePoseSingleMarkers(corners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);

		for (int i = 0; i < ids.size(); i++) {
			drawAxis(frame, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.05);
		}
		//cv::imshow("Detected ArUco markers", undistorted);
		cv::imshow("Detected ArUco markers", frame);

		cv::waitKey(0);
	}

	//�������� �������� opengl ��� ����������� �����
	//�������� ����� ��� �������� � ���������� ��� � ����������
	GLuint texture1;
	glGenTextures(1, &texture1);
	//���������� ���� �������� ��� ������� 2D ��������
	glBindTexture(GL_TEXTURE_2D, texture1);
	//��������� ������ ����������� � ������� ��������, ����������� ������� ����������� � ����������� ������
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
	//������ �������� ���������� ��� ��������. ����������� �������� ������ -> OpenGL ����� ������������ �������� ������������ ��� ������ ����� ��������, ����� ��� ����������� //?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//������������� ��������� ������ �� �������� ��� �������� ������������ ��� ���������� ��������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	/////////
	VAO fullScreenVAO;
	VBO fullScreenVBO1(verticesHalf1, sizeof(verticesHalf1));
	VBO fullScreenVBO2(verticesHalf2, sizeof(verticesHalf2));
	////////
	//�������� � ���������� ��������

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

	//�������� � �������� ������� � ������, ��� ���������� ������ ������ ����� ������������� �� ���������� ������������ � ������� � ����� ���������� �� 1 ������� �� ������
	//��������� ������� ��� ��������, �����������, ��������������� �������
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
	//������, ������, ���������� ������� �����������
	int widthIm, heightIm, nrChannels;
	float rotationAngle = 0.0f;
	//�������� �����������
	unsigned char* data = stbi_load("brick.png", &widthIm, &heightIm, &nrChannels, 0);
	GLuint textureCube;

	if (data) {
		glGenTextures(1, &textureCube);
		glBindTexture(GL_TEXTURE_2D, textureCube);

		//���������� �� �����������
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//���������� �� ���������
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//�������
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (nrChannels == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthIm, heightIm, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else if (nrChannels == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthIm, heightIm, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		//��������� �������� ��� ��������
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load the texture" << std::endl;
	}
	//������������ ������
	stbi_image_free(data);

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

	std::vector<cv::Vec3d> rvecs, tvecs;
	//while (!glfwWindowShouldClose(window1) && !glfwWindowShouldClose(window2))
	while (!glfwWindowShouldClose(window1))
	{
		//������ ������ �����
		cap >> frame;
		if (frame.empty()) {
			break;
		}

		//glfwMakeContextCurrent(window1);
		//glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

		//VAO1.Bind();
		//VBO1.Bind();
		//VAO1.BindEBO(EBO1);

		//VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
		//VAO1.LinkAttrib(VBO1, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		glEnable(GL_DEPTH_TEST);

		//��������� ��������� ���������
		//���������� ��������
		glBindTexture(GL_TEXTURE_2D, texture1);
		//������ ������ ������� �������� ������� �� ������ �����
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, frame.data);

		//������� �������, ���� ������ 
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glfwGetFramebufferSize(window1, &width, &height);
		glViewport(0, 0, width, height);

		//�������� ������� ��� ������� ����������� ��� �� ���������� � ������������
		//ShaderHelper::PassMatrix(glm::value_ptr(model1), locationModel);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//ShaderHelper::PassMatrix(glm::value_ptr(model2), locationModel);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//VAO1.Unbind();
		//VBO1.Unbind();
		//VAO1.UnbindEBO(EBO1);

		try {
			
			cv::aruco::detectMarkers(frame, dictionary, corners, ids);

			if (ids.size() > 0) {
				// ������ ���� ��������
				cv::aruco::estimatePoseSingleMarkers(corners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);
				for (int i = 0; i < ids.size(); i++) {
					// ��������� ���������� ��� ���������� ������ (��������, ����� ������� ���� �������)
					cv::Point2f textPosition = corners[i][0];
					// ����������� ID ������� � ������
					std::string idStr = std::to_string(ids[i]);
					// ������ ID �� �����������
					cv::putText(frame, idStr, textPosition, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 2);
					// ������ ��� �� �������
					drawAxis(frame, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1); // 0.05 - ��� ����� ���
					cv::imwrite("debug_image.jpg", frame);

				}

				glBindTexture(GL_TEXTURE_2D, texture1);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, frame.data);

				shaderProgram1.Activate();
				ShaderHelper::PassMatrix(glm::value_ptr(staticCameraMatrix), locationModel);
				fullScreenVAO.Bind();
				fullScreenVBO1.Bind();

				for (int i = 0; i < ids.size(); i++) {			
					glm::mat4 markerModel = convertRodriguesToMat4(rvecs[i], tvecs[i]);
					//markerModel = glm::rotate(markerModel, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					markerModel = glm::rotate(markerModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					markerModel = glm::scale(markerModel, glm::vec3(5.0f, 5.0f, 5.0f));
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

		glfwSwapBuffers(window1);
		glfwPollEvents();

	}

	//����������� �������
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