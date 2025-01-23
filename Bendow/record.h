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
    void                    tick(const stk::StkFloat& sample);

    bool                    recording(){return _recording;};

private:
    stk::FileWvOut*         _output;
    bool                    _recording; 
    std::string             _filename;
};

#endif