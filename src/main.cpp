// The SmartHeadTracker Head Tracker
// Copyright (C) 2016  Tristan Hume
// Released under MIT license, see LICENSE file for full text

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <vector>
#include <cstdint>

#include "colorconv.h"
#include "ps3eye.h"

static const int kCaptureWidth = 640;
static const int kCaptureHeight = 480;
static const int kCaptureFPS = 60;

using namespace ps3eye;
using namespace cv;

int main() {
  std::vector<PS3EYECam::PS3EYERef> devices( PS3EYECam::getDevices() );

  if(devices.empty()) {
    std::cerr << "No cameras found, can't run." << std::endl;
    return 1;
  }

  PS3EYECam::PS3EYERef eye = devices.at(0);
  bool res = eye->init(kCaptureWidth, kCaptureHeight, kCaptureFPS);
  eye->start();
  eye->setAutoWhiteBalance(false);
  eye->setGain(10);
  // eye->setRedBalance(0);

  namedWindow("main");
  // int trackVal1 = 10;
  // createTrackbar("val1","main",&trackVal1,255);

  uint8_t *videoFrame  = new unsigned char[eye->getWidth()*eye->getHeight()*3];
  Mat cvFrame(Size(kCaptureWidth, kCaptureHeight), CV_8UC3, (void*)videoFrame);

  while(true) {
    uint8_t* new_pixels = eye->getFrame();
    yuv422_to_bgr(new_pixels, eye->getRowBytes(), videoFrame, eye->getWidth(),eye->getHeight());
    free(new_pixels);

    // if(trackVal1 != eye->getExposure()) eye->setExposure(trackVal1);

    imshow("main", cvFrame);
    int chr = waitKey(4);
    if(chr == 'q') break;
  }

  if(eye) eye->stop();
  delete[] videoFrame;
  return 0;
}
