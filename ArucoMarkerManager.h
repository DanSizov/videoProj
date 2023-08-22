#ifndef ARUCO_MARKER_MANAGER_H
#define ARUCO_MARKER_MANAGER_H

#include<opencv2/opencv.hpp>
#include<opencv2/aruco.hpp>
#include<iostream>

class ArucoMarkerManager
{
private:
	cv::Ptr<cv::aruco::Dictionary> dictionary;
	cv::Mat cameraMatrix;
	cv::Mat distCoeffs;

public:
	ArucoMarkerManager(const cv::Mat& camMatrix, const cv::Mat& distCoeff,const int& dictionaryId);
	void detectAndEstimatePose(const cv::Mat& frame, std::vector<int>& ids, std::vector<std::vector<cv::Point2f>>& corners, const double& length, std::vector<cv::Vec3d>& rvecs, std::vector<cv::Vec3d>& tvecs);
	void drawDetectedMarkers(cv::Mat& frame, const std::vector<std::vector<cv::Point2f>>& corners, const std::vector<int>& ids);
	void drawAxis(const cv::Mat& image, const cv::Vec3d& rvec, const cv::Vec3d& tvec, const float& length);
	void drawCube(const cv::Mat& image, const cv::Vec3d& rvec, const cv::Vec3d& tvec, const float& sideLength);
	cv::Ptr<cv::aruco::Dictionary> getDictionary() const;
};


#endif // !ARUCO_MARKER_MANAGER_H

