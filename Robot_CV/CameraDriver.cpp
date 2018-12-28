#include "PoseEstimation.h"

cv::VideoCapture		capture;

cv::Mat			view;

// Load camera parameters: Camera Matrix and Distortion Coefficients:
void LoadCameraParams(cv::Mat& camera_matrix, cv::Mat& dist_coeffs, cv::Size calibratedImageSize)
{
	readCameraMatrix(cameraParamFileName, camera_matrix, dist_coeffs, calibratedImageSize);
}



bool readCameraMatrix(const string& filename,
	cv::Mat& cameraMatrix, cv::Mat& distCoeffs,
	cv::Size& calibratedImageSize)
{
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	fs["image_width"] >> calibratedImageSize.width;
	fs["image_height"] >> calibratedImageSize.height;
	fs["distortion_coefficients"] >> distCoeffs;
	fs["camera_matrix"] >> cameraMatrix;

	if (distCoeffs.type() != CV_64F)
		distCoeffs = cv::Mat_<double>(distCoeffs);
	if (cameraMatrix.type() != CV_64F)
		cameraMatrix = cv::Mat_<double>(cameraMatrix);

	return true;
}

// Initiate camera capture and video-frame grabbing:
void GetCamera()
{
	capture.open(1);

}

void GetCamera(int id)
{
	capture.open(id);

}

// Acquire input image:
cv::Mat GetImage()

{
	capture >> view;

	return	view;
}



// Release camera capture and stop video-frame grabbing:
void ReleaseCamera()
{
	capture.release();
}
