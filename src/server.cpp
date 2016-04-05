// The SmartHeadTracker Head Tracker
// Copyright (C) 2016  Tristan Hume
// Released under MIT license, see LICENSE file for full text

#include "server.h"

#include <zmq.h>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <sys/time.h>

static const int kMsgBufferSize = 250;
static const double kFiltMinCutoff = 3.0;
static const double kFiltBeta = 10.00;

static double sTicks() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (double) tv.tv_usec / 1000000 + tv.tv_sec;
}

Server::Server()
  : xFilt(60.0, kFiltMinCutoff, kFiltBeta, 1.0), yFilt(60.0, kFiltMinCutoff, kFiltBeta, 1.0)
 {
  context = zmq_ctx_new();
  publisher = zmq_socket(context, ZMQ_PUB);
  int rc = zmq_bind(publisher, "tcp://*:5456");
  assert(rc == 0);
}

Server::~Server() {
  zmq_close(publisher);
  zmq_ctx_destroy(context);
}

void Server::send(float x, float y) {
  double t = sTicks();
  char update[kMsgBufferSize];
  snprintf(update, kMsgBufferSize, "{\"x\":%F,\"y\":%F,\"fx\":%F,\"fy\":%F,\"t\":%F}",
    x, y, xFilt(x, t), yFilt(y, t),t);
  printf("%s\n", update);
  zmq_send(publisher, update, strlen(update), 0);
}
