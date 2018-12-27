/***********************************************************************************************************************************
Copyright (c) 2014, Giovanni Sutanto
You can contact the author at <giovanni dot sutanto at gmail dot com>
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* Neither the name of the author nor the names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE AUTHOR/CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
************************************************************************************************************************************
*/

#pragma once

#ifndef POSE_ESIMATION_H
#define POSE_ESIMATION_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/videoio.hpp>
#include "CameraDriver.h"
#include "ImagingSource.h"

using namespace std;


enum MeasurementMode {
	CENTIMETERS,
	MILLIMETERS,
	INCHES,
};

#define THETAARMLENGTH 177.8f
#define CAMERA_X_TRANSLATION 18;
#define CAMERA_Y_TRANSLATION -18.5;
#define CAMERA_Z_TRANSLATION 33.02;

	// TCP/IP Port to use: 50058
	// Functions and Procedures defined in CameraDriver.cpp:
	//string GetDevice();
	//void LoadCameraParams(string device, Mat& camera_matrix, Mat& dist_coeffs, Size calibratedImageSize);
	//void GetCamera();
	//Mat GetImage();
	//void ReleaseCamera();
	//bool readCameraMatrix(const string& filename, Mat& cameraMatrix, Mat& distCoeffs, Size& calibratedImageSize);

	// Functions and Procedures defined in PoseEstimation.cpp:
	cv::Mat3b getMean(const vector<cv::Mat3b>& images);
	void SetRobotCoordinates(cv::Mat robotPosition);
	void CalcBoardCornerPositions(vector<cv::Point3f>& corner_object_points, cv::Size board_size = cv::Size(7, 4), float square_size = 2.0f/*(in millimeter)*/);
	void UndistortImage(cv::Mat& in_img, cv::Mat camera_matrix, cv::Mat dist_coeffs);
	bool GetPose(cv::Mat robotMat, cv::Mat& undist_in_img, cv::Mat& transfvec12, cv::Mat& rvec, cv::Mat& tvec, vector<cv::Point3f> corner_object_points/*(3D object points)*/, cv::Mat camera_matrix, cv::Mat dist_coeffs, bool print = true, cv::Size board_size = cv::Size(7, 4));
	void DrawObjCoordFrame(cv::Mat& undist_in_img, cv::Mat rvec, cv::Mat tvec, cv::Mat camera_matrix, cv::Mat dist_coeffs);
	int FindTarget(ImagingSource& mImagingSource, cv::Mat& transfvec12, cv::Mat& img, bool print, bool show_img, MeasurementMode measUnits);

#endif