#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <iostream>
#include <vector>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

class Calibration
{
public:
    bool saveCameraParams(const std::string& filename, cv::Size imageSize, float aspectRatio,
        int flags, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
        double totalAvgErr);
    bool readCameraParameters(std::string filename, cv::Mat& camMatrix, cv::Mat& distCoeffs);
    int calibration();
};



#endif // !CALIBRATION_H


