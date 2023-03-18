CMAKE_MINIMUM_REQUIRED (VERSION 3.0.0)
PROJECT (Bendow)

# Require C++14
SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)
if(WIN32)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:inline-")
endif()

# glfw
if(NOT DEFINED GLFW_DIR)
	MESSAGE(FATAL_ERROR "No GLFW_DIR variable set! GLFW path must be passed to cmake!!!")
endif()
SET(GLFW_INCLUDE_DIR "${GLFW_DIR}/include")
FIND_LIBRARY(GLFW NAMES glfw3 PATHS "${GLFW_DIR}/build/src" "${GLFW_DIR}/build/src/Release")

# stk 
if(NOT DEFINED STK_DIR)
	MESSAGE(FATAL_ERROR "No STK_DIR variable set! STK path must be passed to cmake!!!")
endif()

# OpenGL
FIND_PACKAGE(OpenGL REQUIRED)

# system
IF(APPLE)
  SET(ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES} 
    -lm 
	-framework CoreFoundation 
	-framework AudioToolbox 
	-framework AudioUnit 
	-framework CoreAudio 
	-framework CoreAudioKit
	-framework CoreMIDI
	-framework CoreMIDIServer
	-framework Cocoa
	-framework IOKit
	-framework OpenGL
  )
ELSEIF(WIN32)
 SET(ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES} 
    dsound
  )
ENDIF(APPLE)

IF(APPLE)
  ADD_COMPILE_DEFINITIONS(
    __MACOSX_CORE__
	D__LITTLE_ENDIAN__
  )
ELSEIF(WIN32)
  ADD_COMPILE_DEFINITIONS(
    __WINDOWS_DS__ 
	__LITTLE_ENDIAN__ 
	WIN32_LEAN_AND_MEAN
  )
  ADD_DEFINITIONS("/wd4244")
  ADD_DEFINITIONS("/wd4305")
ENDIF()

SET(SOURCES 
	imgui/imgui.cpp
	imgui/imgui_stdlib.cpp
	imgui/imgui_impl_glfw.cpp
	imgui/imgui_impl_opengl3.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_widgets.cpp
	imgui/imgui_tables.cpp
	imgui/imgui_demo.cpp
	imgui/imgui_knobs.cpp
	imgui/imgui_plot.cpp
	imgui/implot.cpp
	imgui/implot_items.cpp
	imgui/implot_demo.cpp
	${STK_DIR}/src/Stk.cpp
	${STK_DIR}/src/BeeThree.cpp
	${STK_DIR}/src/Noise.cpp
	${STK_DIR}/src/Blit.cpp
	${STK_DIR}/src/BlitSaw.cpp
	${STK_DIR}/src/BlitSquare.cpp
	${STK_DIR}/src/Modulate.cpp
	${STK_DIR}/src/Granulate.cpp
	${STK_DIR}/src/FileRead.cpp
	${STK_DIR}/src/FileWvIn.cpp
	${STK_DIR}/src/FileLoop.cpp
	${STK_DIR}/src/ADSR.cpp
	${STK_DIR}/src/Chorus.cpp
	${STK_DIR}/src/Echo.cpp
	${STK_DIR}/src/Fir.cpp
	${STK_DIR}/src/Delay.cpp
	${STK_DIR}/src/DelayA.cpp
	${STK_DIR}/src/DelayL.cpp
	${STK_DIR}/src/SineWave.cpp
	${STK_DIR}/src/SingWave.cpp
	${STK_DIR}/src/TwoZero.cpp
	${STK_DIR}/src/OneZero.cpp
	${STK_DIR}/src/OnePole.cpp
	${STK_DIR}/src/Envelope.cpp
	${STK_DIR}/src/Twang.cpp
	${STK_DIR}/src/Plucked.cpp
	${STK_DIR}/src/RtAudio.cpp
	${STK_DIR}/src/FM.cpp
	parameter.cpp
	buffer.cpp
	node.cpp
	value.cpp
	random.cpp
	oscillator.cpp
	effect.cpp
	filter.cpp
	arythmetic.cpp
	lfo.cpp
	adsr.cpp
	sequencer.cpp
	graph.cpp
	main.cpp
)

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES})

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} 
  PUBLIC
    ${GLFW_INCLUDE_DIR}
    ${STK_DIR}/include
	imgui
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	${OPENGL_gl_LIBRARY}
	${GLFW}
	${ADDITIONAL_LIBRARIES}
)

ADD_COMPILE_OPTIONS(/C4244)
