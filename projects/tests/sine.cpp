
// sineosc.cpp
#include "FileLoop.h"
#include "FileWvOut.h"
using namespace stk;
int main()
{
    // Set the global sample rate before creating class instances.
    Stk::setSampleRate( 44100.f );
    FileLoop input;
    FileWvOut output;
    // Load the sine wave file.
    //input.openFile( "../../rawwaves/sinewave.raw", true );
    input.openFile("/Users/benmalartre/Documents/RnD/PureBasic/Noodle/waves/Generic Tones/400Hz.wav", false);
    // Open a 16-bit, one-channel WAV formatted output file
    output.openFile( "test.wav", 2, FileWrite::FILE_WAV, Stk::STK_SINT8 );
    //input.setRate(16000.f/44100.f);
    //input.setRate(input.getFileRate()/ Stk::sampleRate());
    //input.setFrequency( 400.f);
    // Run the oscillator for 40000 samples, writing to the output file
    for ( int i=0; i<40000; i++ )
        output.tick( input.tick() );
    return 0;
}
