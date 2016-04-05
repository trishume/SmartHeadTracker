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

using namespace cv;

static const double kCannyLowThresh = 30;
static const double kCannyRatio = 3;

static void morphologicalClose(Mat &m) {
  Mat structuringEl = getStructuringElement(MORPH_ELLIPSE, Size(5,5));
  dilate(m,m,structuringEl);
  erode(m,m,structuringEl);
}

static Point2f findMarkerBlob(Mat &mask, Mat &origImage, bool showUI) {
  Mat labels, stats, centroids;
  connectedComponentsWithStats(mask, labels, stats, centroids, 8, CV_16U);
  Mat areas = stats.col(CC_STAT_AREA);
  areas.adjustROI(-1,0,0,0);
  if(areas.empty()) return Point2f(0.0,0.0);
  int maxIdx;
  minMaxIdx(areas, nullptr, nullptr, nullptr, &maxIdx);
  // Point2f markerCenter(centroids.at<double>(maxIdx+1,0),centroids.at<double>(maxIdx+1,1));

  // Size s = origImage.size();
  // Mat floodMask = Mat::zeros(Size(s.width+2,s.height+2), CV_8UC1);
  // int lo = 50;
  // int up = 60;
  // floodFill(origImage, floodMask, Point(markerCenter.x,markerCenter.y), Scalar(0,0,255), nullptr,
  //   Scalar(lo,lo,lo), Scalar(up,up,up), 4 | FLOODFILL_FIXED_RANGE);

  Mat channels[3];
  split(origImage, channels);
  Mat detectedEdges;
  blur(channels[2], detectedEdges, Size(3,3));
  Canny(detectedEdges,detectedEdges,kCannyLowThresh,kCannyLowThresh*kCannyRatio);

  compare(labels, Scalar(maxIdx+1), labels, CMP_EQ);
  Mat structuringEl = getStructuringElement(MORPH_ELLIPSE, Size(5,5));
  dilate(labels, labels, structuringEl);
  Mat intersect;
  bitwise_and(detectedEdges, labels, intersect);

  std::vector<cv::Point> pts;
  for(int j=0; j<intersect.rows; ++j) {
    uint8_t *row = intersect.ptr<uint8_t>(j);
    for(int i=0; i<intersect.cols; ++i) {
      if(row[i]) {
        pts.push_back(cv::Point(i,j));
      }
    }
  }
  RotatedRect fittedEllipse = fitEllipse(pts);
  Point2f markerCenter = fittedEllipse.center;

  if(showUI) {
    // imshow("edges",detectedEdges);
    // imshow("mask",labels);
    // imshow("intersect", intersect);
    ellipse(origImage, fittedEllipse, Scalar(0,255,0));
    line(origImage, Point(markerCenter.x, 0), Point(markerCenter.x, origImage.size().height), Scalar(255,255,255));
    line(origImage, Point(0, markerCenter.y), Point(origImage.size().width, markerCenter.y), Scalar(255,255,255));
    imshow("main", origImage);
  }

  return markerCenter;
}

Point2f trackMarkers(HalideGens *gens, Mat &m, bool showUI) {
  auto start = std::chrono::high_resolution_clock::now();

  Mat mask = colourMatchMask(gens, m);

  // auto end1 = std::chrono::high_resolution_clock::now();
  // auto elapsed1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start);
  // std::cout << "color: " << elapsed1.count() << '\n';

  // imshow("mask orig",mask);
  morphologicalClose(mask);
  // imshow("mask",mask);

  Point2f res = findMarkerBlob(mask, m, showUI);

  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "vision: " << elapsed.count() << '\n';

  return res;
}
