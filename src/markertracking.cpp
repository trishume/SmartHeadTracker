// The SmartHeadTracker Head Tracker
// Copyright (C) 2016  Tristan Hume
// Released under MIT license, see LICENSE file for full text

#include "markertracking.h"

#include <opencv2/highgui/highgui.hpp>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <iostream>

typedef cv::Point3_<uint8_t> UPixel;
typedef cv::Point3_<float> FPixel;
using namespace cv;

float vecDist(FPixel a, FPixel b) {
  FPixel diff = a - b;
  return diff.x*diff.x + diff.y*diff.y + diff.z*diff.z;
}

static void morphologicalClose(Mat &m) {
  Mat structuringEl = getStructuringElement(MORPH_ELLIPSE, Size(5,5));
  dilate(m,m,structuringEl);
  erode(m,m,structuringEl);
}

static void findMarkerBlob(Mat &mask, Mat &origImage) {
  Mat labels, stats, centroids;
  connectedComponentsWithStats(mask, labels, stats, centroids, 8, CV_16U);
  if(centroids.empty()) return;
  int maxIdx;
  Mat areas = stats.col(CC_STAT_AREA);
  areas.adjustROI(-1,0,0,0);
  minMaxIdx(areas, nullptr, nullptr, nullptr, &maxIdx);
  Point2f markerCenter(centroids.at<double>(maxIdx+1,0),centroids.at<double>(maxIdx+1,1));
  // std::cout << markerCenter << std::endl;

  // circle(origImage, Point(markerCenter.x, markerCenter.y), 2, Scalar(255,255,255));
  line(origImage, Point(markerCenter.x, 0), Point(markerCenter.x, origImage.size().height), Scalar(255,255,255));
  line(origImage, Point(0, markerCenter.y), Point(origImage.size().width, markerCenter.y), Scalar(255,255,255));
  imshow("main", origImage);
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

    p.z = (p.y < 25 && maxc > 160) ? 255 : 0;
  }

  std::vector<Mat> rgb;
  split(m, rgb);
  Mat mask = rgb[2];

  morphologicalClose(mask);
  // imshow("mask",mask);

  findMarkerBlob(mask, m);
}
