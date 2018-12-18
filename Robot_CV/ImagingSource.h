#pragma once


#ifndef IMAGING_SOURCE_H
#define IMAGING_SOURCE_H

#include <opencv2/core.hpp>
//#include <opencv2/highgui.hpp>
#include <iostream>

// Import ImagingSource Library
#define UDSHL_LIB_NO_COMPILER_CHECK
#include <tisudshl.h>
// TODO extend ofBaseVideoGrabber



enum class CaptureMode {
	STILL,
	VIDEO,
};

class ImagingSource : DShowLib::GrabberListener {
public:
	ImagingSource();
	~ImagingSource();

	void listDevices();
	void setDeviceID(int id);
	void videoSettings();

	void setDeviceSerial(long long serial);
	//void loadSettings(const ci::fs::path &path);
	//void saveSettings(const ci::fs::path &path);

	void setCaptureMode(CaptureMode mode);
	CaptureMode getCaptureMode();
	long long GetDeviceSerial(int selectedIndex);
	std::string GetDeviceName(int selectedIndex);

	cv::Mat getFrame();
	void snapStill();

	void setIsEnabled(bool value);
	bool getIsEnabled();
	void ListAllPropertyItems();

private:
	DShowLib::Grabber *mGrabber;
	DShowLib::FrameHandlerSink::tFHSPtr mFrameHandlerSink;
	cv::Mat mFrame;

	// GrabberListener implementation
	virtual void frameReady(DShowLib::Grabber &caller, smart_ptr<DShowLib::MemBuffer> pBuffer, DWORD FrameNumber);
	// virtual void deviceLost(Grabber& caller);
	// virtual void overlayCallback(Grabber& caller, smart_ptr<OverlayBitmap> pBitmap, const tsMediaSampleDesc& desc);
};

#endif