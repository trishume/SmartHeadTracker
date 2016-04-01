// The SmartGaze Eye Tracker
// Copyright (C) 2016  Tristan Hume
// Released under GPLv2, see LICENSE file for full text

#include <opencv2/imgproc/imgproc.hpp>

struct HalideGens;
HalideGens *createGens();
void deleteGens(HalideGens *gens);

cv::Mat colourMatchMask(HalideGens *gens, cv::Mat &m);
