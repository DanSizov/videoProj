#include "DrawClass.h"

void DrawClass::drawAxis(const cv::Mat& image, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs, const cv::Vec3d& rvec, const cv::Vec3d& tvec, const float& length) {
	std::vector<cv::Point3f> axisPoints;
	axisPoints.push_back(cv::Point3f(0, 0, 0)); // Начало осей
	axisPoints.push_back(cv::Point3f(length, 0, 0)); // Конец оси X
	axisPoints.push_back(cv::Point3f(0, length, 0)); // Конец оси Y
	axisPoints.push_back(cv::Point3f(0, 0, length)); // Конец оси Z

	float arrowLength = length / 7;  // Уменьшим размер стрелки
	float angle = 30.0;  // Угол стрелки в градусах
	float angleRad = angle * CV_PI / 180.0;  // Угол в радианах
	float dx = arrowLength * sin(angleRad);  // Разница в координатах по x и y для стрелки
	float dy = arrowLength * cos(angleRad);

	axisPoints.push_back(cv::Point3f(length - dy, dx, 0)); // Стрелка для X
	axisPoints.push_back(cv::Point3f(length - dy, -dx, 0)); // Стрелка для X
	axisPoints.push_back(cv::Point3f(dx, length - dy, 0)); // Стрелка для Y
	axisPoints.push_back(cv::Point3f(-dx, length - dy, 0)); // Стрелка для Y
	axisPoints.push_back(cv::Point3f(0, dx, length - dy)); // Стрелка для Z
	axisPoints.push_back(cv::Point3f(0, -dx, length - dy)); // Стрелка для Z

	std::vector<cv::Point2f> imagePoints;
	cv::projectPoints(axisPoints, rvec, tvec, cameraMatrix, distCoeffs, imagePoints);

	// Рисование осей
	cv::line(image, imagePoints[0], imagePoints[1], cv::Scalar(0, 0, 255), 3); // X axis in red
	cv::line(image, imagePoints[0], imagePoints[2], cv::Scalar(0, 255, 0), 3); // Y axis in green
	cv::line(image, imagePoints[0], imagePoints[3], cv::Scalar(255, 0, 0), 3); // Z axis in blue

	// Рисование стрелок
	cv::line(image, imagePoints[1], imagePoints[4], cv::Scalar(0, 0, 255), 3);
	cv::line(image, imagePoints[1], imagePoints[5], cv::Scalar(0, 0, 255), 3);
	cv::line(image, imagePoints[2], imagePoints[6], cv::Scalar(0, 255, 0), 3);
	cv::line(image, imagePoints[2], imagePoints[7], cv::Scalar(0, 255, 0), 3);
	cv::line(image, imagePoints[3], imagePoints[8], cv::Scalar(255, 0, 0), 3);
	cv::line(image, imagePoints[3], imagePoints[9], cv::Scalar(255, 0, 0), 3);

	int fontFace = cv::FONT_HERSHEY_SIMPLEX;
	double fontScale = 0.5;
	int thickness = 2;

	// Подписи осей
	cv::putText(image, "X", imagePoints[1], fontFace, fontScale, cv::Scalar(0, 0, 255), thickness);
	cv::putText(image, "Y", imagePoints[2], fontFace, fontScale, cv::Scalar(0, 255, 0), thickness);
	cv::putText(image, "Z", imagePoints[3], fontFace, fontScale, cv::Scalar(255, 0, 0), thickness);
}

void DrawClass::drawCube(const cv::Mat& image, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs, const cv::Vec3d& rvec, const cv::Vec3d& tvec, const float& sideLength) {
	std::vector<cv::Point3f> cubePoints;
	float halfLength = sideLength / 2.0f;
	cubePoints.push_back(cv::Point3f(-halfLength, halfLength, 0));  // 0
	cubePoints.push_back(cv::Point3f(halfLength, halfLength, 0));   // 1
	cubePoints.push_back(cv::Point3f(halfLength, -halfLength, 0));  // 2
	cubePoints.push_back(cv::Point3f(-halfLength, -halfLength, 0)); // 3
	cubePoints.push_back(cv::Point3f(-halfLength, halfLength, sideLength));  // 4
	cubePoints.push_back(cv::Point3f(halfLength, halfLength, sideLength));   // 5
	cubePoints.push_back(cv::Point3f(halfLength, -halfLength, sideLength));  // 6
	cubePoints.push_back(cv::Point3f(-halfLength, -halfLength, sideLength)); // 7

	std::vector<cv::Point2f> imagePoints;
	cv::projectPoints(cubePoints, rvec, tvec, cameraMatrix, distCoeffs, imagePoints);

	// Рисование граней куба
	cv::line(image, imagePoints[0], imagePoints[1], cv::Scalar(255, 0, 0), 2);
	cv::line(image, imagePoints[1], imagePoints[2], cv::Scalar(255, 0, 0), 2);
	cv::line(image, imagePoints[2], imagePoints[3], cv::Scalar(255, 0, 0), 2);
	cv::line(image, imagePoints[3], imagePoints[0], cv::Scalar(255, 0, 0), 2);
	cv::line(image, imagePoints[4], imagePoints[5], cv::Scalar(255, 0, 0), 2);
	cv::line(image, imagePoints[5], imagePoints[6], cv::Scalar(255, 0, 0), 2);
	cv::line(image, imagePoints[6], imagePoints[7], cv::Scalar(255, 0, 0), 2);
	cv::line(image, imagePoints[7], imagePoints[4], cv::Scalar(255, 0, 0), 2);
	cv::line(image, imagePoints[0], imagePoints[4], cv::Scalar(255, 0, 0), 2);
	cv::line(image, imagePoints[1], imagePoints[5], cv::Scalar(255, 0, 0), 2);
	cv::line(image, imagePoints[2], imagePoints[6], cv::Scalar(255, 0, 0), 2);
	cv::line(image, imagePoints[3], imagePoints[7], cv::Scalar(255, 0, 0), 2);
}