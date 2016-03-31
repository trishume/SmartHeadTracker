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

float vecDist(FPixel a, FPixel b) {
  FPixel diff = a - b;
  return diff.x*diff.x + diff.y*diff.y + diff.z*diff.z;
}

void trackMarkers(Mat &m) {
  FPixel target(0.35,0.4,1.0);
  FPixel target2(0.43,0.55,1.0);
  for (UPixel &p : cv::Mat_<UPixel>(m)) {
    float maxc = std::max(p.x, std::max(p.y, p.z));
    FPixel normalized(p.x / maxc, p.y / maxc, p.z / maxc);
    float dist = std::min(vecDist(normalized,target),vecDist(normalized,target2));
    p.x = 0;
    p.y = (uint8_t)std::min(dist*3000.0f, 255.0f);

    p.z = (p.y < 25 && maxc > 180) ? 255 : 0;
  }

  imshow("main", m);
}
