// The SmartHeadTracker Head Tracker
// Copyright (C) 2016  Tristan Hume
// Released under MIT license, see LICENSE file for full text

#include "markertracking.h"

#include <opencv2/highgui/highgui.hpp>
#include <cstdint>
#include <cmath>
#include <algorithm>

typedef cv::Point3_<uint8_t> UPixel;
typedef cv::Point3_<float> FPixel;
using namespace cv;

void trackMarkers(Mat &m) {
  FPixel target(0.35,0.4,1.0);
  for (UPixel &p : cv::Mat_<UPixel>(m)) {
    float maxc = std::max(p.x, std::max(p.y, p.z));
    FPixel normalized(p.x / maxc, p.y / maxc, p.z / maxc);
    FPixel diff = normalized - target;
    float dist = std::abs(diff.x) + std::abs(diff.y) + std::abs(diff.z);
    p.x = 0;
    p.y = (uint8_t)std::min(dist*200.0f, 255.0f);

    p.z = (p.y < 30 && maxc > 150) ? 255 : 0;
  }

  imshow("main", m);
}
