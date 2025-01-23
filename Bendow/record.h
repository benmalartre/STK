#include <string>
#include <Stk.h>
#include <FileWvOut.h>


#ifndef TX_RECORD_H
#define TX_RECORD_H

class TxRecorder {
public:
    TxRecorder();
    ~TxRecorder();

    void                    setFileName(const char* filename);
    void                    start();
    void                    stop();

    void                    draw();
    inline stk::StkFloat    tick();

private:
    stk::FileWvOut*         _output;
    bool                    _recording; 
    std::string             _filename;
};




/*

output[j] = new FileWvOut(fileName, 1, FileWrite::FILE_WAV );

  // Define and open a 16-bit, four-channel AIFF formatted output file
  try {
    output.openFile( "foursine.aif", 4, FileWrite::FILE_AIF, Stk::STK_SINT16 );
  }
  catch (StkError &) {
    exit( 1 );
  }

  // Write two seconds of four sines to the output file
  StkFrames frames( 88200, 4 );
  for ( i=0; i<4; i++ )
    inputs[i].tick( frames, i );

  output.tick( frames );

  // Now write the first sine to all four channels for two seconds
  for ( i=0; i<88200; i++ ) {
    output.tick( inputs[0].tick() );
  }
*/
#endif