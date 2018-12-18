#include "PoseEstimation.h"

cv::VideoCapture		capture;

cv::Mat			view;

string GetDevice()
{

	string			device = "";

	return device;

}



// Load camera parameters: Camera Matrix and Distortion Coefficients:
void LoadCameraParams(string device, cv::Mat& camera_matrix, cv::Mat& dist_coeffs, cv::Size calibratedImageSize)
{

	if (device == "Laptop")

	{

		// Giovanni's ThinkPad T430s Laptop Webcam's Camera Matrix and Distortion Coefficients (results of performing camera calibration):

		camera_matrix = (cv::Mat_<double>(3, 3) << 6.4607779344117296e+002, 0.0, 3.1950000000000000e+002,

			0.0, 6.4611590221166568e+002, 2.3950000000000000e+002,

			0.0, 0.0, 1.0);

		dist_coeffs = (cv::Mat_<double>(5, 1) << -1.2568930877482700e-002,

			-6.5894418731935356e-002,

			-2.1086106689000052e-003,

			-1.1323933275534813e-003,

			-6.6066720498874881e-002);

	}
	else
	{
		readCameraMatrix(cameraParamFileName, camera_matrix, dist_coeffs, calibratedImageSize);
	}
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
	capture.open(2);

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
