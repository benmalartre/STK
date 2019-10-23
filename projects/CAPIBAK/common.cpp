#include "common.h"
#include "streams.h"

// ----------------------------------------------------------------------
//    STK GENERAL FUNCTIONS
// ----------------------------------------------------------------------
RtAudio* STKInit()
{
    try {
        Stk::setSampleRate(44100.0);
        Stk::setRawwavePath("../../../../rawwaves");
        RtAudio* DAC = new RtAudio();
        return DAC;
    }
    catch (RtAudioError &error) {
        return NULL;
    }
    
}

void STKTerm(RtAudio* DAC)
{
    //DAC->closeStream();
    //delete DAC;
}

void STKSetSampleRate(float _rate)
{
    Stk::setSampleRate(_rate);
}

int STKGetDevices(RtAudio* DAC)
{
    if (DAC != NULL) {
        int nb = DAC->getDeviceCount();
        for (int i = 0; i<nb; i++) {
            RtAudio::DeviceInfo info;
            info = DAC->getDeviceInfo(i);
        }
        return nb;
    }
    else
        return 0;
}

void STKSetNodeHasNoEffect(STKNode* node, bool hasnoeffect)
{
    node->setHasNoEffect(hasnoeffect);
}

StkFloat STKNode::update(StkFloat value)
{
    m_outidx = (m_outidx + 1) % m_noutput;
    return value;
}

void STKSetNodeVolume(STKNode* node, StkFloat volume)
{
    node->setVolume(volume);
}
