
#include "CameraCalibration.h"


	//! [compute_errors]
	double ComputeReprojectionErrors(const std::vector<std::vector<cv::Point3f> >& objectPoints,
		const std::vector<std::vector<cv::Point2f> >& imagePoints,
		const std::vector<cv::Mat>& rvecs, const std::vector<cv::Mat>& tvecs,
		const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
		std::vector<float>& perViewErrors, bool fisheye)
{
	std::vector<cv::Point2f> imagePoints2;
	size_t totalPoints = 0;
	double totalErr = 0, err;
	perViewErrors.resize(objectPoints.size());

	for (size_t i = 0; i < objectPoints.size(); ++i)
	{
		if (fisheye)
		{
			cv::fisheye::projectPoints(objectPoints[i], imagePoints2, rvecs[i], tvecs[i], cameraMatrix,
				distCoeffs);
		}
		else
		{
			projectPoints(objectPoints[i], rvecs[i], tvecs[i], cameraMatrix, distCoeffs, imagePoints2);
		}
		err = norm(imagePoints[i], imagePoints2, cv::NORM_L2);

		size_t n = objectPoints[i].size();
		perViewErrors[i] = (float)std::sqrt(err*err / n);
		totalErr += err * err;
		totalPoints += n;
	}

	return std::sqrt(totalErr / totalPoints);
}

	void SaveCameraCalibrationParams(CameraSettings& s, cv::Size& imageSize, cv::Mat& cameraMatrix, cv::Mat& distCoeffs,
		const std::vector<cv::Mat>& rvecs, const std::vector<cv::Mat>& tvecs,
		const std::vector<float>& reprojErrs, const std::vector<std::vector<cv::Point2f> >& imagePoints,
		double totalAvgErr)
	{
		cv::FileStorage fs(s.outputFileName, cv::FileStorage::WRITE);

		time_t tm;
		time(&tm);
		struct tm *t2 = localtime(&tm);
		char buf[1024];
		strftime(buf, sizeof(buf), "%c", t2);

		fs << "calibration_time" << buf;

		fs << "camera_model" << s.cameraModel;
		fs << "camera_serial" << s.cameraSerial;

		if (!rvecs.empty() || !reprojErrs.empty())
			fs << "nr_of_frames" << (int)std::max(rvecs.size(), reprojErrs.size());
		fs << "image_width" << imageSize.width;
		fs << "image_height" << imageSize.height;
		fs << "board_width" << s.boardSize.width;
		fs << "board_height" << s.boardSize.height;
		fs << "square_size" << s.squareSize;

		if (s.flag & cv::CALIB_FIX_ASPECT_RATIO)
			fs << "fix_aspect_ratio" << s.aspectRatio;

		if (s.flag)
		{
			std::stringstream flagsStringStream;
			if (s.useFisheye)
			{
				flagsStringStream << "flags:"
					<< (s.flag & cv::fisheye::CALIB_FIX_SKEW ? " +fix_skew" : "")
					<< (s.flag & cv::fisheye::CALIB_FIX_K1 ? " +fix_k1" : "")
					<< (s.flag & cv::fisheye::CALIB_FIX_K2 ? " +fix_k2" : "")
					<< (s.flag & cv::fisheye::CALIB_FIX_K3 ? " +fix_k3" : "")
					<< (s.flag & cv::fisheye::CALIB_FIX_K4 ? " +fix_k4" : "")
					<< (s.flag & cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC ? " +recompute_extrinsic" : "");
			}
			else
			{
				flagsStringStream << "flags:"
					<< (s.flag & cv::CALIB_USE_INTRINSIC_GUESS ? " +use_intrinsic_guess" : "")
					<< (s.flag & cv::CALIB_FIX_ASPECT_RATIO ? " +fix_aspectRatio" : "")
					<< (s.flag & cv::CALIB_FIX_PRINCIPAL_POINT ? " +fix_principal_point" : "")
					<< (s.flag & cv::CALIB_ZERO_TANGENT_DIST ? " +zero_tangent_dist" : "")
					<< (s.flag & cv::CALIB_FIX_K1 ? " +fix_k1" : "")
					<< (s.flag & cv::CALIB_FIX_K2 ? " +fix_k2" : "")
					<< (s.flag & cv::CALIB_FIX_K3 ? " +fix_k3" : "")
					<< (s.flag & cv::CALIB_FIX_K4 ? " +fix_k4" : "")
					<< (s.flag & cv::CALIB_FIX_K5 ? " +fix_k5" : "");
			}
			fs.writeComment(flagsStringStream.str());
		}

		fs << "flags" << s.flag;

		fs << "fisheye_model" << s.useFisheye;

		fs << "camera_matrix" << cameraMatrix;
		fs << "distortion_coefficients" << distCoeffs;

		fs << "avg_reprojection_error" << totalAvgErr;
		if (s.writeExtrinsics && !reprojErrs.empty())
			fs << "per_view_reprojection_errors" << cv::Mat(reprojErrs);

		if (s.writeExtrinsics && !rvecs.empty() && !tvecs.empty())
		{
			CV_Assert(rvecs[0].type() == tvecs[0].type());
			cv::Mat bigmat((int)rvecs.size(), 6, CV_MAKETYPE(rvecs[0].type(), 1));
			bool needReshapeR = rvecs[0].depth() != 1 ? true : false;
			bool needReshapeT = tvecs[0].depth() != 1 ? true : false;

			for (size_t i = 0; i < rvecs.size(); i++)
			{
				cv::Mat r = bigmat(cv::Range(int(i), int(i + 1)), cv::Range(0, 3));
				cv::Mat t = bigmat(cv::Range(int(i), int(i + 1)), cv::Range(3, 6));

				if (needReshapeR)
					rvecs[i].reshape(1, 1).copyTo(r);
				else
				{
					//*.t() is MatExpr (not Mat) so we can use assignment operator
					CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
					r = rvecs[i].t();
				}

				if (needReshapeT)
					tvecs[i].reshape(1, 1).copyTo(t);
				else
				{
					CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);
					t = tvecs[i].t();
				}
			}
			fs.writeComment("a set of 6-tuples (rotation vector + translation vector) for each view");
			fs << "extrinsic_parameters" << bigmat;
		}

		if (s.writePoints && !imagePoints.empty())
		{
			cv::Mat imagePtMat((int)imagePoints.size(), (int)imagePoints[0].size(), CV_32FC2);
			for (size_t i = 0; i < imagePoints.size(); i++)
			{
				cv::Mat r = imagePtMat.row(int(i)).reshape(2, imagePtMat.cols);
				cv::Mat imgpti(imagePoints[i]);
				imgpti.copyTo(r);
			}
			fs << "image_points" << imagePtMat;
		}
	}
	
	void CalcBoardCornerPositions(cv::Size boardSize, float squareSize, std::vector<cv::Point3f>& corners,
		CameraSettings::Pattern patternType /*= Settings::CHESSBOARD*/)
	{
		corners.clear();

		switch (patternType)
		{
		case CameraSettings::CHESSBOARD:
		case CameraSettings::CIRCLES_GRID:
			for (int i = 0; i < boardSize.height; ++i)
				for (int j = 0; j < boardSize.width; ++j)
					corners.push_back(cv::Point3f(j*squareSize, i*squareSize, 0));
			break;

		case CameraSettings::ASYMMETRIC_CIRCLES_GRID:
			for (int i = 0; i < boardSize.height; i++)
				for (int j = 0; j < boardSize.width; j++)
					corners.push_back(cv::Point3f((2 * j + i % 2)*squareSize, i*squareSize, 0));
			break;
		default:
			break;
		}
	}						 //! [run_and_save]
	
	bool RunCalibration(CameraSettings& s, cv::Size& imageSize, cv::Mat& cameraMatrix, cv::Mat& distCoeffs,
		std::vector<std::vector<cv::Point2f> > imagePoints, std::vector<cv::Mat>& rvecs, std::vector<cv::Mat>& tvecs,
		std::vector<float>& reprojErrs, double& totalAvgErr)
	{
		//! [fixed_aspect]
		cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
		if (s.flag & cv::CALIB_FIX_ASPECT_RATIO)
			cameraMatrix.at<double>(0, 0) = s.aspectRatio;
		//! [fixed_aspect]
		if (s.useFisheye) {
			distCoeffs = cv::Mat::zeros(4, 1, CV_64F);
		}
		else {
			distCoeffs = cv::Mat::zeros(8, 1, CV_64F);
		}

		std::vector<std::vector<cv::Point3f> > objectPoints(1);
		CalcBoardCornerPositions(s.boardSize, s.squareSize, objectPoints[0], s.calibrationPattern);

		objectPoints.resize(imagePoints.size(), objectPoints[0]);

		//Find intrinsic and extrinsic camera parameters
		double rms;

		if (s.useFisheye) {
			cv::Mat _rvecs, _tvecs;
			rms = cv::fisheye::calibrate(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs, _rvecs,
				_tvecs, s.flag);

			rvecs.reserve(_rvecs.rows);
			tvecs.reserve(_tvecs.rows);
			for (int i = 0; i < int(objectPoints.size()); i++) {
				rvecs.push_back(_rvecs.row(i));
				tvecs.push_back(_tvecs.row(i));
			}
		}
		else {
			rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs,
				s.flag);
		}

		std::cout << "Re-projection error reported by calibrateCamera: " << rms << std::endl;

		bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);

		totalAvgErr = ComputeReprojectionErrors(objectPoints, imagePoints, rvecs, tvecs, cameraMatrix,
			distCoeffs, reprojErrs, s.useFisheye);

		return ok;
	}

	bool RunCalibrationAndSave(CameraSettings& s, cv::Size imageSize, cv::Mat& cameraMatrix, cv::Mat& distCoeffs,
		std::vector<std::vector<cv::Point2f> > imagePoints)
	{
		std::vector<cv::Mat> rvecs, tvecs;
		std::vector<float> reprojErrs;
		double totalAvgErr = 0;

		bool ok = RunCalibration(s, imageSize, cameraMatrix, distCoeffs, imagePoints, rvecs, tvecs, reprojErrs,
			totalAvgErr);
		std::cout << (ok ? "Calibration succeeded" : "Calibration failed")
			<< ". avg re projection error = " << totalAvgErr << std::endl;

		if (ok)
			SaveCameraCalibrationParams(s, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs, reprojErrs, imagePoints,
				totalAvgErr);
		return ok;
	}


	static inline void read(const cv::FileNode& node, CameraSettings& x, const CameraSettings& default_value = CameraSettings())
	{
		if (node.empty())
			x = default_value;
		else
			x.read(node);
	}

	static inline void write(cv::FileStorage& fs, const std::string&, const CameraSettings& s)
	{
		s.write(fs);
	}

	bool CalibrateCamera(const std::string inputFile, std::string cameraName, std::string serial)
	{
		std::vector<cv::Point3f> boardPoints;
		CameraSettings s;
		const std::string inputSettingsFile = inputFile != "" ? inputFile : "configuration\\default.xml";
		//if (inputFile == "")inputSettingsFile = "default.xml";
		cv::FileStorage fs(inputSettingsFile, cv::FileStorage::READ); // Read the settings
		if (!fs.isOpened())
		{
			std::cout << "Could not open the configuration file: \"" << inputSettingsFile << "\"" << std::endl;
			return false;
		}

		fs["CameraSettings"] >> s;

		s.cameraModel = cameraName;
		s.cameraSerial = serial;

		fs.release();

		if (!s.goodInput)
		{
			std::cout << "Invalid input detected. Application stopping. " << std::endl;
			return false;
		}
		std::vector<std::vector<cv::Point2f> > imagePoints;
		cv::Mat cameraMatrix, distCoeffs;
		cv::Size imageSize;
		int mode = s.inputType == CameraSettings::IMAGE_LIST ? CAPTURING : DETECTION;
		clock_t prevTimestamp = 0;
		const cv::Scalar RED(0, 0, 255), GREEN(0, 255, 0);
		const char ESC_KEY = 27;

		bool grabNext = !s.imageList.empty();

		//! [get_input]
		for (;;)
		{
			cv::Mat view;
			bool blinkOutput = false;

			view = s.nextImage();

			//-----  If no more image, or got enough, then stop calibration and show result -------------
			if (mode == CAPTURING && imagePoints.size() >= (size_t)s.nrFrames)
			{
				if (RunCalibrationAndSave(s, imageSize, cameraMatrix, distCoeffs, imagePoints))
					mode = CALIBRATED;
				else
					mode = DETECTION;
			}
			if (view.empty())          // If there are no more images stop the loop
			{
				// if calibration threshold was not reached yet, calibrate now
				if (mode != CALIBRATED && !imagePoints.empty())
					RunCalibrationAndSave(s, imageSize, cameraMatrix, distCoeffs, imagePoints);
				break;
			}
			//! [get_input]

			imageSize = view.size();  // Format input image.
			if (s.flipVertical)    flip(view, view, 0);

			//! [find_pattern]
			std::vector<cv::Point2f> pointBuf;

			bool found;

			int chessBoardFlags = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE;

			if (!s.useFisheye) {
				// fast check erroneously fails with high distortions like fisheye
				chessBoardFlags |= cv::CALIB_CB_FAST_CHECK;
			}

			switch (s.calibrationPattern) // Find feature points on the input format
			{
			case CameraSettings::CHESSBOARD:
				found = findChessboardCorners(view, s.boardSize, pointBuf, chessBoardFlags);
				break;
			case CameraSettings::CIRCLES_GRID:
				found = findCirclesGrid(view, s.boardSize, pointBuf);
				break;
			case CameraSettings::ASYMMETRIC_CIRCLES_GRID:
				found = findCirclesGrid(view, s.boardSize, pointBuf, cv::CALIB_CB_ASYMMETRIC_GRID);
				break;
			default:
				found = false;
				break;
			}
			//! [find_pattern]
			//! [pattern_found]
			if (found && grabNext)                // If done with success,
			{
				// improve the found corners' coordinate accuracy for chessboard
				if (s.calibrationPattern == CameraSettings::CHESSBOARD)
				{
					cv::Mat viewGray;
					cvtColor(view, viewGray, cv::COLOR_BGR2GRAY);
					cornerSubPix(viewGray, pointBuf, cv::Size(11, 11),
						cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));
				}

				if (mode == CAPTURING &&  // For camera only take new samples after delay time
					(!s.inputCapture.isOpened() || clock() - prevTimestamp > s.delay*1e-3*CLOCKS_PER_SEC))
				{
					imagePoints.push_back(pointBuf);
					prevTimestamp = clock();
					blinkOutput = s.inputCapture.isOpened();

				}

				// Draw the corners.
				drawChessboardCorners(view, s.boardSize, cv::Mat(pointBuf), found);
				grabNext = !s.imageList.empty();
			}
			//! [pattern_found]
			//----------------------------- Output Text ------------------------------------------------
			//! [output_text]
			std::string msg = (mode == CAPTURING) ? "100/100" :
				mode == CALIBRATED ? "Calibrated" : "Press 'g' to start";
			int baseLine = 0;
			cv::Size textSize = cv::getTextSize(msg, 1, 1, 1, &baseLine);
			cv::Point textOrigin(view.cols - 2 * textSize.width - 10, view.rows - 2 * baseLine - 10);

			if (mode == CAPTURING)
			{
				if (s.showUndistorsed)
					msg = cv::format("%d/%d Undist", (int)imagePoints.size(), s.nrFrames);
				else
					msg = cv::format("%d/%d", (int)imagePoints.size(), s.nrFrames);
			}

			putText(view, msg, textOrigin, 1, 1, mode == CALIBRATED ? GREEN : RED);

			if (blinkOutput)
				bitwise_not(view, view);
			//! [output_text]
			//------------------------- Video capture  output  undistorted ------------------------------
			//! [output_undistorted]
			if (mode == CALIBRATED && s.showUndistorsed)
			{
				cv::Mat temp = view.clone();
				if (s.useFisheye)
					cv::fisheye::undistortImage(temp, view, cameraMatrix, distCoeffs);
				else
					undistort(temp, view, cameraMatrix, distCoeffs);
			}
			//! [output_undistorted]
			//------------------------------ Show image and check for input commands -------------------
			//! [await_input]
			imshow("Camera Calibration", view);
			char key = (char)cv::waitKey(s.inputCapture.isOpened() ? 50 : s.delay);

			if (key == ESC_KEY)
			{
				cvDestroyAllWindows();
				break;
			}

			if (key == 'u' && mode == CALIBRATED)
				s.showUndistorsed = !s.showUndistorsed;
			if (mode == CAPTURING)
			{
				if (key == '\r' || key == '\n')
					grabNext = true;
			}

			if (s.inputCapture.isOpened() && key == 'g')
			{
				mode = CAPTURING;
				imagePoints.clear();
			}
			//! [await_input]
		}

		// -----------------------Show the undistorted image for the image list ------------------------
		//! [show_results]
		if (s.inputType == CameraSettings::IMAGE_LIST && s.showUndistorsed)
		{
			cv::Mat view, rview, map1, map2;

			if (s.useFisheye)
			{
				cv::Mat newCamMat;
				cv::fisheye::estimateNewCameraMatrixForUndistortRectify(cameraMatrix, distCoeffs, imageSize,
					cv::Matx33d::eye(), newCamMat, 1);
				cv::fisheye::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Matx33d::eye(), newCamMat, imageSize,
					CV_16SC2, map1, map2);
			}
			else
			{
				initUndistortRectifyMap(
					cameraMatrix, distCoeffs, cv::Mat(),
					getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, 0), imageSize,
					CV_16SC2, map1, map2);
			}

			for (size_t i = 0; i < s.imageList.size(); i++)
			{
				view = cv::imread(s.imageList[i], cv::IMREAD_COLOR);
				if (view.empty())
					continue;
				remap(view, rview, map1, map2, cv::INTER_LINEAR);
				imshow("Camera Calibration", rview);
				char c = (char)cv::waitKey();
				if (c == ESC_KEY || c == 'q' || c == 'Q')
				{
					cvDestroyAllWindows();
					break;
				}
					
			}
		}
		//! [show_results]
	}                            
	// close Settings file
	CameraSettings::CameraSettings() : goodInput(false) {}

	void CameraSettings::write(cv::FileStorage& fs)  const                         //Write serialization for this class
	{
		fs << "{"
			<< "Camera_Model" << cameraModel
			<< "Camera_Serial" << cameraSerial
			<< "BoardSize_Width" << boardSize.width
			<< "BoardSize_Height" << boardSize.height
			<< "Square_Size" << squareSize
			<< "Calibrate_Pattern" << patternToUse
			<< "Calibrate_NrOfFrameToUse" << nrFrames
			<< "Calibrate_FixAspectRatio" << aspectRatio
			<< "Calibrate_AssumeZeroTangentialDistortion" << calibZeroTangentDist
			<< "Calibrate_FixPrincipalPointAtTheCenter" << calibFixPrincipalPoint

			<< "Write_DetectedFeaturePoints" << writePoints
			<< "Write_extrinsicParameters" << writeExtrinsics
			<< "Write_outputFileName" << outputFileName

			<< "Show_UndistortedImage" << showUndistorsed

			<< "Input_FlipAroundHorizontalAxis" << flipVertical
			<< "Input_Delay" << delay
			<< "Input" << input
			<< "}";
	}

	void CameraSettings::read(const cv::FileNode& node)                          //Read serialization for this class
	{
		node["Camera_Model"] >> cameraModel;
		node["Camera_Serial"] >> cameraSerial;
		node["BoardSize_Width"] >> boardSize.width;
		node["BoardSize_Height"] >> boardSize.height;
		node["Calibrate_Pattern"] >> patternToUse;
		node["Square_Size"] >> squareSize;
		node["Calibrate_NrOfFrameToUse"] >> nrFrames;
		node["Calibrate_FixAspectRatio"] >> aspectRatio;
		node["Write_DetectedFeaturePoints"] >> writePoints;
		node["Write_extrinsicParameters"] >> writeExtrinsics;
		node["Write_outputFileName"] >> outputFileName;
		node["Calibrate_AssumeZeroTangentialDistortion"] >> calibZeroTangentDist;
		node["Calibrate_FixPrincipalPointAtTheCenter"] >> calibFixPrincipalPoint;
		node["Calibrate_UseFisheyeModel"] >> useFisheye;
		node["Input_FlipAroundHorizontalAxis"] >> flipVertical;
		node["Show_UndistortedImage"] >> showUndistorsed;
		node["Input"] >> input;
		node["Input_Delay"] >> delay;
		node["Fix_Kg1"] >> fixK1;
		node["Fix_K2"] >> fixK2;
		node["Fix_K3"] >> fixK3;
		node["Fix_K4"] >> fixK4;
		node["Fix_K5"] >> fixK5;

		validate();
	}

	void CameraSettings::validate()
	{
		goodInput = true;
		if (boardSize.width <= 0 || boardSize.height <= 0)
		{
			std::cerr << "Invalid Board size: " << boardSize.width << " " << boardSize.height << std::endl;
			goodInput = false;
		}
		if (squareSize <= 10e-6)
		{
			std::cerr << "Invalid square size " << squareSize << std::endl;
			goodInput = false;
		}
		if (nrFrames <= 0)
		{
			std::cerr << "Invalid number of frames " << nrFrames << std::endl;
			goodInput = false;
		}

		if (input.empty())      // Check for valid input
			inputType = INVALID;
		else
		{
			if (input[0] >= '0' && input[0] <= '9')
			{
				std::stringstream ss(input);
				ss >> cameraID;
				inputType = CAMERA;
			}
			else
			{
				if (isListOfImages(input) && readStringList(input, imageList))
				{
					inputType = IMAGE_LIST;
					nrFrames = (nrFrames < (int)imageList.size()) ? nrFrames : (int)imageList.size();
				}
				else
					inputType = VIDEO_FILE;
			}
			if (inputType == CAMERA)
			{
				inputCapture.open(cameraID);
				GetCamera(0);
				//cameraSerial = inputCapture.get(CV_CAP_PROP_XI_DEVICE_SN);
				//cameraModel = inputCapture.get(CV_CAP_PROP_XI_DEVICE_MODEL_ID);		
			}
			if (inputType == VIDEO_FILE)
				inputCapture.open(input);
			if (inputType != IMAGE_LIST && !inputCapture.isOpened())
				inputType = INVALID;
		}
		if (inputType == INVALID)
		{
			std::cerr << " Input does not exist: " << input;
			goodInput = false;
		}

		flag = 0;
		if (calibFixPrincipalPoint) flag |= cv::CALIB_FIX_PRINCIPAL_POINT;
		if (calibZeroTangentDist)   flag |= cv::CALIB_ZERO_TANGENT_DIST;
		if (aspectRatio)            flag |= cv::CALIB_FIX_ASPECT_RATIO;
		if (fixK1)                  flag |= cv::CALIB_FIX_K1;
		if (fixK2)                  flag |= cv::CALIB_FIX_K2;
		if (fixK3)                  flag |= cv::CALIB_FIX_K3;
		if (fixK4)                  flag |= cv::CALIB_FIX_K4;
		if (fixK5)                  flag |= cv::CALIB_FIX_K5;

		if (useFisheye) {
			// the fisheye model has its own enum, so overwrite the flags
			flag = cv::fisheye::CALIB_FIX_SKEW | cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;
			if (fixK1)                   flag |= cv::fisheye::CALIB_FIX_K1;
			if (fixK2)                   flag |= cv::fisheye::CALIB_FIX_K2;
			if (fixK3)                   flag |= cv::fisheye::CALIB_FIX_K3;
			if (fixK4)                   flag |= cv::fisheye::CALIB_FIX_K4;
			if (calibFixPrincipalPoint) flag |= cv::fisheye::CALIB_FIX_PRINCIPAL_POINT;
		}

		calibrationPattern = NOT_EXISTING;
		if (!patternToUse.compare("CHESSBOARD")) calibrationPattern = CHESSBOARD;
		if (!patternToUse.compare("CIRCLES_GRID")) calibrationPattern = CIRCLES_GRID;
		if (!patternToUse.compare("ASYMMETRIC_CIRCLES_GRID")) calibrationPattern = ASYMMETRIC_CIRCLES_GRID;
		if (calibrationPattern == NOT_EXISTING)
		{
			std::cout << " Camera calibration mode does not exist: " << patternToUse << std::endl;
			goodInput = false;
		}
		atImageList = 0;

	};

	cv::Mat CameraSettings::nextImage()
	{
		cv::Mat result;
		if (inputCapture.isOpened())
		{
			cv::Mat view0;
			inputCapture >> view0;
			view0.copyTo(result);
		}
		else if (atImageList < imageList.size())
			result = cv::imread(this->imageList[atImageList++], cv::IMREAD_COLOR);

		return result;
	}

	bool CameraSettings::readStringList(const std::string& filename, std::vector<std::string>& l)
	{
		l.clear();
		cv::FileStorage fs(filename, cv::FileStorage::READ);
		if (!fs.isOpened())
			return false;
		cv::FileNode n = fs.getFirstTopLevelNode();
		if (n.type() != cv::FileNode::SEQ)
			return false;
		cv::FileNodeIterator it = n.begin(), it_end = n.end();
		for (; it != it_end; ++it)
			l.push_back((std::string)*it);
		return true;
	}

	bool CameraSettings::isListOfImages(const std::string& filename)
	{
		std::string s(filename);
		// Look for file extension
		if (s.find(".xml") == std::string::npos && s.find(".yaml") == std::string::npos && s.find(".yml") == std::string::npos)
			return false;
		else
			return true;
	}


		/*
		double computeReprojectionErrors(
		const std::vector<std::vector<cv::Point3f> >& objectPoints,
		const std::vector<std::vector<cv::Point2f> >& imagePoints,
		const std::vector<cv::Mat>& rvecs, const std::vector<cv::Mat>& tvecs,
		const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
		std::vector<float>& perViewErrors)
		{
		std::vector<cv::Point2f> imagePoints2;
		int i, totalPoints = 0;
		double totalErr = 0, err;
		perViewErrors.resize(objectPoints.size());

		for (i = 0; i < (int)objectPoints.size(); i++)
		{
		projectPoints(cv::Mat(objectPoints[i]), rvecs[i], tvecs[i],
		cameraMatrix, distCoeffs, imagePoints2);
		err = cv::norm(cv::Mat(imagePoints[i]), cv::Mat(imagePoints2), cv::NORM_L2);
		int n = (int)objectPoints[i].size();
		perViewErrors[i] = (float)std::sqrt(err*err / n);
		totalErr += err * err;
		totalPoints += n;
		}

		return std::sqrt(totalErr / totalPoints);
		}
		*/

		//void calcChessboardCorners(cv::Size boardSize, float squareSize, std::vector<cv::Point3f>& corners, CameraSettings::Pattern patternType )
		//{
		//	corners.resize(0);

		//	switch (patternType)
		//	{
		//	case CameraSettings::CHESSBOARD:
		//	case CameraSettings::CIRCLES_GRID:
		//		for (int i = 0; i < boardSize.height; i++)
		//			for (int j = 0; j < boardSize.width; j++)
		//				corners.push_back(cv::Point3f(float(j*squareSize),
		//					float(i*squareSize), 0));
		//		break;

		//	case CameraSettings::ASYMMETRIC_CIRCLES_GRID:
		//		for (int i = 0; i < boardSize.height; i++)
		//			for (int j = 0; j < boardSize.width; j++)
		//				corners.push_back(cv::Point3f(float((2 * j + i % 2)*squareSize),
		//					float(i*squareSize), 0));
		//		break;

		//	default:
		//		//CV_Error(Error::StsBadArg, "Unknown pattern type\n");
		//	}
		//}

		//bool runCalibration(std::vector<std::vector<cv::Point2f> > imagePoints,
		//	cv::Size imageSize, cv::Size boardSize, CameraSettings::Pattern patternType,
		//	float squareSize, float aspectRatio, float focalLength,
		//	int flags, cv::Mat& cameraMatrix, cv::Mat& distCoeffs,
		//	std::vector<cv::Mat>& rvecs, std::vector<cv::Mat>& tvecs,
		//	std::vector<float>& reprojErrs,
		//	double& totalAvgErr)
		//{
		//	cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
		//	if (flags & cv::CALIB_FIX_ASPECT_RATIO)
		//		cameraMatrix.at<double>(0, 0) = aspectRatio;

		//	if (flags & cv::CALIB_FIX_FOCAL_LENGTH)
		//	{
		//		cameraMatrix.at<double>(0, 0) = focalLength;
		//		cameraMatrix.at<double>(1, 1) = focalLength;
		//	}
		//	distCoeffs = cv::Mat::zeros(8, 1, CV_64F);

		//	std::vector<std::vector<cv::Point3f> > objectPoints(1);
		//	calcChessboardCorners(boardSize, squareSize, objectPoints[0], patternType);

		//	objectPoints.resize(imagePoints.size(), objectPoints[0]);

		//	double rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
		//		distCoeffs, rvecs, tvecs, flags | cv::CALIB_FIX_K4 | cv::CALIB_FIX_K5);
		//	///*|cv::CALIB_FIX_K3*/|cv::CALIB_FIX_K4|cv::CALIB_FIX_K5);
		//	std::printf("RMS error reported by calibrateCamera: %g\n", rms);

		//	bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);

		//	totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
		//		rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);

		//	return ok;
		//}


		//void saveCameraParams(const std::string& filename,
		//	cv::Size imageSize, cv::Size boardSize,
		//	float squareSize, float aspectRatio, int flags,
		//	const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
		//	const std::vector<cv::Mat>& rvecs, const std::vector<cv::Mat>& tvecs,
		//	const std::vector<float>& reprojErrs,
		//	const std::vector<std::vector<cv::Point2f>>& imagePoints,
		//	double totalAvgErr)
		//{
		//	cv::FileStorage fs(filename, cv::FileStorage::WRITE);

		//	time_t tt;
		//	time(&tt);
		//	struct tm *t2 = localtime(&tt);
		//	char buf[1024];
		//	strftime(buf, sizeof(buf) - 1, "%c", t2);

		//	fs << "calibration_time" << buf;

		//	if (!rvecs.empty() || !reprojErrs.empty())
		//		fs << "nframes" << (int)std::max(rvecs.size(), reprojErrs.size());
		//	fs << "image_width" << imageSize.width;
		//	fs << "image_height" << imageSize.height;
		//	fs << "board_width" << boardSize.width;
		//	fs << "board_height" << boardSize.height;
		//	fs << "square_size" << squareSize;

		//	if (flags & cv::CALIB_FIX_ASPECT_RATIO)
		//		fs << "aspectRatio" << aspectRatio;

		//	if (flags != 0)
		//	{
		//		sprintf(buf, "flags: %s%s%s%s",
		//			flags & cv::CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "",
		//			flags & cv::CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "",
		//			flags & cv::CALIB_FIX_PRINCIPAL_POINT ? "+fix_principal_point" : "",
		//			flags & cv::CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "");
		//		cvWriteComment(*fs, buf, 0);
		//	}

		//	fs << "flags" << flags;

		//	fs << "camera_matrix" << cameraMatrix;
		//	fs << "distortion_coefficients" << distCoeffs;

		//	fs << "avg_reprojection_error" << totalAvgErr;
		//	if (!reprojErrs.empty())
		//		fs << "per_view_reprojection_errors" << cv::Mat(reprojErrs);

		//	if (!rvecs.empty() && !tvecs.empty())
		//	{
		//		CV_Assert(rvecs[0].type() == tvecs[0].type());
		//		cv::Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());
		//		for (int i = 0; i < (int)rvecs.size(); i++)
		//		{
		//			cv::Mat r = bigmat(cv::Range(i, i + 1), cv::Range(0, 3));
		//			cv::Mat t = bigmat(cv::Range(i, i + 1), cv::Range(3, 6));

		//			CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
		//			CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);
		//			//*.t() is MatExpr (not Mat) so we can use assignment operator
		//			r = rvecs[i].t();
		//			t = tvecs[i].t();
		//		}
		//		cvWriteComment(*fs, "a set of 6-tuples (rotation vector + translation vector) for each view", 0);
		//		fs << "extrinsic_parameters" << bigmat;
		//	}

		//	if (!imagePoints.empty())
		//	{
		//		cv::Mat imagePtMat((int)imagePoints.size(), (int)imagePoints[0].size(), CV_32FC2);
		//		for (int i = 0; i < (int)imagePoints.size(); i++)
		//		{
		//			cv::Mat r = imagePtMat.row(i).reshape(2, imagePtMat.cols);
		//			cv::Mat imgpti(imagePoints[i]);
		//			imgpti.copyTo(r);
		//		}
		//		fs << "image_points" << imagePtMat;
		//	}
		//}

		//bool readStringList(const std::string& filename, std::vector<std::string>& l)
		//{
		//	l.resize(0);
		//	cv::FileStorage fs(filename, cv::FileStorage::READ);
		//	if (!fs.isOpened())
		//		return false;
		//	cv::FileNode n = fs.getFirstTopLevelNode();
		//	if (n.type() != cv::FileNode::SEQ)
		//		return false;
		//	cv::FileNodeIterator it = n.begin(), it_end = n.end();
		//	for (; it != it_end; ++it)
		//		l.push_back((std::string)*it);
		//	return true;
		//}


		//bool runAndSave(CameraSettings& s,
		//	const std::vector<std::vector<cv::Point2f> >& imagePoints,
		//	cv::Size imageSize, cv::Size boardSize, CameraSettings::Pattern patternType, float squareSize,
		//	float aspectRatio, float focalLength, int flags, cv::Mat& cameraMatrix,
		//	cv::Mat& distCoeffs, bool writeExtrinsics, bool writePoints)
		//{
		//	std::vector<cv::Mat> rvecs, tvecs;
		//	std::vector<float> reprojErrs;
		//	double totalAvgErr = 0;

		//	bool ok = runCalibration(imagePoints, imageSize, boardSize, patternType, squareSize,
		//		aspectRatio, focalLength, flags, cameraMatrix, distCoeffs,
		//		rvecs, tvecs, reprojErrs, totalAvgErr);
		//	printf("%s. avg reprojection error = %.2f\n",
		//		ok ? "Calibration succeeded" : "Calibration failed",
		//		totalAvgErr);

		//	if (ok)
		//		saveCameraCalibrationParams(s, imageSize,
		//			boardSize, squareSize, aspectRatio,
		//			flags, cameraMatrix, distCoeffs,
		//			writeExtrinsics ? rvecs : std::vector<cv::Mat>(),
		//			writeExtrinsics ? tvecs : std::vector<cv::Mat>(),
		//			writeExtrinsics ? reprojErrs : std::vector<float>(),
		//			writePoints ? imagePoints : std::vector<std::vector<cv::Point2f> >(),
		//			totalAvgErr);
		//	//cv::find
		//	return ok;
		//}

		///// <summary>
		///// points in world coordinate
		///// </summary>
		///// <param name="corners3D">coordinate value</param>
		///// <param name="chessBoardSize">size of chessboard</param>
		///// <param name="nImages">number of images</param>
		///// <param name="squareSize">actual size of square</param>
		//void objectCorners3D(std::vector<cv::Point3f>& corners, cv::Size boardSize, float squareSize)
		//{
		//	corners.resize(0);

		//	for (int i = 0; i < boardSize.height; i++)
		//		for (int j = 0; j < boardSize.width; j++)
		//			corners.push_back(cv::Point3f(float(j*squareSize),
		//				float(i*squareSize), 0));

		//}

		// Print camera parameters to the output file

		/*
		//
		//bool calibrate(std::string inputSettingsFile)
		//{
		//	/*cv::Mat view;
		//	cv::Size boardSize, imageSize;
		//	float squareSize, aspectRatio;
		//	cv::Mat cameraMatrix, distCoeffs;
		//	std::string outputFilename;
		//	std::string inputFilename = "";
		//	float focalLength = 0;
		//	int i, nframes;
		//	bool writeExtrinsics, writePoints;
		//	bool undistortImage = false;
		//	int flags = 0;
		//	cv::VideoCapture capture;
		//	bool flipVertical;
		//	bool showUndistorted;
		//	bool videofile;
		//	int delay;
		//	clock_t prevTimestamp = 0;
		//	int mode = DETECTION;
		//	int cameraId = 0;
		//	std::vector<std::vector<cv::Point2f> > imagePoints;
		//	std::vector<std::string> imageList;
		//	Pattern pattern = CHESSBOARD;
		//	std::vector<cv::Point3f> boardPoints;*/
		/*
		//	//cv::CommandLineParser parser(argc, argv,
		//	//	"{help ||}{w||}{h||}{ff||}{pt|chessboard|}{n|10|}{d|2|}{s||}{o|out_camera_data.yml|}"
		//	//	"{op||}{oe||}{zt||}{a|1|}{p||}{v||}{V||}{su||}"
		//	//	"{@input_data|1|}");
		//	//if (parser.has("help"))
		//	//{
		//	//	//help();
		//	//	return 0;
		//	//}
		//	//boardSize.width = parser.get<int>("w");
		//	//boardSize.height = parser.get<int>("h");
		//	//if (parser.has("pt"))
		//	//{
		//	//	std::string val = parser.get<std::string>("pt");
		//	//	if (val == "circles")
		//	//		pattern = CIRCLES_GRID;
		//	//	else if (val == "acircles")
		//	//		pattern = ASYMMETRIC_CIRCLES_GRID;
		//	//	else if (val == "chessboard")
		//	//		pattern = CHESSBOARD;
		//	//	else
		//	//		return fprintf(stderr, "Invalid pattern type: must be chessboard or circles\n"), -1;
		//	//}
		//	//squareSize = parser.get<float>("s");
		//	//nframes = parser.get<int>("n");
		//	//aspectRatio = parser.get<float>("a");
		//	//delay = parser.get<int>("d");
		//	//writePoints = parser.has("op");
		//	//writeExtrinsics = parser.has("oe");
		//	//if (parser.has("a"))
		//	//	flags |= cv::CALIB_FIX_ASPECT_RATIO;
		//	//if (parser.has("zt"))
		//	//	flags |= cv::CALIB_ZERO_TANGENT_DIST;
		//	//if (parser.has("p"))
		//	//	flags |= cv::CALIB_FIX_PRINCIPAL_POINT;
		//	//if (parser.has("ff"))
		//	//{
		//	//	focalLength = parser.get<float>("ff");
		//	//	flags |= cv::CALIB_FIX_FOCAL_LENGTH;
		//	//}
		//	//flipVertical = parser.has("v");
		//	//videofile = parser.has("V");
		//	//if (parser.has("o"))
		//	//	outputFilename = parser.get<std::string>("o");
		//	//showUndistorted = parser.has("su");
		//	//if (isdigit(parser.get<std::string>("@input_data")[0]))
		//	//	cameraId = parser.get<int>("@input_data");
		//	//else
		//	//	inputFilename = parser.get<std::string>("@input_data");
		//	//if (!parser.check())
		//	//{
		//	//	//help();
		//	//	parser.printErrors();
		//	//	return -1;
		//	//}
		//	//if (squareSize <= 0)
		//	//	return fprintf(stderr, "Invalid board square width\n"), -1;
		//	//if (nframes <= 3)
		//	//	return printf("Invalid number of images\n"), -1;
		//	//if (aspectRatio <= 0)
		//	//	return printf("Invalid aspect ratio\n"), -1;
		//	//if (delay <= 0)
		//	//	return printf("Invalid delay\n"), -1;
		//	//if (boardSize.width <= 0)
		//	//	return fprintf(stderr, "Invalid board width\n"), -1;
		//	//if (boardSize.height <= 0)
		//	//	return fprintf(stderr, "Invalid board height\n"), -1;

		//	//if (!inputFilename.empty())
		//	//{
		//	//	if (!videofile && readStringList(inputFilename, imageList))
		//	//		mode = CAPTURING;
		//	//	else
		//	//		capture.open(inputFilename);
		//	//}
		//	//else
		//	//	capture.open(cameraId);

		//	//if (!capture.isOpened() && imageList.empty())
		//	//	return fprintf(stderr, "Could not initialize video (%d) capture\n", cameraId), -2;

		//	//if (!imageList.empty())
		//	//	nframes = (int)imageList.size();



		//	//bool grabNext = !imageList.empty();

		//	//cv::namedWindow("Image View", 1);
		//	//
		//	//calcChessboardCorners(s.boardSize, s.squareSize, boardPoints, s.calibrationPattern);

		//	//for (i = 0;; i++)
		//	//{
		//	//	cv::Mat viewGray;
		//	//	bool blink = false;

		//	//	if (capture.isOpened())
		//	//	{
		//	//		cv::Mat view0;
		//	//		capture >> view0;
		//	//		view0.copyTo(view);
		//	//	}
		//	//	else if (i < (int)imageList.size())
		//	//		view = cv::imread(imageList[i], 1);

		//	//	if (view.empty())
		//	//	{
		//	//		if (imagePoints.size() > 0)
		//	//			runAndSave(outputFilename, imagePoints, imageSize,
		//	//				boardSize, pattern, squareSize, aspectRatio,
		//	//				flags, focalLength, cameraMatrix, distCoeffs,
		//	//				writeExtrinsics, writePoints);
		//	//		break;
		//	//	}

		//	//	imageSize = view.size();

		//	//	if (flipVertical)
		//	//		flip(view, view, 0);

		//	//	std::string msg = mode == CAPTURING ? "100/100" :
		//	//		mode == CALIBRATED ? "Calibrated" : "Press 'g' to start";
		//	//	int baseLine = 0;
		//	//	cv::Size textSize = cv::getTextSize(msg, 1, 1, 1, &baseLine);
		//	//	cv::Point textOrigin(view.cols - 2 * textSize.width - 10, view.rows - 2 * baseLine - 10);

		//	//	std::vector<cv::Point2f> pointbuf;
		//	//	cvtColor(view, viewGray, cv::COLOR_BGR2GRAY);

		//	//	bool found;
		//	//	if (grabNext)
		//	//	{
		//	//		switch (pattern)
		//	//		{
		//	//		case CHESSBOARD:
		//	//			found = findChessboardCorners(view, boardSize, pointbuf,
		//	//				cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE);
		//	//			break;
		//	//		case CIRCLES_GRID:
		//	//			found = findCirclesGrid(view, boardSize, pointbuf);
		//	//			break;
		//	//		case ASYMMETRIC_CIRCLES_GRID:
		//	//			found = findCirclesGrid(view, boardSize, pointbuf, cv::CALIB_CB_ASYMMETRIC_GRID);
		//	//			break;
		//	//		default:
		//	//			return fprintf(stderr, "Unknown pattern type\n"), -1;
		//	//		}

		//	//		// improve the found corners' coordinate accuracy
		//	//		if (pattern == CHESSBOARD && found)
		//	//			cornerSubPix(viewGray, pointbuf, cv::Size(5, 5), cv::Size(-1, -1),
		//	//				cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));

		//	//		if (mode == CAPTURING && found &&
		//	//			(!capture.isOpened() || clock() - prevTimestamp > delay*1e-3*CLOCKS_PER_SEC))
		//	//		{
		//	//			imagePoints.push_back(pointbuf);
		//	//			prevTimestamp = clock();
		//	//			blink = capture.isOpened();
		//	//		}

		//	//		if (found)
		//	//			drawChessboardCorners(view, boardSize, cv::Mat(pointbuf), found);


		//	//		if (mode == CAPTURING)
		//	//		{
		//	//			if (undistortImage)
		//	//				msg = cv::format("%d/%d Undist", (int)imagePoints.size(), nframes);
		//	//			else
		//	//				msg = cv::format("%d/%d", (int)imagePoints.size(), nframes);
		//	//		}

		//	//		putText(view, msg, textOrigin, 1, 1,
		//	//			mode != CALIBRATED ? cv::Scalar(0, 0, 255) : cv::Scalar(0, 255, 0));

		//	//		if (blink)
		//	//			bitwise_not(view, view);

		//	//		if (mode == CALIBRATED && undistortImage)
		//	//		{
		//	//			cv::Mat temp = view.clone();
		//	//			undistort(temp, view, cameraMatrix, distCoeffs);
		//	//		}

		//	//		imshow("Image View", view);
		//	//		char key = (char)cv::waitKey(capture.isOpened() ? 50 : 500);

		//	//		if (key == 27)
		//	//			break;

		//	//		cv::Mat rvec, tvec;
		//	//		if (mode == CALIBRATED && key == 'p')
		//	//		{
		//	//			cv::solvePnP(cv::Mat(boardPoints), cv::Mat(pointbuf), cameraMatrix,
		//	//				distCoeffs, rvec, tvec, false);

		//	//			cv::Mat R;
		//	//			cv::Rodrigues(rvec, R); // R is 3x3

		//	//			R = R.t();  // rotation of inverse
		//	//			tvec = -R * tvec; // translation of inverse

		//	//			cv::Mat T = cv::Mat::eye(4, 4, R.type()); // T is 4x4
		//	//			T(cv::Range(0, 3), cv::Range(0, 3)) = R * 1; // copies R into T
		//	//			T(cv::Range(0, 3), cv::Range(3, 4)) = tvec * 1; // copies tvec into T
		//	//			fprintf(stderr, "Unknown pattern type\n"), -1;
		//	//		}

		//	//		if (key == 'u' && mode == CALIBRATED)
		//	//		{
		//	//			undistortImage = !undistortImage;
		//	//		}

		//	//		if (capture.isOpened() && key == 'g')
		//	//		{
		//	//			mode = CAPTURING;
		//	//			imagePoints.clear();
		//	//		}

		//	//		if (mode == CAPTURING && imagePoints.size() >= (unsigned)nframes)
		//	//		{
		//	//			if (runAndSave(outputFilename, imagePoints, imageSize,
		//	//				boardSize, pattern, squareSize, aspectRatio, focalLength,
		//	//				flags, cameraMatrix, distCoeffs,
		//	//				writeExtrinsics, writePoints))
		//	//				mode = CALIBRATED;
		//	//			else
		//	//				mode = DETECTION;
		//	//			if (!capture.isOpened())
		//	//				break;
		//	//		}
		//	//		grabNext = !imageList.empty();
		//	//	}
		//	//	else
		//	//	{
		//	//		cv::imshow("Image View", view);
		//	//	}
		//	//	char c = (char)cv::waitKey(imageList.empty() && capture.isOpened() ? 30 : 300);
		//	//	if (c == '\r' || c == '\n')
		//	//	{
		//	//		mode == CAPTURING;
		//	//		grabNext = true;
		//	//	}

		//	//}

		//	//if (!capture.isOpened() && showUndistorted)
		//	//{
		//	//	cv::Mat view, rview, map1, map2;
		//	//	initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(),
		//	//		getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, 0),
		//	//		imageSize, CV_16SC2, map1, map2);

		//	//	for (i = 0; i < (int)imageList.size(); i++)
		//	//	{
		//	//		view = cv::imread(imageList[i], 1);
		//	//		if (view.empty())
		//	//			continue;
		//	//		undistort(view, rview, cameraMatrix, distCoeffs, cameraMatrix);
		//	//		remap(view, rview, map1, map2, cv::INTER_LINEAR);
		//	//		imshow("Image View", rview);
		//	//		char c = (char)cv::waitKey();
		//	//		if (c == 27 || c == 'q' || c == 'Q')
		//	//			break;
		//	//	}
		//	//}

		//	return true;
		//}
		*/