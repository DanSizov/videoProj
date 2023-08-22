#ifndef DRAW_CLASS_H
#define DRAW_CLASS_H

#include<opencv2/opencv.hpp>


class DrawClass
{
public:
	static void drawAxis(const cv::Mat& image, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs, const cv::Vec3d& rvec, const cv::Vec3d& tvec, const float& length);
	static void drawCube(const cv::Mat& image, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs, const cv::Vec3d& rvec, const cv::Vec3d& tvec, const float& sideLength);
};

#endif // !DRAW_CLASS_H


