#include "PoseEstimation.h"
#include "ImagingSource.h"
#include "CameraCalibration.h"



int main(int argc, const char** argv)
{
	cv::Mat robotPosition = (cv::Mat_<double>(3, 1) << 0,0,0);
	MeasurementMode measurementUnits = MeasurementMode::CENTIMETERS;

	string option;
	ImagingSource mImagingSource = ImagingSource();
	// Overall Transformation Vector:
	cv::Mat		transfvec12;
	// Image Buffer:
	cv::Mat		img;
	// Flags:
	bool	print = true; // atoi(argv[1]);
	bool	show_img = true;  // atoi(argv[2]);


	// target_found = 1 if target pattern is found, otherwise target_found = -1:
	int		target_found;

	//VideoCapture capture;

	//capture.open(1);

	//if (!capture.isOpened())
	//	return fprintf(stderr, "Could not initialize video capture\n"), -2;

	mImagingSource.listDevices(); 
	string name = mImagingSource.GetDeviceName(0);
	string serial = to_string(mImagingSource.GetDeviceSerial(0));
	//mImagingSource.setDeviceID(0);
	//mImagingSource.setCaptureMode(CaptureMode::VIDEO);
	//mImagingSource.videoSettings();

	SetRobotCoordinates(robotPosition);
	CalibrateCamera("", name, serial);
	GetCamera(1);
	if (show_img)
	{
		// Create displaying window:
		cv::namedWindow("Object Postion", CV_WINDOW_NORMAL);
	}
	//while (mImagingSource.getFrame().empty());
	while (true)
	{
		//if (!mImagingSource.getFrame().empty())
			target_found = FindTarget(mImagingSource, transfvec12, img, print, show_img, measurementUnits);


		if (show_img)
		{
			imshow("Object Postion", img);
		}

		// Wait for 10 miliseconds until user press some key:
		int iKey = cv::waitKey(10);

		/*	char c = (char)waitKey(imageList.empty() && !box.empty() ? 30 : 300);
			if (c == 'q' || c == 'Q')
				break;
			if (c == '\r' || c == '\n')
				grabNext = true;*/
		// If user press "ESC" key:
		if (iKey == 27)
		{
			//imwrite("Result.jpg", img);
			break;	// Break the infinite loop...
		}
		else if (iKey == 'c')
		{
			measurementUnits = MeasurementMode::CENTIMETERS;
		}
		else if (iKey == 'm')
		{
			measurementUnits = MeasurementMode::MILLIMETERS;
		}
		else if (iKey == 'i')
		{
			measurementUnits = MeasurementMode::INCHES;
		}
		else if (iKey == 'r')
		{
			double p;
			cout << "Enter X Position:" << endl;
			cin >> p;
			robotPosition.at<double>(0) = p;
			cout << "Enter Theta Position:" << endl;
			cin >> p;
			robotPosition.at<double>(1) = p;
			cout << "Enter Z Position:" << endl;
			cin >> p;
			robotPosition.at<double>(2) = p;
			SetRobotCoordinates(robotPosition);
		}
		else if (iKey == '\r' || iKey == '\n')
		{
			char text[100];
			sprintf(text, "%Position = %f %f %f", robotPosition.at<double>(0), robotPosition.at<double>(1), robotPosition.at<double>(2));
			cout << text << endl;
		}
	}

	if (show_img)
	{
		// Close displaying windows:
		cvDestroyWindow("Object Postion");
	}

	ReleaseCamera();
	return 0;

}

