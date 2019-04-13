// foursine.cpp STK tutorial program
#include "Stk.h"
#include "BeeThree.h"
#include "Messager.h"
#include "Voicer.h"
#include "SKINImsg.h"
#include <algorithm>
#include "RtAudio.h"
using std::min;
using namespace stk;

#include "SineWave.h"
#include "Blit.h"
#include "BlitSquare.h"
#include "BlitSaw.h"
#include "Asymp.h"
#include "FileWvOut.h"
#include <cstdlib>
using namespace stk;

#define C0	16.35
#define Db0 17.32
#define D0	18.35
#define Eb0 19.45
#define E0	20.60
#define F0	21.83
#define Gb0 23.12
#define G0	24.50
#define Ab0 25.96
#define A0	27.50
#define Bb0 29.14
#define B0	30.87

#define C1	32.70
#define Db1 34.65	
#define D1	36.71
#define Eb1 38.89	
#define E1	41.20
#define F1	43.65
#define Gb1 46.25	
#define G1	49.00
#define Ab1 51.91	
#define A1	55.00
#define Bb1 58.27	
#define B1	61.74

#define C2  65.41
#define Db2 69.30
#define D2	73.42
#define Eb2	77.78
#define E2  82.41
#define F2	87.31
#define Gb2 92.50
#define G2	98.00
#define Ab2 103.83
#define A2  110.00
#define Bb2 116.54
#define B2  123.47

#define C3	130.81
#define Db3 138.59
#define D3	146.83
#define Eb3 155.56
#define E3	164.81
#define F3	174.61
#define Gb3 185.00
#define G3	196.00
#define Ab3 207.65
#define A3	220.00
#define Bb3 233.08
#define B3	246.94

#define C4	261.63
#define Db4 277.18
#define D4	293.66
#define Eb4 311.13
#define E4	329.63
#define F4	349.23
#define Gb4 369.99
#define G4	392.00
#define Ab4 415.30
#define A4	440.00
#define Bb4 466.16
#define B4	493.88

#define C5	523.25
#define Db5 554.37
#define D5	587.33
#define Eb5 622.25
#define E5	659.25
#define F5	698.46
#define Gb5 739.99
#define G5	783.99
#define Ab5 830.61
#define A5	880.00
#define Bb5 932.33
#define B5	987.77

#define C6	1046.50
#define Db6 1108.73
#define D6	1174.66
#define Eb6 1244.51
#define E6	1318.51
#define F6	1396.91
#define Gb6 1479.98
#define G6	1567.98
#define Ab6 1661.22
#define A6	1760.00
#define Bb6 1864.66
#define B6	1975.53

#define C7	2093.00
#define Db7 2217.46
#define D7	2349.32
#define Eb7 2489.02
#define E7	2637.02
#define F7	2793.83
#define Gb7 2959.96
#define G7	3135.96
#define Ab7 3322.44
#define A7	3520.00
#define Bb7 3729.31
#define B7	3951.07

#define C8	4186.01
#define Db8 4434.92
#define D8	4698.63
#define Eb8 4978.03
#define E8	5274.04
#define F8	5587.65
#define Gb8 5919.91
#define G8	6271.93
#define Ab8 6644.88
#define A8	7040.00
#define Bb8 7458.62
#define B8	7902.13


#define NUM_NOTES 12
#define NUM_OCTAVES 9

StkFloat NOTES[NUM_OCTAVES][NUM_NOTES] = { 
	{ C0, Db0, D0, Eb0, E0, F0, Gb0, G0, Ab0, A0, Bb0, B0 },
	{ C1, Db1, D1, Eb1, E1, F1, Gb1, G1, Ab1, A1, Bb1, B1 },
	{ C2, Db2, D2, Eb2, E2, F2, Gb2, G2, Ab2, A2, Bb2, B2 },
	{ C3, Db3, D3, Eb3, E3, F3, Gb3, G3, Ab3, A3, Bb3, B3 },
	{ C4, Db4, D4, Eb4, E4, F4, Gb4, G4, Ab4, A4, Bb4, B4 },
	{ C5, Db5, D5, Eb5, E5, F5, Gb5, G5, Ab5, A5, Bb5, B5 },
	{ C6, Db6 ,D6, Eb6, E6, F6, Gb6, G6, Ab6, A6, Bb6, B6 },
	{ C7, Db7, D7, Eb7, E7, F7, Gb7, G7, Ab7, A7, Bb7, B7 }, 
	{ C8, Db8, D8, Eb8, E8, F8, Gb8, G8, Ab8, A8, Bb8, B8 } 
};

/*
int currentNote = 0;
int currentOctave = 3;
int main()
{
	// Set the global sample rate before creating class instances.
	Stk::setSampleRate(44100.0);
	int i;
	FileWvOut output;
	SineWave lfo;
	lfo.setFrequency(3.0f);
	SineWave generator;
	// Set the sine wave frequency.
	generator.setFrequency(64.0);
	// Define and open a 16-bit, four-channel AIFF formatted output file
	try {
		output.openFile("onesine.wav", 1, FileWrite::FILE_WAV, Stk::STK_SINT16);
	}
	catch (StkError &) {
		exit(1);
	}
	// Write two seconds of four sines to the output file
	StkFrames frames(882000, 1);

	// Now write the first sine to all four channels for two seconds
	for (i = 0; i<882000; i++) {
		output.tick(generator.tick() + lfo.tick() *0.25f);
		if (i % 22050 == 0){
			currentNote++;
			if (currentNote >= NUM_NOTES){
				currentNote = 0; currentOctave++;
			}
			generator.setFrequency(NOTES[currentOctave][currentNote]);
		}
	}
	output.closeFile();
	return 0;
}
*/

// The TickData structure holds all the class instances and data that
// are shared by the various processing functions.
struct TickData {
	Voicer voicer;
	Messager messager;
	Skini::Message message;
	int counter;
	bool haveMessage;
	bool done;
	// Default constructor.
	TickData()
		: counter(0), haveMessage(false), done(false) {}
};
#define DELTA_CONTROL_TICKS 64 // default sample frames between control input checks
// The processMessage() function encapsulates the handling of control
// messages.  It can be easily relocated within a program structure
// depending on the desired scheduling scheme.
void processMessage(TickData* data)
{
	register StkFloat value1 = data->message.floatValues[0];
	register StkFloat value2 = data->message.floatValues[1];
	switch (data->message.type) {
	case __SK_Exit_:
		data->done = true;
		return;
	case __SK_NoteOn_:
		std::cout << "RECIEVED NOTE ON..." << value2 << std::endl;
		if (value2 == 0.0) // velocity is zero ... really a NoteOff
			data->voicer.noteOff(value1, 64.0);
		else { // a NoteOn
			data->voicer.noteOn(value1, value2);
		}
		break;
	case __SK_NoteOff_:
		data->voicer.noteOff(value1, value2);
		break;
	case __SK_ControlChange_:
		data->voicer.controlChange((int)value1, value2);
		break;
	case __SK_AfterTouch_:
		data->voicer.controlChange(128, value1);
	case __SK_PitchChange_:
		data->voicer.setFrequency(value1);
		break;
	case __SK_PitchBend_:
		data->voicer.pitchBend(value1);
	} // end of switch
	data->haveMessage = false;
	return;
}
// This tick() function handles sample computation and scheduling of
// control updates.  It will be called automatically when the system
// needs a new buffer of audio samples.
int tick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *dataPointer)
{
	TickData *data = (TickData *)dataPointer;
	register StkFloat *samples = (StkFloat *)outputBuffer;
	int counter, nTicks = (int)nBufferFrames;
	while (nTicks > 0 && !data->done) {
		if (!data->haveMessage) {
			data->messager.popMessage(data->message);
			if (data->message.type > 0) {
				data->counter = (long)(data->message.time * Stk::sampleRate());
				data->haveMessage = true;
				std::cout << "GOT MESSAGE :)" << std::endl;
			}
			else
				data->counter = DELTA_CONTROL_TICKS;
		}
		counter = min(nTicks, data->counter);
		data->counter -= counter;
		for (int i = 0; i<counter; i++) {
			*samples++ = data->voicer.tick();
			nTicks--;
		}
		if (nTicks == 0) break;
		// Process control messages.
		if (data->haveMessage) processMessage(data);
	}
	return 0;
}
int main()
{
	// Set the global sample rate and rawwave path before creating class instances.
	Stk::setSampleRate(44100.0);
	Stk::setRawwavePath("../../../../rawwaves/");
	int i;
	TickData data;
	RtAudio dac;
	Instrmnt *instrument[3];
	for (i = 0; i<3; i++) instrument[i] = 0;
	// Figure out how many bytes in an StkFloat and setup the RtAudio stream.
	RtAudio::StreamParameters parameters;
	parameters.deviceId = dac.getDefaultOutputDevice();
	parameters.nChannels = 1;
	RtAudioFormat format = (sizeof(StkFloat) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
	unsigned int bufferFrames = RT_BUFFER_SIZE;
	try {
		dac.openStream(&parameters, NULL, format, (unsigned int)Stk::sampleRate(), &bufferFrames, &tick, (void *)&data);
	}
	catch (RtAudioError &error) {
		error.printMessage();
		goto cleanup;
	}
	try {
		// Define and load the BeeThree instruments
		for (i = 0; i<3; i++)
			instrument[i] = new BeeThree();
	}
	catch (StkError &) {
		goto cleanup;
	}
	// "Add" the instruments to the voicer.
	for (i = 0; i<3; i++)
		data.voicer.addInstrument(instrument[i]);
	if (data.messager.startStdInput() == false)
		goto cleanup;
	try {
		dac.startStream();
	}
	catch (RtAudioError &error) {
		error.printMessage();
		goto cleanup;
	}
	// Block waiting until callback signals done.
	while (!data.done)
		Stk::sleep(100);

	// Shut down the callback and output stream.
	try {
		dac.closeStream();
	}
	catch (RtAudioError &error) {
		error.printMessage();
	}
cleanup:
	for (i = 0; i<3; i++) delete instrument[i];
	return 0;
}
