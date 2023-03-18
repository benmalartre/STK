#include "common.h"
#include "arythmetic.h"

const char* TxArythmetic::ModeName[TxArythmetic::NumMode] = {
  "Add",
  "Subtract",
  "Multiply",
  "Divide"
};

TxArythmetic::TxArythmetic(const std::string& name) 
  : TxNode(name)
  , _mode(ADD)
{
  _params.push_back(new TxParameterEnum("Mode", &TxArythmetic::ModeName[0], 
    TxArythmetic::NumMode, &_mode));  
  _params.push_back(new TxParameterSamples("Input1"));
  _params.push_back(new TxParameterSamples("Input2"));
}

TxArythmetic::~TxArythmetic() 
{
}

stk::StkFloat TxArythmetic::tick(unsigned int)
{
  const stk::StkFloat input1 = _params[INPUT1]->tick();
  const stk::StkFloat input2 = _params[INPUT2]->tick();
  stk::StkFloat sample = 0.f;
  switch(_mode) {
    case ADD:
      sample = input1 + input2;
      break;
    
    case SUB:
      sample = input1 - input2;
      break;

    case MUL:
      sample = input1 * input2;
      break;
    
    case DIV:
      sample = input1 / input2;
      break;

    case MIX:
      sample = (input1 + input2) * 0.5f;
      break;
  }
  _buffer.write(sample);
  return sample;
}

stk::StkFrames& TxArythmetic::tick(stk::StkFrames& frames, unsigned int channel)
{
  if(_dirty) {
    stk::StkFloat* samples = &frames[0];
    for(size_t f = 0; f < frames.size(); ++f) {
        //*samples *= _amplitude;
        //*samples++ += _offset;
    }
    //_dirty = false;
  }
  return frames;
}

void TxArythmetic::setMode(int mode)
{
  _mode = mode;
}

void TxArythmetic::draw()
{
  ImGui::Begin(_name.c_str(), NULL);

  ImGui::BeginGroup();
  ImGui::SetNextItemWidth(128);
  TxParameter* mode = _params[TxArythmetic::MODE];
  mode->draw();
  ImGui::EndGroup();

  ImGui::SameLine();
  ImGui::Dummy(ImVec2(20, 100));
  ImGui::SameLine();
  commonControls();
  ImGui::End();
}

void TxArythmetic::reset()
{
}