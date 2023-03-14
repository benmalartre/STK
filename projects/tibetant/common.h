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


#endif // TX_COMMON_H