#include "common.h"
#include "arythmetic.h"

ImVec2 TxArythmetic::Size(512, 128);

const char* TxArythmetic::ModeName[TxArythmetic::NumMode] = {
  "Add",
  "Subtract",
  "Multiply",
  "Divide",
  "Mix"
};

TxArythmetic::TxArythmetic(TxGraph* parent, const std::string& name) 
  : TxNode(parent, name)
  , _mode(ADD)
{
  _params.push_back(new TxParameterEnum(this, "Mode", &TxArythmetic::ModeName[0], 
    TxArythmetic::NumMode, &_mode));  
  _params.push_back(new TxParameterSamples(this, "Input1", false, 1));
  _params.push_back(new TxParameterSamples(this, "Input2", false, 1));
  _params.push_back(new TxParameterFloat(this, "Float1", 0.f, 1.f, &_float1, TxParameterFloat::HORIZONTAL));
  _params.back()->setLabel("Blend");
}

TxArythmetic::~TxArythmetic() 
{
}

const ImVec2& TxArythmetic::size()
{
  return TxArythmetic::Size;
}

stk::StkFloat TxArythmetic::tick(unsigned int)
{
  const stk::StkFloat input1 = _params[INPUTS]->tick(0);
  const stk::StkFloat input2 = _params[INPUTS]->tick(1);
  const stk::StkFloat float1 = _params[FLOAT1]->tick();
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
      sample = (1.f - float1) * input1 + float1 * input2;
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

void TxArythmetic::_drawImpl(bool* modified)
{
  TxParameter* inputs = _params[INPUTS];
  inputs->draw();
  ImGui::SameLine();
  ImGui::BeginGroup();
  ImGui::SetNextItemWidth(128);
  TxParameter* mode = _params[TxArythmetic::MODE];
  if(mode->draw() && modified)*modified = true;
  if (mode->tick() == MIX) {
    ImGui::SetNextItemWidth(128);
    TxParameter* float1 = _params[TxArythmetic::FLOAT1];
    if(float1->draw() && modified)*modified = true;
  }

  ImGui::EndGroup();

  ImGui::SameLine();
  ImGui::Dummy(ImVec2(20, 100));
  ImGui::SameLine();
  TxNode::_drawCommonControls();
}

void TxArythmetic::reset()
{
}