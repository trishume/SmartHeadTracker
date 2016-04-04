// The SmartHeadTracker Head Tracker
// Copyright (C) 2016  Tristan Hume
// Released under MIT license, see LICENSE file for full text

#pragma once

#include "filters.h"

struct Server {
  void *context;
  void *publisher;

  one_euro_filter<> xFilt;
  one_euro_filter<> yFilt;

  Server();
  ~Server();
  void send(float x, float y);
};
