#include "exports.h"
#include "envelopes.h"
#include "arythmetics.h"
#include "buffers.h"
#include "effects.h"
#include "filters.h"
#include "generators.h"
#include "instruments.h"
#include "nodes.h"
#include "readers.h"

// ----------------------------------------------------------------------
//	STK GENERAL FUNCTIONS
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

void STKNodeSetHasNoEffect(STKNode* node, bool hasnoeffect)
{
  /*
  //--------------------------------------------------------------------
  // STKBuffer Set Has No Effect
  //--------------------------------------------------------------------
  void STKNodeSetHasNoEffect(STKNode* node, bool hasnoeffect)
  {
    if (hasnoeffect != node->m_hasnoeffect)
    {
      if (hasnoeffect)
      {
        node->m_tickCallback = [this](){return this->BufferTickHasNoEffect(); };
      }
      else
      {
        term();
        init();
      }
    }
  }
  */
}

