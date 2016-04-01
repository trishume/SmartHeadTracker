// The SmartHeadTracker Head Tracker
// Copyright (C) 2016  Tristan Hume
// Released under MIT license, see LICENSE file for full text

#include "markertracking.h"

#include <opencv2/highgui/highgui.hpp>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <chrono>

typedef cv::Point3_<uint8_t> UPixel;
typedef cv::Point3_<float> FPixel;
typedef cv::Point_<uint32_t> IPixel;

static const int fixedMult = 10000;

using namespace cv;

static inline uint32_t vecDist(IPixel normalized, IPixel target) {
  uint32_t xDist = normalized.x - target.x;
  uint32_t yDist = normalized.y - target.y;
  return xDist*xDist + yDist*yDist;
}

static void morphologicalClose(Mat &m) {
  Mat structuringEl = getStructuringElement(MORPH_ELLIPSE, Size(5,5));
  dilate(m,m,structuringEl);
  erode(m,m,structuringEl);
}

static Point2f findMarkerBlob(Mat &mask, Mat &origImage) {
  Mat labels, stats, centroids;
  connectedComponentsWithStats(mask, labels, stats, centroids, 8, CV_16U);
  int maxIdx;
  Mat areas = stats.col(CC_STAT_AREA);
  areas.adjustROI(-1,0,0,0);
  if(areas.empty()) return Point2f(0.0,0.0);
  minMaxIdx(areas, nullptr, nullptr, nullptr, &maxIdx);
  Point2f markerCenter(centroids.at<double>(maxIdx+1,0),centroids.at<double>(maxIdx+1,1));

  // line(origImage, Point(markerCenter.x, 0), Point(markerCenter.x, origImage.size().height), Scalar(255,255,255));
  // line(origImage, Point(0, markerCenter.y), Point(origImage.size().width, markerCenter.y), Scalar(255,255,255));
  // imshow("main", origImage);

  return markerCenter;
}

static void colourDistances(Mat &m) {
  IPixel target(0.35*fixedMult,0.4*fixedMult);
  IPixel target2(0.43*fixedMult,0.55*fixedMult);
  Size size = m.size();
  for(int i = 0; i < size.height; i++) {
    UPixel* row = m.ptr<UPixel>(i);
    for(int j = 0; j < size.width; j++) {
      UPixel *p = row+j;
      if(p->z == 0) continue;

      IPixel normalized(p->x*fixedMult/(p->z), p->y*fixedMult/(p->z));
      uint32_t dist1 = vecDist(normalized, target);
      uint32_t dist2 = vecDist(normalized, target2);
      uint32_t dist = std::min(dist1, dist2);

      // p->x = 0;
      // p->y = (uint8_t)std::min(dist / (fixedMult*10), 255U);
      p->z = (dist < 70*fixedMult && p->z > 160) ? 255 : 0;
    }
  }
}

Point2f trackMarkers(Mat &m) {
  auto start = std::chrono::system_clock::now();

  colourDistances(m);

  auto end1 = std::chrono::system_clock::now();
  auto elapsed1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start);
  std::cout << elapsed1.count() << '\n';

  std::vector<Mat> rgb;
  split(m, rgb);
  Mat mask = rgb[2];

  morphologicalClose(mask);
  // imshow("mask",mask);

  Point2f res = findMarkerBlob(mask, m);

  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << elapsed.count() << '\n';

  return res;
}
