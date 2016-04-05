// The SmartHeadTracker Head Tracker
// Copyright (C) 2016  Tristan Hume
// Released under MIT license, see LICENSE file for full text

#pragma once

#include <opencv2/imgproc/imgproc.hpp>

#include "halideFuncs.h"

cv::Point2f trackMarkers(HalideGens *gens, cv::Mat &m, bool showUI);
