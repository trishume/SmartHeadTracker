// The SmartHeadTracker Head Tracker
// Copyright (C) 2016  Tristan Hume
// Released under MIT license, see LICENSE file for full text

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono>

#include "ps3eye.h"
#include "colorconv.h"

#include "markertracking.h"
#include "server.h"

static const int kCaptureWidth = 640;
static const int kCaptureHeight = 480;
// static const int kCaptureWidth = 320;
// static const int kCaptureHeight = 240;
static const int kCaptureFPS = 75;

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
  if(!res) return 2;
  eye->start();
  eye->setAutoWhiteBalance(false);
  eye->setGain(10);

  namedWindow("main");
  namedWindow("raw");
  // int trackVal1 = 10;
  // createTrackbar("val1","main",&trackVal1,255);

  uint8_t *videoFrame  = new unsigned char[eye->getWidth()*eye->getHeight()*3];
  Mat cvFrame(Size(kCaptureWidth, kCaptureHeight), CV_8UC3, (void*)videoFrame);

  Server server;
  HalideGens *gens = createGens();
  bool showUI = false;

  while(true) {
    auto start = std::chrono::system_clock::now();
    uint8_t* new_pixels = eye->getFrame();
    yuv422_to_bgr(new_pixels, eye->getRowBytes(), videoFrame, eye->getWidth(),eye->getHeight());
    free(new_pixels);

    // if(trackVal1 != eye->getRedBalance()) eye->setRedBalance(trackVal1);

    // imshow("raw",cvFrame);
    Point2f pt = trackMarkers(gens, cvFrame, showUI);
    if(pt.x != 0.0 || pt.y != 0.0)
      server.send(kCaptureWidth - pt.x, pt.y);

    int chr = waitKey(1);
    if(chr == 'q') break;
    else if(chr == 'u') showUI = !showUI;

    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "frame: " << elapsed.count() << '\n';
  }

  if(eye) eye->stop();
  deleteGens(gens);
  delete[] videoFrame;
  return 0;
}
