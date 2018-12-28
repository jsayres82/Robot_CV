#pragma once


#ifndef CAMERA_DRIVER_H
#define CAMERA_DRIVER_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>

//using namespace cv;
using namespace std;

//const cv::String cameraParamFileName = "configuration/camera.yml";

const cv::String cameraParamFileName = "configuration/out_camera_data.xml";

// TCP/IP Port to use: 50058

// Functions and Procedures defined in CameraDriver.cpp:
void LoadCameraParams(cv::Mat& camera_matrix, cv::Mat& dist_coeffs, cv::Size calibratedImageSize);
void GetCamera();
void GetCamera(int id);
cv::Mat GetImage();
void ReleaseCamera();
bool readCameraMatrix(const string& filename, cv::Mat& cameraMatrix, cv::Mat& distCoeffs, cv::Size& calibratedImageSize);

#endif
