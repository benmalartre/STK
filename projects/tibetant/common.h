// GLFW
#include <GLFW/glfw3.h>

// Dear ImGui
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_plot.h>
#include <imgui/imgui_knobs.h>

// Standard Template Library
#include <iostream>

// Synthesis Tool Kit
#include <RtAudio.h>
#include <Stk.h>

#ifndef TX_COMMON_H
#define TX_COMMON_H

#define TX_NUM_CHANNELS 2
 
static float computeSampleTime() {
  return stk::RT_BUFFER_SIZE / stk::Stk::sampleRate();
};


#endif // TX_COMMON_H