
#include "Calibration.h"

using namespace std;
using namespace cv;

const double FRAME_WIDTH = 640.0f;
const double FRAME_HEIGHT = 480.0f;

namespace {
    const char* about =
        "Calibration using a ChArUco board\n"
        "  To capture a frame for calibration, press 'c',\n"
        "  If input comes from video, press any key for next frame\n"
        "  To finish capturing, press 'ESC' key and calibration starts.\n";
    const char* keys =
        "{w        |       | Number of squares in X direction }"
        "{h        |       | Number of squares in Y direction }"
        "{sl       |       | Square side length (in meters) }"
        "{ml       |       | Marker side length (in meters) }"
        "{d        |       | dictionary: DICT_4X4_50=0, DICT_4X4_100=1, DICT_4X4_250=2,"
        "DICT_4X4_1000=3, DICT_5X5_50=4, DICT_5X5_100=5, DICT_5X5_250=6, DICT_5X5_1000=7, "
        "DICT_6X6_50=8, DICT_6X6_100=9, DICT_6X6_250=10, DICT_6X6_1000=11, DICT_7X7_50=12,"
        "DICT_7X7_100=13, DICT_7X7_250=14, DICT_7X7_1000=15, DICT_ARUCO_ORIGINAL = 16}"
        "{cd       |       | Input file with custom dictionary }"
        "{@outfile |<none> | Output file with calibrated camera parameters }"
        "{v        |       | Input from video file, if ommited, input comes from camera }"
        "{ci       | 0     | Camera id if input doesnt come from video (-v) }"
        "{dp       |       | File of marker detector parameters }"
        "{rs       | false | Apply refind strategy }"
        "{zt       | false | Assume zero tangential distortion }"
        "{a        |       | Fix aspect ratio (fx/fy) to this value }"
        "{pc       | false | Fix the principal point at the center }"
        "{sc       | false | Show detected chessboard corners after calibration }";
}

bool Calibration::saveCameraParams(const std::string& filename, cv::Size imageSize, float aspectRatio,
    int flags, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
    double totalAvgErr)
{
    cv::FileStorage fs(filename, cv::FileStorage::WRITE);
    if (!fs.isOpened())
        return false;

    fs << "image_width" << imageSize.width;
    fs << "image_height" << imageSize.height;
    fs << "aspectRatio" << aspectRatio;
    fs << "calibration_flags" << flags;
    fs << "camera_matrix" << cameraMatrix;
    fs << "distortion_coefficients" << distCoeffs;
    fs << "avg_reprojection_error" << totalAvgErr;

    return true;
}

bool Calibration::readCameraParameters(std::string filename, cv::Mat& camMatrix, cv::Mat& distCoeffs) {
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if (!fs.isOpened())
        return false;
    fs["camera_matrix"] >> camMatrix;
    fs["distortion_coefficients"] >> distCoeffs;
    return true;
}

int Calibration::calibration() {

    int squaresX = 8;
    int squaresY = 8;
    float squareLength = 0.015f;
    float markerLength = 0.011f;
    string outputFile = "camera_parameters_2.yml";

    bool showChessboardCorners = true;

    int calibrationFlags = 0;
    float aspectRatio = 1;

    aruco::DetectorParameters detectorParams = aruco::DetectorParameters();


    bool refindStrategy = true;
    int camId = 0;
    String video;

    VideoCapture inputVideo;
    int waitTime;
    if (!video.empty()) {
        inputVideo.open(video);
        waitTime = 0;
    }
    else {
        inputVideo.open(camId);
        waitTime = 10;
    }

    inputVideo.set(cv::CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
    inputVideo.set(cv::CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

    auto frameWidth = static_cast<int>(inputVideo.get(cv::CAP_PROP_FRAME_WIDTH));
    auto frameHeight = static_cast<int>(inputVideo.get(cv::CAP_PROP_FRAME_HEIGHT));

    std::cout << "Frame size = " << frameWidth << " x " << frameHeight << endl;

    aruco::Dictionary dictionary = aruco::getPredefinedDictionary(0);
    int dictionaryId = 10;
    dictionary = aruco::getPredefinedDictionary(aruco::PredefinedDictionaryType(dictionaryId));

    // Create charuco board object
    aruco::CharucoBoard board(Size(squaresX, squaresY), squareLength, markerLength, dictionary);
    aruco::CharucoParameters charucoParams;

    if (refindStrategy) {
        charucoParams.tryRefineMarkers = true;
    }

    aruco::CharucoDetector detector(board, charucoParams, detectorParams);

    // Collect data from each frame
    vector<Mat> allCharucoCorners;
    vector<Mat> allCharucoIds;

    vector<vector<Point2f>> allImagePoints;
    vector<vector<Point3f>> allObjectPoints;

    vector<Mat> allImages;
    Size imageSize;

    while (inputVideo.grab()) {
        Mat image, imageCopy;
        inputVideo.retrieve(image);

        vector<int> markerIds;
        vector<vector<Point2f>> markerCorners, rejectedMarkers;
        Mat currentCharucoCorners;
        Mat currentCharucoIds;
        vector<Point3f> currentObjectPoints;
        vector<Point2f> currentImagePoints;

        // Detect ChArUco board
        detector.detectBoard(image, currentCharucoCorners, currentCharucoIds);

        // Draw results
        image.copyTo(imageCopy);
        if (!markerIds.empty()) {
            aruco::drawDetectedMarkers(imageCopy, markerCorners);
        }

        if (currentCharucoCorners.total() > 3) {
            aruco::drawDetectedCornersCharuco(imageCopy, currentCharucoCorners, currentCharucoIds);
        }

        cv::putText(imageCopy, "Press 'c' to add current frame. 'ESC' to finish and calibrate",
            Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 240), 1);

        cv::putText(imageCopy, "Frame size = " + to_string(frameWidth) + " x " + to_string(frameHeight),
            Point(10, 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 240, 0), 1);

        cv::imshow("out", imageCopy);

        // Wait for key pressed
        char key = (char)waitKey(waitTime);

        if (key == 27) {
            break;
        }

        if (key == 'c' && currentCharucoCorners.total() > 3) {
            // Match image points
            board.matchImagePoints(currentCharucoCorners, currentCharucoIds, currentObjectPoints, currentImagePoints);

            if (currentImagePoints.empty() || currentObjectPoints.empty()) {
                std::cout << "Point matching failed, try again." << endl;
                continue;
            }

            std::cout << "Frame captured" << endl;

            allCharucoCorners.push_back(currentCharucoCorners);
            allCharucoIds.push_back(currentCharucoIds);
            allImagePoints.push_back(currentImagePoints);
            allObjectPoints.push_back(currentObjectPoints);
            allImages.push_back(image);

            imageSize = image.size();
        }
    }

    if (allCharucoCorners.size() < 4) {
        cerr << "Not enough corners for calibration" << endl;
        return 0;
    }

    Mat cameraMatrix, distCoeffs;

    if (calibrationFlags & CALIB_FIX_ASPECT_RATIO) {
        cameraMatrix = Mat::eye(3, 3, CV_64F);
        cameraMatrix.at<double>(0, 0) = aspectRatio;
    }

    // Calibrate camera using ChArUco
    double repError = calibrateCamera(
        allObjectPoints, allImagePoints, imageSize,
        cameraMatrix, distCoeffs, noArray(), noArray(), noArray(),
        noArray(), noArray(), calibrationFlags
    );

    bool saveOk = saveCameraParams(
        outputFile, imageSize, aspectRatio, calibrationFlags,
        cameraMatrix, distCoeffs, repError
    );

    if (!saveOk) {
        cerr << "Cannot save output file" << endl;
        return 0;
    }

    std::cout << "Rep Error: " << repError << endl;
    std::cout << "Calibration saved to " << outputFile << endl;

    // Show interpolated charuco corners for debugging
    if (showChessboardCorners) {
        for (size_t frame = 0; frame < allImages.size(); frame++) {
            Mat imageCopy = allImages[frame].clone();

            if (allCharucoCorners[frame].total() > 0) {
                aruco::drawDetectedCornersCharuco(
                    imageCopy, allCharucoCorners[frame], allCharucoIds[frame]
                );
            }

            cv::imshow("out", imageCopy);
            char key = (char)waitKey(0);
            if (key == 27) {
                break;
            }
        }
    }

    return 0;
}
