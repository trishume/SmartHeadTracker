// The SmartHeadTracker Head Tracker
// Copyright (C) 2016  Tristan Hume
// Released under MIT license, see LICENSE file for full text

#include "halideFuncs.h"

#include "Halide.h"
using namespace Halide;


// template <class T> class MyGenerator : public Halide::Generator<T> {
// };

class ColourMatchGenerator {
public:
  static const int kFixedMult = 10000;
  static const int kThresh = 70;
  ImageParam input{UInt(8), 3, "input"};
  Var x, y, c, x2, y2;

  Func build() {
    Tuple target = Tuple(cast<int>(0.35f*kFixedMult),cast<int>(0.4f*kFixedMult));
    Tuple target2 = Tuple(cast<int>(0.43f*kFixedMult),cast<int>(0.55f*kFixedMult));

    Func vecDist;
    Expr xDist = x - x2;
    Expr yDist = y - y2;
    vecDist(x, y, x2, y2) = xDist*xDist + yDist*yDist;

    Func p;
    p(x, y, c) = Halide::cast<int>(input(x,y,c));

    Tuple normalized = Tuple(p(x,y,0)*kFixedMult/(p(x,y,2)+1), p(x,y,1)*kFixedMult/(p(x,y,2)+1));
    Expr dist1 = vecDist(normalized[0],normalized[1],target[0],target[1]);
    Expr dist2 = vecDist(normalized[0],normalized[1],target2[0],target2[1]);
    Expr dist = min(dist1, dist2);

    Func out;
    out(x, y) = select(
      dist < kThresh*kFixedMult && input(x,y,2) > 160, Halide::cast<uint8_t>(255),
      Halide::cast<uint8_t>(0));

    out.vectorize(x, 4).parallel(y);

    // Deal with interleaved format of OpenCV
    input.set_stride(0, 3) // stride in dimension 0 (x) is three
         .set_stride(2, 1); // stride in dimension 2 (c) is one
    input.set_bounds(2, 0, 3);

    return out;
  }
};

struct HalideGens {
  ColourMatchGenerator colourMatchGen;
  Func colourMatchFunc;

  HalideGens() {
    colourMatchFunc = colourMatchGen.build();
    colourMatchFunc.compile_jit();
  }
};

HalideGens *createGens() {
  return new HalideGens();
}
void deleteGens(HalideGens *gens) {
  delete gens;
}

// Convert from OpenCV cv::Mat to Halide Buffer
static cv::Mat runFunc(Func &f, ImageParam &inParam, cv::Mat &m, cv::Size outSize) {
  assert(m.isContinuous());

  buffer_t buf = {0};
  buf.host = m.ptr();
  buf.elem_size = m.elemSize()/m.channels();
  buf.extent[0] = m.cols;
  buf.extent[1] = m.rows;
  buf.extent[2] = (m.channels() == 1) ? 0 : m.channels();

  buf.stride[0] = m.channels();
  buf.stride[1] = m.channels()*m.cols;
  buf.stride[2] = 1;

  inParam.set(Buffer(UInt((m.elemSize()/m.channels())*8), &buf, "cv::Mat"));

  cv::Mat out(outSize, CV_8UC1);
  Image<uint8_t> output(Buffer(UInt(8), out.cols, out.rows, 0, 0, out.ptr()));
  f.realize(output);

  return out;
}

cv::Mat colourMatchMask(HalideGens *gens, cv::Mat &m) {
  assert(m.type() == CV_8UC3);
  return runFunc(gens->colourMatchFunc, gens->colourMatchGen.input, m, m.size());
}
