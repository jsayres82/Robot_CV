#include "PoseEstimation.h"

#define	PI	3.14159265358979323846

double unitConversion = 10;
static cv::Mat R_C_Matrix = cv::Mat_<double>(4, 4);

// TCP/IP Port to use: 50058
	void ConvertIntoHomogeneous(cv::Mat& matrix)
	{
		// Get Homogeneous (4X4) cv::Matrix from Non-Homogeneous (3X3) cv::Matrix:
		cv::hconcat(matrix, (cv::Mat)(cv::Mat_<double>(3, 1) << 0.0, 0.0, 0.0), matrix);
		cv::vconcat(matrix, (cv::Mat)(cv::Mat_<double>(1, 4) << 0.0, 0.0, 0.0, 1.0), matrix);
	}

	void ConvertIntoNonHomogeneous(cv::Mat& matrix)
	{
		// Get Non-Homogeneocvus (3X3) cv::Matrix from Homogeneous (4X4) cv::Matrix:
		matrix = matrix(cv::Rect(0, 0, 3, 3));
	}

	cv::Mat RotationMatrix(unsigned char axis, double angle)
	{
		// Get Homogeneous (4X4) Rotation cv::Matrix along specified axis, with specified angle:
		if (axis == 'x')
		{
			return	(cv::Mat_<double>(4, 4) << 
				1.0,	 0.0,			0.0,				0.0,
				0.0,	cos(angle),		-sin(angle),		0.0,
				0.0,	sin(angle),		cos(angle),			0.0,
				0.0,	 0.0,		0.0,				1.0);
		}
		else if (axis == 'y')
		{
			return	(cv::Mat_<double>(4, 4) << 
				cos(angle),		0.0,	 sin(angle), 0.0,
				0.0,			1.0,	 0.0,		 0.0,
				-sin(angle),	0.0,	cos(angle),	 0.0,
				0.0, 0.0, 0.0, 1.0);
		}
		else if (axis == 'z')
		{
			return	(cv::Mat_<double>(4, 4) << 
				cos(angle), -sin(angle), 0.0, 0.0,
				sin(angle), cos(angle), 0.0, 0.0,
				0.0, 0.0, 1.0,			0.0,
				0.0, 0.0, 0.0, 1.0);
		}
	}

	cv::Mat TranslationMatrix(double tx, double ty, double tz)
	{
		// Get Homogeneous (4X4) Translation cv::Matrix, with specified tx (translation along x-axis),
		// ty (translation along y-axis), and tz (translation along z-axis):
		return	(cv::Mat_<double>(4, 4) << 1.0, 0.0, 0.0, tx,
			0.0, 1.0, 0.0, ty,
			0.0, 0.0, 1.0, tz,
			0.0, 0.0, 0.0, 1.0);
	}

	void CalcBoardCornerPositions(vector<cv::Point3f>& corner_object_points, cv::Size board_size/*=Size(3, 6)*/, float square_size/*=2(in millimeter)*/)
	{
		corner_object_points.clear();

		for (int y = 0; y < board_size.height; ++y)
		{
			for (int x = 0; x < board_size.width; ++x)
			{
				corner_object_points.push_back(cv::Point3f(float(x*square_size), float(-y * square_size), 0));
			}
		}
	}

	void UndistortImage(cv::Mat& in_img, cv::Mat camera_matrix, cv::Mat dist_coeffs)
	{
		// Undistort input image:
		cv::Mat temp = in_img.clone();
		undistort(temp, in_img, camera_matrix, dist_coeffs);
	}

	bool GetPose(cv::Mat robotMat, cv::Mat& undist_in_img, cv::Mat& transfvec12, cv::Mat& rvec, cv::Mat& tvec, vector<cv::Point3f> corner_object_points/*(3D object points)*/, cv::Mat camera_matrix, cv::Mat dist_coeffs, bool print/*=true*/, cv::Size board_size/*=Size(3, 4)*/)
	{
		// Let's say we have 3D object point P, which is seen as 2D image point p on the image plane;
		// the point p is related to point P by applying a rotation cv::Matrix R and a translation vector t to P, or cv::Mathematically:
		// p = [R|t] * P
		// See the theoretical explanation here: http://docs.opencv.org/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html
		// Points:
		vector<cv::Point2f>	corner_image_points;			// 2D image points (on image plane)

		bool			found;

		// Rotation cv::Matrix:
		cv::Mat				rmat;

		// Overall Transformation cv::Matrices:

		cv::Mat				transfmatCam;					// Overall Transformation cv::Matrix (3X4) in reference to Camera Coordinate System
		cv::Mat				transfmatCamHomogeneous;		// Overall Homogeneous Transformation cv::Matrix (4X4) in reference to Camera Coordinate System
		cv::Mat				transfmatSBHomogeneous;			// Overall Homogeneous Transformation cv::Matrix (4X4) in reference to Max's End-Effector Coordinate System
		cv::Mat				transfmatSB;					// Overall Transformation cv::Matrix (3X4) in reference to Max's End-Effector Coordinate System
		cv::Mat				transfmatSB2;					// Overall Transformation cv::Matrix (3X4) in reference to Max's End-Effector Coordinate System

															// Images:
		cv::Mat				undist_in_img_gray;

		found = findChessboardCorners(undist_in_img, board_size, corner_image_points, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
		if (found)		// If done with success,
		{
			// improve the found corners' coordinate accuracy for chessboard:
			cvtColor(undist_in_img, undist_in_img_gray, CV_BGR2GRAY);
			cornerSubPix(undist_in_img_gray, corner_image_points, cv::Size(5,5), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

			// Draw the corners:
			drawChessboardCorners(undist_in_img, board_size, cv::Mat(corner_image_points), found);

			// Compute Rotation Vector (rvec) and Translation Vector (tvec) using solvePnPRansac algorithm:
			solvePnPRansac(corner_object_points, corner_image_points, camera_matrix, dist_coeffs, rvec, tvec);
			// Compute Rotation cv::Matrix (rmat, size 3-by-3 cv::Matrix) from Rotation Vector (rvec, size 3-by-1 cv::Matrix) using Rodrigues transform
			// (http://docs.opencv.org/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html#rodrigues):
			Rodrigues(rvec, rmat);
			// Rotate Target Chessboard Coordinate System 180 degrees along x-axis (Roll-direction)
			// (to align Target Chessboard Coordinate System with Camera Coordinate System):
			ConvertIntoHomogeneous(rmat);
			rmat = rmat * RotationMatrix('x', PI);
			ConvertIntoNonHomogeneous(rmat);
			// Update the Rotation Vector (rvec) after the change above:
			Rodrigues(rmat, rvec);
			//if (print)
			//{
			//	cout << "rvec = " << rvec << endl;
			//	cout << "rmat = " << rmat << endl;
			//	cout << "tvec = " << tvec << endl;
			//}
			// Combine Rotation cv::Matrix (rmat) and Translation Vector (tvec) into the overall transformation cv::Matrix
			// in reference to Camera coordinate system (transfmatCam, size 3-by-4 cv::Matrix), that is [rmat|tvec]:
			hconcat(rmat, tvec, transfmatCam);
			// Convert transfmatCam into transfmatSB (overall transformation cv::Matrix in reference 
			// to Max's gripper coordinate system):
			cv::Mat temp = (cv::Mat_<double>(1, 4) << 0.0, 0.0, 0.0, 1.0);
			vconcat(transfmatCam, temp, transfmatCamHomogeneous);
			cv::Mat Rz = RotationMatrix('z', PI);
			cv::Mat Rx = RotationMatrix('x', -(PI / 2));
			// Camera Offset from Max's calibtation tool End-Effector coordinate system's point of origin:
			//cv::Mat T1 = TranslationMatrix(CAMERA_X_TRANSLATION, CAMERA_Y_TRANSLATION, CAMERA_Z_TRANSLATION);
			cv::Mat T1 = TranslationMatrix(-18, -5, 18.5);
			transfmatSBHomogeneous = T1 * Rx *  transfmatCamHomogeneous;
			//transfmatSBHomogeneous = transfmatCamHomogeneous;// T1 * transfmatCamHomogeneous;
			cv::Mat finalMatrix = robotMat * transfmatSBHomogeneous;

			transfmatSB = transfmatSBHomogeneous(cv::Rect(0, 0, 4, 3));
			transfmatSB2 = finalMatrix(cv::Rect(0, 0, 4, 3));
			cv::Point3f realPoint(transfmatSB.at<double>(0, 3) / unitConversion, transfmatSB.at<double>(1, 3) / unitConversion, transfmatSB.at<double>(2, 3) / unitConversion); // point in world coordinates
			char text[100];
			sprintf(text, "%Position = %f %f %f", realPoint.x, realPoint.y, realPoint.z);
			putText(undist_in_img, text, cv::Point(5, 15), cv::FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0, 255, 255));
			cv::Point3f realPoint2(transfmatSB2.at<double>(0, 3) / unitConversion, transfmatSB2.at<double>(1, 3) / unitConversion, transfmatSB2.at<double>(2, 3) / unitConversion); // point in world coordinates

			sprintf(text, "%Position = %f %f %f", realPoint2.x, realPoint2.y, realPoint2.z);
			putText(undist_in_img, text, cv::Point(5, 35), cv::FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0, 255, 255));
			//cout << "transfmatSB = " << transfmatSB << endl;
			// Final transfmat re-shaping to satisfy requirements of manipulator program:
			transpose(transfmatSB, transfvec12);
			transfvec12 = transfvec12.reshape(12, 1);
		}
		else		// If NOT found,
		{
			// return extremely huge transfvec12 (which means error/failure):
			transfvec12 = (cv::Mat_<double>(12, 1) << 10.0e+20,
				10.0e+20,
				10.0e+20,
				10.0e+20,
				10.0e+20,
				10.0e+20,
				10.0e+20,
				10.0e+20,
				10.0e+20,
				10.0e+20,
				10.0e+20,
				10.0e+20);
		}

		/*if ((found) && (print))
		{
			cout << "transfvec12 = " << transfvec12 << endl;
			cout << endl;
		}*/

		return found;
	}

	void DrawObjCoordFrame(cv::Mat& undist_in_img, cv::Mat rvec, cv::Mat tvec, cv::Mat camera_matrix, cv::Mat dist_coeffs)
	{
		vector<cv::Point3f>	coord_frame_object_points;		// 3D coordinate frame (origin, x-axis pointer, y-axis pointer, z-axis pointer) points
		vector<cv::Point2f>	coord_frame_image_points;		// 2D image points (on image plane)

															// 3D object point coordinates of the axes' pointer of the target object plane (imprinted with the chessboard pattern)'s coordinate frame:
		cv::Point3f			OBJ_COORD_ORIGIN(0.0f, 0.0f, 0.0f),
			OBJ_COORD_X(10.0f, 0.0f, 0.0f),
			OBJ_COORD_Y(0.0f, 10.0f, 0.0f),
			OBJ_COORD_Z(0.0f, 0.0f, 10.0f);

		// Push in 3D descriptor points of target plane (object)'s coordinate frame:
		coord_frame_object_points.push_back(OBJ_COORD_ORIGIN);
		coord_frame_object_points.push_back(OBJ_COORD_X);
		coord_frame_object_points.push_back(OBJ_COORD_Y);
		coord_frame_object_points.push_back(OBJ_COORD_Z);

		cv::Mat rmat;
		Rodrigues(rvec, rmat);
		//cout<<"rmat = "<<rmat<<endl;
		//cout << "rvec = " << rvec << endl;
		//cout << "tvec = " << tvec << endl;

		//ConvertIntoHomogeneous(rmat);
		//rmat = rmat * RotationMatrix('x', -(PI/2));
		//rmat = rmat * RotationMatrix('y', (PI / 2));
		//ConvertIntoNonHomogeneous(rmat);
		//Rodrigues(rmat, rvec);
		if (!undist_in_img.empty())
		{
			//// Project the 3D descriptor points of target plane's coordinate frame into image plane using computed rvec and tvec:
			projectPoints(coord_frame_object_points, rvec, tvec, camera_matrix, dist_coeffs, coord_frame_image_points);
			//// Draw the projected X-axis of the target plane (object)'s coordinate frame on image plane, on the output image:
			line(undist_in_img, cv::Point(coord_frame_image_points[0].x, coord_frame_image_points[0].y), cv::Point(coord_frame_image_points[1].x, coord_frame_image_points[1].y), cv::Scalar(0, 0, 255), 5, CV_AA);
			putText(undist_in_img, "X", cv::Point(coord_frame_image_points[1].x, coord_frame_image_points[1].y), 1, 1, cv::Scalar(0, 0, 255));
			//// Draw the projected Y-axis of the target plane (object)'s coordinate frame on image plane, on the output image:
			line(undist_in_img, cv::Point(coord_frame_image_points[0].x, coord_frame_image_points[0].y), cv::Point(coord_frame_image_points[2].x, coord_frame_image_points[2].y), cv::Scalar(0, 255, 0), 5, CV_AA);
			putText(undist_in_img, "Y", cv::Point(coord_frame_image_points[2].x, coord_frame_image_points[2].y), 1, 1, cv::Scalar(0, 255, 0));
			//// Draw the projected Z-axis of the target plane (object)'s coordinate frame on image plane, on the output image:
			line(undist_in_img, cv::Point(coord_frame_image_points[0].x, coord_frame_image_points[0].y), cv::Point(coord_frame_image_points[3].x, coord_frame_image_points[3].y), cv::Scalar(255, 0, 0), 5, CV_AA);
			putText(undist_in_img, "Z", cv::Point(coord_frame_image_points[3].x, coord_frame_image_points[3].y), 1, 1, cv::Scalar(255, 0, 0));
		}
	}

	int FindTarget(ImagingSource& mImagingSource, cv::Mat& transfvec12, cv::Mat& img, bool print, bool show_img,MeasurementMode measUnits)
	{
		// Let's say we have 3D object point P, which is seen as 2D image point p on the image plane;
		// the point p is related to point P by applying a rotation cv::Matrix R and a translation vector t to P.
		// See the theoretical explanation here: http://docs.opencv.org/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html

		// Points:
		vector<cv::Point3f>	corner_object_points;			// 3D object points
		vector<cv::Point2f>	corner_image_points;			// 2D image points (on image plane)

		vector<cv::Point3f>	coord_frame_object_points;		// 3D coordinate frame (origin, x-axis pointer, y-axis pointer, z-axis pointer) points
		vector<cv::Point2f>	coord_frame_image_points;		// 2D image points (on image plane)

		cv::Mat				my_camera_matrix, my_dist_coeffs;

		bool			found = false;

	


		// Translation and Rotation Vectors:
		cv::Mat				tvec;							// translation vector
		cv::Mat				rvec;							// rotation vector (convertible to rotation cv::Matrix via Rodrigues transformation)

		cv::Size calibratedImageSize;
		switch (measUnits)
		{
		case MeasurementMode::CENTIMETERS:
			unitConversion = 10;
			break;
		case MeasurementMode::MILLIMETERS:
			unitConversion = 1;
			break;
		case MeasurementMode::INCHES:
			unitConversion = 25.4;
			break;
		default:
			unitConversion = 1;
			break;
		}
		LoadCameraParams(GetDevice(), my_camera_matrix, my_dist_coeffs, calibratedImageSize);

		CalcBoardCornerPositions(corner_object_points);

		img = GetImage();

		UndistortImage(img, my_camera_matrix, my_dist_coeffs);

		found = GetPose(R_C_Matrix, img, transfvec12, rvec, tvec, corner_object_points, my_camera_matrix, my_dist_coeffs, print);

		if (show_img)
		{
			if (found)		// If done with success,
			{
				DrawObjCoordFrame(img, rvec, tvec, my_camera_matrix, my_dist_coeffs);
			}
		}

		if (found)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}

	void SetRobotCoordinates(cv::Mat robotPosition)
	{
		cv::Mat temp;
		//cv::Mat frameRotation = RotationMatrix('x', -(PI / 2));
		double thetaRad = robotPosition.at<double>(1) * PI / 180;
		cv::Mat thetaRotation = RotationMatrix('z', thetaRad);
		double thetaY = -sin(thetaRad) * THETAARMLENGTH;
		double thetaX = cos(thetaRad) * THETAARMLENGTH;
		//cv::Mat Rotation = thetaRotation * frameRotation;
		ConvertIntoNonHomogeneous(thetaRotation);
		cv::Mat transMatrix = (cv::Mat_<double>(3, 1) << robotPosition.at<double>(0) * 25.4 + thetaX, thetaY, robotPosition.at<double>(2) * 25.4);
		cv::hconcat(thetaRotation, transMatrix, R_C_Matrix);
		cv::vconcat(R_C_Matrix, (cv::Mat)(cv::Mat_<double>(1, 4) << 0.0, 0.0, 0.0, 1.0), R_C_Matrix);

		//cout << R_C_Matrix << endl;
	}