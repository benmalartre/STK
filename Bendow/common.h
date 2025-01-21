
// Dear ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
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

// commons
#define TX_TITLE_HEIGHT 32.f
#define TX_TITLE_X 64.f
#define TX_TITLE_Y 8.f
#define TX_PADDING_X 16.f
#define TX_PADDING_Y 4.f
#define TX_SLIDER_WIDTH 128.f
#define TX_PLUG_WIDTH 12.f
#define TX_PLUG_HEIGHT 12.f
#define TX_PLUG_DETAIL 6.f
#define TX_CONTOUR_WIDTH 6.f
#define TX_CONNEXION_WIDTH 4.f
#define TX_NODE_ROUNDING 2.f
#define TX_PLUG_SIZE 8.f
#define TX_KNOB_SMALL_SIZE 32.f
#define TX_KNOB_MIDDLE_SIZE 48.f
#define TX_KNOB_LARGE_SIZE 64.f
#define TX_SLIDER_SIZE 32.f

// macros
#define REGISTER_NODE(CLASS, ID) \
TxNode* create##CLASS(TxNode* parent, const std::string& name) { \
  return new CLASS(parent, name);\
};

#define DEFINE_NODE(CLASS) \
  create##CLASS


enum TX_COLOR_INDEX {
  TX_COLOR_CONTOUR_ID,
  TX_COLOR_PLUG_ID
};

static ImU32 TX_PLUG_COLOR_DEFAULT = IM_COL32(180,180,180,255);
static ImU32 TX_CONTOUR_COLOR_DEFAULT = IM_COL32(50,50,50, 255);
static ImU32 TX_PLUG_COLOR_SELECTED = IM_COL32(200,200,200,255);
static ImU32 TX_CONTOUR_COLOR_SELECTED = IM_COL32(255,200,100,  255);
static ImU32 TX_PLUG_COLOR_AVAILABLE = IM_COL32(255,200,100,255);

using Beat = std::pair<short, float>;
using Sequence = std::vector<Beat>;
using Index = std::pair<uint32_t, uint32_t>;

static const uint32_t NumBits = 4;

static float computeSampleTime() {
  return stk::RT_BUFFER_SIZE / stk::Stk::sampleRate();
};

static float computeSampleRate() {
  return 1.f / stk::Stk::sampleRate();
};

class TxTime {
public:
  TxTime() : _time(0.f), _rate(computeSampleRate()), _bpm(60) {};
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
  void setBPM(int bpm){_bpm=bpm;};
  double rate() const {return _rate;};
  int bpm() const {return _bpm;};

  Index index(size_t length);
  static TxTime& instance() {
    static TxTime instance;
    return instance;
  }

protected:
  double _time;
  double _rate;
  int _bpm;
};

class TxGraphEditor;
class TxSequencerEditor;

TxGraphEditor* getGraphEditor();
TxSequencerEditor* getSequencerEditor();



#endif // TX_COMMON_H