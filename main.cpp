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

#define GL_CALL(x) x; GLDebug::CheckError(#x);

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
// constant
glm::mat4 INVERSE_MATRIX (
	1.0, 0.0, 0.0, 0.0,
	0.0, -1.0, 0.0, 0.0,
	0.0, 0.0, -1.0, 0.0,
	0.0, 0.0, 0.0, 1.0
);

glm::mat4 convertRodriguesToMat4(const cv::Vec3d& rvec, const cv::Vec3d& tvec) {
	//преобразование Rodrigues в матрицу вращения
	cv::Mat rotationMatrix;
	//преобразование вектора Родригеса в матрицу вращения
	cv::Rodrigues(rvec, rotationMatrix);

	cv::Mat model = cv::Mat::eye(4, 4, CV_64F);
	rotationMatrix.copyTo(model(cv::Rect(0, 0, 3, 3)));

	model.at<double>(0, 3) = tvec[0];
	model.at<double>(1, 3) = tvec[1];
	model.at<double>(2, 3) = tvec[2];

	glm::mat4 glmModel = glm::make_mat4x4(model.ptr<double>());
	glmModel = glmModel * INVERSE_MATRIX;
	glmModel = glm::transpose(glmModel);


	return glmModel;
}

std::unique_ptr<cv::Mat> objPoints;

std::unique_ptr<cv::Mat> initializeObjPoints(const float& markerLength) {
	auto objPoints = std::make_unique<cv::Mat>(4, 1, CV_32FC3);
	// Set coordinate system
	objPoints->ptr<cv::Vec3f>(0)[0] = cv::Vec3f(-markerLength / 2.f, markerLength / 2.f, 0);
	objPoints->ptr<cv::Vec3f>(0)[1] = cv::Vec3f(markerLength / 2.f, markerLength / 2.f, 0);
	objPoints->ptr<cv::Vec3f>(0)[2] = cv::Vec3f(markerLength / 2.f, -markerLength / 2.f, 0);
	objPoints->ptr<cv::Vec3f>(0)[3] = cv::Vec3f(-markerLength / 2.f, -markerLength / 2.f, 0);
	return objPoints;
}

struct Markers
{
	std::vector<int> ids;
	std::vector<std::vector<cv::Point2f>> corners, rejected;
	std::vector<cv::Vec3d> rvecs;
	std::vector<cv::Vec3d> tvecs;
	std::vector<glm::mat4> viewMatrixes;
};

float length{ 1.0f };

void processMarkersAndDrawCubes(cv::Mat& frame, ArucoMarkerManager& arucoManager, GLuint& textureCube, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs, VAO& VAO, const Shader& shaderProgram, Markers& markers) {
	cv::aruco::detectMarkers(frame, arucoManager.getDictionary(), markers.corners, markers.ids);

	if (markers.ids.size() > 0) {
		objPoints = initializeObjPoints(length);

		size_t nMarkers = markers.corners.size();
		markers.rvecs.resize(nMarkers);
		markers.tvecs.resize(nMarkers);
		markers.viewMatrixes.resize(nMarkers);

		for (int i = 0; i < markers.ids.size(); i++) {
			solvePnP(*objPoints, markers.corners[i], cameraMatrix, distCoeffs, markers.rvecs[i], markers.tvecs[i]);
			markers.viewMatrixes.at(i) = convertRodriguesToMat4(markers.rvecs.at(i), markers.tvecs.at(i));

			cv::Point2f textPosition = markers.corners[i][0];
			std::string idStr = std::to_string(markers.ids[i]);
			cv::putText(frame, idStr, textPosition, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 2);
			arucoManager.drawAxis(frame, markers.rvecs[i], markers.tvecs[i], length);
			arucoManager.drawCube(frame, markers.rvecs[i], markers.tvecs[i], length);
			glm::mat4 markerModel = markers.viewMatrixes.at(i);
			GLint locationModelCube = ShaderHelper::GetLocation(shaderProgram.ID, "model");
			ShaderHelper::PassMatrix(glm::value_ptr(markerModel), locationModelCube);
			VAO.Bind();
			glBindTexture(GL_TEXTURE_2D, textureCube);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			VAO.Unbind();
		}
	}
}

void framebuffer_size_callback(GLFWwindow* window, int new_width, int new_height) {
	width = new_width;
	height = new_height;

	glViewport(0, 0, width, height);
}

int main() {


	cv::Mat cameraMatrix;
	cv::Mat distCoeffs;
	WindowManager window1(width, height, "window1", nullptr);
	glfwSetFramebufferSizeCallback(window1.GetWindow(), framebuffer_size_callback);

	window1.MakeContextCurrent();

	//открытие камеры с помощью opencv
	cv::VideoCapture cap(0);
	cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
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

	// Getting focal lenght from camera calibration parameters
	double focal_length_x = cameraMatrix.at<double>(0, 0);

	// Calculate Field of View in radians and degrees
	float FOV = 2.0 * std::atan2(0.5 * height, focal_length_x);
	float FOV_Deg = glm::degrees(FOV);

	ArucoMarkerManager arucoManager(cameraMatrix, distCoeffs, cv::aruco::DICT_6X6_250);
	TextureManager textureManager;
	GLuint texture1 = textureManager.LoadTextureFromFrame(frame);
	GLuint texture2 = textureManager.LoadTextureFromFrame(frame);
	VAO fullScreenVAO1;
	VBO fullScreenVBO1(fullScreenVertices, sizeof(fullScreenVertices));

	fullScreenVAO1.Bind();
	fullScreenVBO1.Bind();
	fullScreenVAO1.LinkAttrib(fullScreenVBO1, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
	fullScreenVAO1.LinkAttrib(fullScreenVBO1, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	fullScreenVAO1.Unbind();
	fullScreenVBO1.Unbind();

	VAO fullScreenVAO2;
	VBO fullScreenVBO2(fullScreenVertices, sizeof(fullScreenVertices));

	fullScreenVAO2.Bind();
	fullScreenVBO2.Bind();
	fullScreenVAO2.LinkAttrib(fullScreenVBO2, 0, 2, GL_FLOAT, 5 * sizeof(float), (void*)0);
	fullScreenVAO2.LinkAttrib(fullScreenVBO2, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	fullScreenVAO2.Unbind();
	fullScreenVBO2.Unbind();

	window1.MakeContextCurrent();

	Shader shaderProgram1("default.vert", "default.frag");
	shaderProgram1.Activate();

	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 1.0f));


	camera.Matrix(shaderProgram1, "camMatrix");

	glm::mat4 model1 = glm::mat4(1.0f);
	model1 = glm::translate(model1, glm::vec3(0.0f, 0.0f, 1.0f));
	model1 = glm::rotate(model1, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	auto locationModel = glGetUniformLocation(shaderProgram1.ID, "model");
	ShaderHelper::PassMatrix(glm::value_ptr(model1), locationModel);

	//cube
	GLuint textureCube = textureManager.LoadTextureFromFile("brick.png");

	float rotationAngle = 0.0f;

	VAO VAO2;
	VBO VBO2(cubeVertices, sizeof(cubeVertices));
	//EBO EBO2(indicesCube, sizeof(indicesCube));

	VAO2.Bind();
	VBO2.Bind();
	//VAO2.BindEBO(EBO2);

	VAO2.LinkAttrib(VBO2, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
	VAO2.LinkAttrib(VBO2, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));


	glEnable(GL_DEPTH_TEST);

	glm::mat4 staticCameraMatrix = glm::mat4(1.0f);

	double lastTime = glfwGetTime();
	int nbFrames = 0;
	Markers markers;

	cv::Mat map1, map2;

	cv::initUndistortRectifyMap(
		cameraMatrix,  // матрица камеры
		distCoeffs,    // коэффициенты искажения
		cv::Mat(),     // дополнительные параметры
		cameraMatrix,  // новая матрица камеры, обычно идентична исходной
		frame.size(),  // размер исправленного изображения
		CV_16SC2,      // тип первой выходной карты
		map1,          // первая выходная карта
		map2           // вторая выходная карта
	);
	while (!window1.Close())
	{

		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) {
			std::cout << "FPS: " << nbFrames << std::endl;
			nbFrames = 0;
			lastTime += 1.0;
		}

		cap >> frame;
		if (frame.empty()) {
			break;
		}
		cv::Mat undistortedFrame;
		cv::remap(frame, undistortedFrame, map1, map2, cv::INTER_LINEAR);

		window1.MakeContextCurrent();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		glm::mat4 originalCamMatrix = camera.getMatrix();
		camera.resetMatrix();
		camera.Matrix(shaderProgram1, "camMatrix");

		shaderProgram1.Activate();
		ShaderHelper::PassMatrix(glm::value_ptr(staticCameraMatrix), locationModel);

		glViewport(0, 0, width, height);

		fullScreenVAO1.Bind();
		fullScreenVBO1.Bind();
		glBindTexture(GL_TEXTURE_2D, texture1);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		fullScreenVAO1.Unbind();
		fullScreenVBO1.Unbind();

		camera.setMatrix(originalCamMatrix);
		camera.Matrix(shaderProgram1, "camMatrix");
		glClear(GL_DEPTH_BUFFER_BIT);

		camera.Inputs(window1.GetWindow());
		camera.updateMatrix(FOV_Deg, 0.1f, 100.0f);

		glEnable(GL_DEPTH_TEST);

		textureManager.UpdateTexture(texture1, undistortedFrame);
		textureManager.UpdateTexture(texture2, undistortedFrame);

		glfwGetFramebufferSize(window1.GetWindow(), &width, &height);

		processMarkersAndDrawCubes(undistortedFrame, arucoManager, textureCube, cameraMatrix, distCoeffs, VAO2,  shaderProgram1, markers);
		textureManager.UpdateTexture(texture1, undistortedFrame);
		textureManager.UpdateTexture(texture2, undistortedFrame);

		window1.SwapBuffers();
		window1.PollEvents();

	}

	VAO2.Delete();
	VBO2.Delete();
	//EBO2.Delete();

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