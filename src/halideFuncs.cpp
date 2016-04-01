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
  static const uint32_t kFixedMult = 10000;
  static const uint32_t kThresh = 70;
  ImageParam input{UInt(8), 3, "input"};
  Var x, y, c;

  Func build() {
    Tuple target = Tuple(0.35f*kFixedMult,0.4f*kFixedMult);

    // Expr normalized =

    Func out;
    out(x, y) = select(cast<uint32_t>(input(x,y,2))*Expr(kFixedMult) > Expr(kThresh*kFixedMult), Halide::cast<uint8_t>(255), Halide::cast<uint8_t>(0));

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
