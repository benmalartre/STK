CMAKE_MINIMUM_REQUIRED (VERSION 3.0.0)
PROJECT (STK)

# Require C++14
SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)
IF(WIN32)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:inline-")
ENDIF()


# stk
SET(STK_DIR "../stk")

# system
IF(APPLE)
  SET(ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES} 
    "-lm" 
    "-framework CoreFoundation"
    "-framework AudioToolbox" 
    "-framework AudioUnit" 
    "-framework CoreAudio" 
    "-framework CoreAudioKit"
    "-framework CoreMIDI"
    "-framework CoreMIDIServer"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework OpenGL"
  )
ELSEIF(WIN32)
 SET(ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES} 
    dsound
	Ws2_32
  )
ENDIF(APPLE)

IF(APPLE)
  ADD_COMPILE_DEFINITIONS(
    __MACOSX_CORE__
    __LITTLE_ENDIAN__
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
	${STK_DIR}/src/BiQuad.cpp
	${STK_DIR}/src/TwoZero.cpp
	${STK_DIR}/src/OneZero.cpp
	${STK_DIR}/src/OnePole.cpp
	${STK_DIR}/src/Envelope.cpp
	${STK_DIR}/src/Twang.cpp
	${STK_DIR}/src/Plucked.cpp
	${STK_DIR}/src/RtAudio.cpp
	${STK_DIR}/src/FM.cpp

	${STK_DIR}/src/ADSR.cpp
	${STK_DIR}/src/Asymp.cpp
	${STK_DIR}/src/BandedWG.cpp
	${STK_DIR}/src/BeeThree.cpp
	${STK_DIR}/src/BiQuad.cpp
	${STK_DIR}/src/Blit.cpp
	${STK_DIR}/src/BlitSaw.cpp
	${STK_DIR}/src/BlitSquare.cpp
	${STK_DIR}/src/BlowBotl.cpp
	${STK_DIR}/src/BlowHole.cpp
	${STK_DIR}/src/Bowed.cpp
	${STK_DIR}/src/Brass.cpp
	${STK_DIR}/src/Chorus.cpp
	${STK_DIR}/src/Clarinet.cpp
	${STK_DIR}/src/Delay.cpp
	${STK_DIR}/src/DelayA.cpp
	${STK_DIR}/src/DelayL.cpp
	${STK_DIR}/src/Drummer.cpp
	${STK_DIR}/src/Echo.cpp
	${STK_DIR}/src/Envelope.cpp
	${STK_DIR}/src/FileLoop.cpp
	${STK_DIR}/src/FileRead.cpp
	${STK_DIR}/src/FileWrite.cpp
	${STK_DIR}/src/FileWvIn.cpp
	${STK_DIR}/src/FileWvOut.cpp
	${STK_DIR}/src/Fir.cpp
	${STK_DIR}/src/Flute.cpp
	${STK_DIR}/src/FM.cpp
	${STK_DIR}/src/FMVoices.cpp
	${STK_DIR}/src/FormSwep.cpp
	${STK_DIR}/src/FreeVerb.cpp
	${STK_DIR}/src/Granulate.cpp
	${STK_DIR}/src/Guitar.cpp
	${STK_DIR}/src/FMVoices.cpp
	${STK_DIR}/src/InetWvIn.cpp
	${STK_DIR}/src/InetWvOut.cpp
	${STK_DIR}/src/JCRev.cpp
	${STK_DIR}/src/LentPitShift.cpp
	${STK_DIR}/src/Mandolin.cpp
	${STK_DIR}/src/Mesh2D.cpp
	${STK_DIR}/src/Messager.cpp
	${STK_DIR}/src/MidiFileIn.cpp
	${STK_DIR}/src/Modal.cpp
	${STK_DIR}/src/ModalBar.cpp
	${STK_DIR}/src/Modulate.cpp
	${STK_DIR}/src/Moog.cpp
	${STK_DIR}/src/Mutex.cpp
	${STK_DIR}/src/Noise.cpp
	${STK_DIR}/src/NRev.cpp
	${STK_DIR}/src/OnePole.cpp
	${STK_DIR}/src/OneZero.cpp
	${STK_DIR}/src/PercFlut.cpp
	${STK_DIR}/src/Phonemes.cpp
	${STK_DIR}/src/PitShift.cpp
	${STK_DIR}/src/Plucked.cpp
	${STK_DIR}/src/PoleZero.cpp
	${STK_DIR}/src/PRCRev.cpp
	${STK_DIR}/src/Resonate.cpp
	${STK_DIR}/src/Rhodey.cpp
	${STK_DIR}/src/RtAudio.cpp
	${STK_DIR}/src/RtMidi.cpp
	${STK_DIR}/src/RtWvIn.cpp
	${STK_DIR}/src/RtWvOut.cpp
	${STK_DIR}/src/Sampler.cpp
	${STK_DIR}/src/Simple.cpp
	${STK_DIR}/src/Sphere.cpp
	${STK_DIR}/src/SineWave.cpp
	${STK_DIR}/src/SingWave.cpp
	${STK_DIR}/src/Sitar.cpp
	${STK_DIR}/src/Skini.cpp
	${STK_DIR}/src/Socket.cpp
	${STK_DIR}/src/Stk.cpp
	${STK_DIR}/src/TapDelay.cpp
	${STK_DIR}/src/TcpClient.cpp
	${STK_DIR}/src/TcpServer.cpp
	${STK_DIR}/src/Thread.cpp
	${STK_DIR}/src/TubeBell.cpp
	${STK_DIR}/src/Twang.cpp
	${STK_DIR}/src/TwoPole.cpp
	${STK_DIR}/src/TwoZero.cpp
	${STK_DIR}/src/UdpSocket.cpp
	${STK_DIR}/src/Voicer.cpp
	${STK_DIR}/src/VoicForm.cpp
	${STK_DIR}/src/Whistle.cpp
	${STK_DIR}/src/Wurley.cpp

	nodes.cpp
	arythmetics.cpp
	buffers.cpp
	generators.cpp
	readers.cpp
	streams.cpp
	effects.cpp
	envelopes.cpp
	filters.cpp
	writers.cpp
	exports.cpp
)

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCES})

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} 
  PUBLIC
    ${STK_DIR}/include
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	${ADDITIONAL_LIBRARIES}
)
