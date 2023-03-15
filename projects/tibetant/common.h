// GLFW
#include <GLFW/glfw3.h>

// Dear ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_plot.h>
#include <imgui/imgui_knobs.h>
#include <imgui/implot.h>


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
 
static float computeSampleTime() {
  return stk::RT_BUFFER_SIZE / stk::Stk::sampleRate();
};

static float computeSampleRate() {
  return 1.f / stk::Stk::sampleRate();
};

class TxTime {
public:
  TxTime() : _time(0.f), _rate((1.f / stk::Stk::sampleRate())) {};
  ~TxTime() {};

  TxTime(TxTime &other) = delete;
  void operator=(const TxTime &) = delete;

  void reset() {
    _time = 0.f; 
    _rate = (1.f / stk::Stk::sampleRate());
  };
  void increment() {_time += _rate * stk::RT_BUFFER_SIZE;};
  void incr100() {_time += 100.f;};
  float get() {return _time;};
  void set(float t) {_time = t;};
  float rate() {return _rate;};
  static TxTime& instance() {
    static TxTime instance;
    return instance;
  }

protected:
  float _time;
  float _rate;
};

#endif // TX_COMMON_H