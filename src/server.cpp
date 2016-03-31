// The SmartHeadTracker Head Tracker
// Copyright (C) 2016  Tristan Hume
// Released under MIT license, see LICENSE file for full text

#include "server.h"

#include <zmq.h>
#include <cassert>
#include <cstring>
#include <cstdio>

static const int kMsgBufferSize = 100;

Server::Server() {
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
  char update[kMsgBufferSize];
  snprintf(update, kMsgBufferSize, "{\"x\":%F,\"y\":%F}", x, y);
  printf("%s\n", update);
  zmq_send(publisher, update, strlen(update), 0);
}
