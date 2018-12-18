#include "ImagingSource.h"


ImagingSource::ImagingSource() {
	if (DShowLib::InitLibrary()) {
		std::cout << "IC Imaging Control Library Initialized." << std::endl;
	} else {
		std::cout << "IC Imaging Control Library failed to initialized, unsupported platform." << std::endl;
	}

	mGrabber = new DShowLib::Grabber();

	// Create a FrameTypeInfoArray data structure describing the allowed color formats.
	DShowLib::FrameTypeInfoArray acceptedTypes = DShowLib::FrameTypeInfoArray::createRGBArray();
	mFrameHandlerSink = DShowLib::FrameHandlerSink::create(acceptedTypes, 1);
	mFrameHandlerSink->setSnapMode(false);
	
	if (!mGrabber->setSinkType(mFrameHandlerSink)) {
		std::cout << "Failed to set sink type." << std::endl;
	}

	// register for events
	mGrabber->addListener(this, GrabberListener::eFRAMEREADY);
}

ImagingSource::~ImagingSource() {
	delete mGrabber;
}

void ImagingSource::listDevices() {
	std::cout << "Listing devices" << std::endl;

	DShowLib::Grabber::tVidCapDevListPtr deviceList = mGrabber->getAvailableVideoCaptureDevices();

	if (deviceList == 0 || deviceList->empty()) {
		std::cout << "No cameras detected." << std::endl;
	} else {
		int index = 0;

		for (DShowLib::VideoCaptureDeviceItem &device : *deviceList) {
			long long serialNumber = 0;
			if (device.getSerialNumber(serialNumber)) {
				std::cout << "Camera ID: " << index << "\t Name: " << device.getBaseName() << " \tSerial: " << serialNumber << std::endl;
				index++;
			} else {
				std::cout << "Could not get camera serial number" << std::endl;
			}
		}
	}
}

std::string ImagingSource::GetDeviceName(int selectedIndex) {
	DShowLib::Grabber::tVidCapDevListPtr deviceList = mGrabber->getAvailableVideoCaptureDevices();

	if (deviceList == 0 || deviceList->empty()) {
		std::cout << "No cameras detected." << std::endl;
	}
	else {
		int index = 0;

		for (DShowLib::VideoCaptureDeviceItem &device : *deviceList) {
			if (index == selectedIndex)			
			{
				return device.getBaseName();
			}
			index++;
		}
		std::cout << "Could not get camera Name" << std::endl;
		return "";
	}
}

long long ImagingSource::GetDeviceSerial(int selectedIndex) {

	DShowLib::Grabber::tVidCapDevListPtr deviceList = mGrabber->getAvailableVideoCaptureDevices();

	if (deviceList == 0 || deviceList->empty()) {
		std::cout << "No cameras detected." << std::endl;
	}
	else {
		int index = 0;

		for (DShowLib::VideoCaptureDeviceItem &device : *deviceList) {
			if (index == selectedIndex)
			{
				long long serialNumber = 0;
				if (device.getSerialNumber(serialNumber))
					return serialNumber;
				else {
					std::cout << "Could not get camera serial number" << std::endl;
					return 0;
				}
			}
			index++;
		}
		std::cout << "Could not get camera serial number" << std::endl;
		return 0;
	}
}

void ImagingSource::videoSettings() {
	std::cout << "Showing camera settings window." << std::endl;

	const bool wasLive = mGrabber->isLive();
	if (wasLive) {
		mGrabber->stopLive();
	}

	mGrabber->showDevicePage();
	//mGrabber->showVCDPropertyPage();

	if (wasLive) {
		mGrabber->startLive(true);
	}
}

void ImagingSource::setDeviceID(int id) {
	// get serial of device index
	DShowLib::Grabber::tVidCapDevListPtr deviceList = mGrabber->getAvailableVideoCaptureDevices();

	if (deviceList == 0 || deviceList->empty()) {
		std::cout << "No cameras detected." << std::endl;
	} else {
		long long serialNumber = 0;
		if (deviceList->at(id).getSerialNumber(serialNumber)) {
			setDeviceSerial(serialNumber);
			mGrabber->getAvailableVCDProperties();
		} else {
			std::cout << "Could not get camera serial number" << std::endl;
		}
	}
}

void ImagingSource::setDeviceSerial(long long serial) {
	// Todo close device

	if (mGrabber->openDev(serial)) {
		std::cout << "Open camera with serial number " << serial << std::endl;
		if (mGrabber->startLive(false)) {
			std::cout << "Camera is live." << std::endl;
			ListAllPropertyItems();


		} else {
			std::cout << "Failed to take the camera live." << std::endl;
		}

	} else {
		std::cout << "Failed to open camera with serial number " << serial << std::endl;
	}
}


void ImagingSource::ListAllPropertyItems()
{
	OutputDebugString(TEXT("\n\nVCD Tree:\n\n"));

	// Get all property items
	DShowLib::tIVCDPropertyItemsPtr pVCDProperties = mGrabber->getAvailableVCDProperties();
	if (pVCDProperties != 0)
	{
		// Iterate through the items and print the names.
		DShowLib::tVCDPropertyItemArray itemArray = pVCDProperties->getItems();
		for (DShowLib::tVCDPropertyItemArray::const_iterator itItem = itemArray.begin(); itItem != itemArray.end(); ++itItem)
		{
			// Print the item name 
			OutputDebugString(((*itItem)->getName().c_str()));
			OutputDebugString(TEXT("\n"));

			// Iterate through the elements and print the names.
			DShowLib::tVCDPropertyElementArray elemArray = (*itItem)->getElements();
			for (DShowLib::tVCDPropertyElementArray::iterator itElem = elemArray.begin(); itElem != elemArray.end(); ++itElem)
			{
				// Print the element name 
				OutputDebugString(TEXT("   Element : "));
				OutputDebugString(((*itElem)->getName().c_str()));
				OutputDebugString(TEXT("\n"));

				// Iterate through the interfaces and print the names.
				DShowLib::tVCDPropertyInterfaceArray itfArray = (*itElem)->getInterfaces();
				for (DShowLib::tVCDPropertyInterfaceArray::const_iterator itItf = itfArray.begin(); itItf != itfArray.end(); ++itItf)
				{
					// Check the interface type and print the name 
					OutputDebugString(TEXT("		Interface : "));

					GUID iid = (*itItf)->getInterfaceID();

					if (iid == DShowLib::IID_IVCDAbsoluteValueProperty)
					{
						OutputDebugString(TEXT("Absolute Value\n"));
					}
					else if (iid == DShowLib::IID_IVCDMapStringsProperty)
					{
						OutputDebugString(TEXT("Mapstrings\n"));
					}
					else if (iid == DShowLib::IID_IVCDSwitchProperty)
					{
						OutputDebugString(TEXT("Switch\n"));
					}
					else if (iid == DShowLib::IID_IVCDButtonProperty)
					{
						OutputDebugString(TEXT("Button\n"));
					}
					else if (iid == DShowLib::IID_IVCDRangeProperty)
					{
						OutputDebugString(TEXT("Range\n"));
					}
				}
			}
		}
	}
}
//void ImagingSource::loadSettings(const ci::fs::path &path) {
//	//CI_LOG_V("Loading camera settings from " << path);
//
//	const bool wasLive = mGrabber->isLive();
//	if (wasLive) {
//		mGrabber->stopLive();
//	}
//
//	if (mGrabber->loadDeviceStateFromFile(path.string())) {
//		//CI_LOG_V("Loaded camera settings successfully");
//	} else {
//		//CI_LOG_E("Loading camera settings failed");
//	}
//
//	if (wasLive) {
//		mGrabber->startLive(false);
//	}
//}

//void ImagingSource::saveSettings(const ci::fs::path &path) {
//	//CI_LOG_V("Saving camera settings to " << path);
//
//	if (mGrabber->saveDeviceStateToFile(path.string())) {
//		//CI_LOG_E("Saved camera settings successfully");
//	} else {
//		//CI_LOG_E("Save camera settings failed");
//	}
//}

void ImagingSource::setCaptureMode(CaptureMode mode) {
	switch (mode) {
		case CaptureMode::STILL:
			mFrameHandlerSink->setSnapMode(true);
			break;
		case CaptureMode::VIDEO:
			mFrameHandlerSink->setSnapMode(false);
			break;
	}
}

CaptureMode ImagingSource::getCaptureMode() {
	return mFrameHandlerSink->getSnapMode() ? CaptureMode::STILL : CaptureMode::VIDEO;
}

void ImagingSource::frameReady(DShowLib::Grabber &caller, smart_ptr<DShowLib::MemBuffer> pBuffer, DWORD FrameNumber) {
	std::cout << "Frame Ready! Number: " << FrameNumber << std::endl;

	//if (!mFrame.empty()) {
		//mFrame = Mat(pBuffer->getSize().cx, pBuffer->getSize().cy, true, cinder::SurfaceChannelOrder::BGRX);
	//}
	//cv::Mat mFrame(pBuffer->getSize().cy, pBuffer->getSize().cx, CV_64FC4/*(ie. CV_8UC4)*/, \
	//	NULL/*(This is the Data Pointer)*/, pBuffer->);
	std::cout << "BitsPerPixel: " << pBuffer->getBitsPerPixel() << std::endl;
	std::cout << "ColorFormat: " << pBuffer->getColorformat() << std::endl;
	std::cout << "Dim(cx): " << pBuffer->getFrameType().dim.cx  << std::endl;
	std::cout << "Dim(cy): " << pBuffer->getFrameType().dim.cy << std::endl;
	std::cout << "Size(cx): " << pBuffer->getSize().cx << std::endl;
	std::cout << "Size(cy): " << pBuffer->getSize().cy << std::endl;
	std::cout << "FrameType: " << pBuffer->getSampleDesc().FrameType << std::endl;
	cv::Mat m(cv::Size(640, 480), CV_8UC3, pBuffer->getPtr());
	//m.copyTo(mFrame);
	//
	mFrame.create(640,480, CV_8UC3);
	m.copyTo(mFrame);
	/*this->mFrame = cv::Mat(pBuffer->getSize().cy, pBuffer->getSize().cx, CV_32SC4);
	pBuffer->lock();
	memcpy(mFrame.ptr(), pBuffer->getPtr(), pBuffer->getSize().cx * pBuffer->getSize().cy * 4);
	pBuffer->unlock();*/
}

cv::Mat ImagingSource::getFrame() {
	return mFrame;
}

void ImagingSource::snapStill() {
	mFrameHandlerSink->snapImagesAsync(1);
}

void ImagingSource::setIsEnabled(bool value) {
	if (value) {
		mGrabber->startLive(false);
	} else {
		mGrabber->stopLive();
	}
}

bool ImagingSource::getIsEnabled() {
	return mGrabber->isLive();
}
