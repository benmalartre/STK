
// Dear ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_plot.h>
#include <imgui_knobs.h>
#include <implot.h>


// Standard Template Library
#include <iostream>

// Synthesis Tool Kit
#include <RtAudio.h>
#include <Stk.h>

#ifndef TX_COMMON_H
#define TX_COMMON_H

#define TX_NUM_CHANNELS 2

#define RANDOM_0_1 ((float)rand() / (float)RAND_MAX)

#define RANDOM_0_X(HI) ((float)rand() / (float) RAND_MAX * (HI))

#define RANDOM_LO_HI(LO, HI) ((LO) + (float)rand() / \
  (float)(RAND_MAX / ((HI) - (LO))))

// a=target variable, b=bit number to act upon 0-n 
#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))

// x=target variable, y=mask
#define BITMASK_SET(x,y) ((x) |= (y))
#define BITMASK_CLEAR(x,y) ((x) &= (~(y)))
#define BITMASK_FLIP(x,y) ((x) ^= (y))
#define BITMASK_CHECK(x,y) (((x) & (y)) == (y))
 
static float computeSampleTime() {
  return stk::RT_BUFFER_SIZE / stk::Stk::sampleRate();
};

static float computeSampleRate() {
  return 1.f / stk::Stk::sampleRate();
};

class TxTime {
public:
  TxTime() : _time(0.f), _rate(computeSampleRate()) {};
  ~TxTime() {};

  TxTime(TxTime &other) = delete;
  void operator=(const TxTime &) = delete;

  void reset() {
    _time = 0.f; 
    _rate = computeSampleRate();
  };
  void increment() {_time += _rate/* * stk::RT_BUFFER_SIZE*/;};
  void incr100() {_time += 100.f;};
  double get() const {return _time;};
  void set(float t) {_time = t;};
  double rate() const {return _rate;};
  static TxTime& instance() {
    static TxTime instance;
    return instance;
  }

protected:
  double _time;
  double _rate;
};

#endif // TX_COMMON_H