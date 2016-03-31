// The SmartHeadTracker Head Tracker
// Copyright (C) 2016  Tristan Hume
// Released under MIT license, see LICENSE file for full text

#pragma once

struct Server {
  void *context;
  void *publisher;

  Server();
  ~Server();
  void send(float x, float y);
};
