#pragma once

#ifndef CAMERA_CALIBRATION_H
#define CAMERA_CALIBRATION_H

#include "CameraDriver.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <time.h>
#include <iostream>

	class CameraSettings
	{
	public:
		CameraSettings();

		enum Pattern { NOT_EXISTING, CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };
		enum InputType { INVALID, CAMERA, VIDEO_FILE, IMAGE_LIST };

		void write(cv::FileStorage& fs)const;
		void read(const cv::FileNode& node);
		void validate();
		cv::Mat nextImage();
		static bool readStringList(const std::string& filename, std::vector<std::string>& l);
		static bool isListOfImages(const std::string& filename);

	public:
		std::string cameraModel;
		std::string cameraSerial;
		cv::Size boardSize;              // The size of the board -> Number of items by width and height
		Pattern calibrationPattern;  // One of the Chessboard, circles, or asymmetric circle pattern
		float squareSize;            // The size of a square in your defined unit (point, millimeter,etc).
		int nrFrames;                // The number of frames to use from the input for calibration
		float aspectRatio;           // The aspect ratio
		int delay;                   // In case of a video input
		bool writePoints;            // Write detected feature points
		bool writeExtrinsics;        // Write extrinsic parameters
		bool calibZeroTangentDist;   // Assume zero tangential distortion
		bool calibFixPrincipalPoint; // Fix the principal point at the center
		bool flipVertical;           // Flip the captured images around the horizontal axis
		std::string outputFileName;       // The name of the file where to write
		bool showUndistorsed;        // Show undistorted images after calibration
		std::string input;                // The input ->
		bool useFisheye;             // use fisheye camera model for calibration
		bool fixK1;                  // fix K1 distortion coefficient
		bool fixK2;                  // fix K2 distortion coefficient
		bool fixK3;                  // fix K3 distortion coefficient
		bool fixK4;                  // fix K4 distortion coefficient
		bool fixK5;                  // fix K5 distortion coefficient

		int cameraID;
		std::vector<std::string> imageList;
		size_t atImageList;
		cv::VideoCapture inputCapture;
		InputType inputType;
		bool goodInput;
		int flag;
	private:
		std::string patternToUse;

	};

	enum { DETECTION = 0, CAPTURING = 1, CALIBRATED = 2 };
	//enum pattern { chessboard, circles_grid, asymmetric_circles_grid };
	
	bool CalibrateCamera(const std::string inputFile, std::string cameraName, std::string serial);

	double ComputeReprojectionErrors(const std::vector<std::vector<cv::Point3f> >& objectPoints,
		const std::vector<std::vector<cv::Point2f> >& imagePoints,
		const std::vector<cv::Mat>& rvecs, const std::vector<cv::Mat>& tvecs,
		const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
		std::vector<float>& perViewErrors, bool fisheye);

	void CalcBoardCornerPositions(cv::Size boardSize, float squareSize, std::vector<cv::Point3f>& corners,
		CameraSettings::Pattern patternType /*= Settings::CHESSBOARD*/);
	
	bool RunCalibration(CameraSettings& s, cv::Size& imageSize, cv::Mat& cameraMatrix, cv::Mat& distCoeffs,
		std::vector<std::vector<cv::Point2f> > imagePoints, std::vector<cv::Mat>& rvecs, std::vector<cv::Mat>& tvecs,
		std::vector<float>& reprojErrs, double& totalAvgErr);

	bool RunCalibrationAndSave(CameraSettings& s, cv::Size imageSize, cv::Mat& cameraMatrix, cv::Mat& distCoeffs,
		std::vector<std::vector<cv::Point2f> > imagePoints);
	//double computeReprojectionErrors(
	//	const std::vector<std::vector<cv::Point3f> >& objectPoints,
	//	const std::vector<std::vector<cv::Point2f> >& imagePoints,
	//	const std::vector<cv::Mat>& rvecs, const std::vector<cv::Mat>& tvecs,
	//	const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
	//	std::vector<float>& perViewErrors);


	//void calcChessboardCorners(cv::Size boardSize, float squareSize, std::vector<cv::Point3f>& corners, CameraSettings::Pattern patternType = CameraSettings::Pattern::CHESSBOARD);


	//bool runCalibration(std::vector<std::vector<cv::Point2f> > imagePoints,
	//	cv::Size imageSize, cv::Size boardSize, CameraSettings::Pattern patternType,
	//	float squareSize, float aspectRatio, float focalLength,
	//	int flags, cv::Mat& cameraMatrix, cv::Mat& distCoeffs,
	//	std::vector<cv::Mat>& rvecs, std::vector<cv::Mat>& tvecs,
	//	std::vector<float>& reprojErrs,
	//	double& totalAvgErr);


	//void saveCameraParams(const std::string& filename,
	//	cv::Size imageSize, cv::Size boardSize,
	//	float squareSize, float aspectRatio, int flags,
	//	const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
	//	const std::vector<cv::Mat>& rvecs, const std::vector<cv::Mat>& tvecs,
	//	const std::vector<float>& reprojErrs,
	//	const std::vector<std::vector<cv::Point2f>>& imagePoints,
	//	double totalAvgErr);

	//bool readStringList(const std::string& filename, std::vector<std::string>& l);


	//bool runAndSave(const std::string& outputFilename,
	//	const std::vector<std::vector<cv::Point2f> >& imagePoints,
	//	cv::Size imageSize, cv::Size boardSize, CameraSettings::Pattern patternType, float squareSize,
	//	float aspectRatio, float focalLength, int flags, cv::Mat& cameraMatrix,
	//	cv::Mat& distCoeffs, bool writeExtrinsics, bool writePoints);

	///// <summary>
	///// points in world coordinate
	///// </summary>
	///// <param name="corners3D">coordinate value</param>
	///// <param name="chessBoardSize">size of chessboard</param>
	///// <param name="nImages">number of images</param>
	///// <param name="squareSize">actual size of square</param>
	//void objectCorners3D(std::vector<cv::Point3f>& corners, cv::Size boardSize, float squareSize);

	void SaveCameraCalibrationParams(CameraSettings& s, cv::Size& imageSize, cv::Mat& cameraMatrix, cv::Mat& distCoeffs,
		const std::vector<cv::Mat>& rvecs, const std::vector<cv::Mat>& tvecs,
		const std::vector<float>& reprojErrs, const std::vector<std::vector<cv::Point2f> >& imagePoints,
		double totalAvgErr);


#endif