#include "common.h"
#include "arythmetic.h"
#include "graph.h"
#include "editor.h"

ImVec2 TxArythmetic::Size(300, 100);

const char* TxArythmetic::ModeName[TxArythmetic::NumMode] = {
  "Add",
  "Subtract",
  "Multiply",
  "Divide",
  "Mix"
};

TxArythmetic::TxArythmetic(TxNode* parent, const std::string& name) 
  : TxNode(parent, TxNode::ARYTHMETIC, name)
  , _mode(ADD)
{
  _params.push_back(new TxParameterEnum(this, "Mode", &TxArythmetic::ModeName[0], 
    TxArythmetic::NumMode, &_mode));  
  _params.push_back(new TxParameterSamples(this, "Input1", false, 1));
  _params.push_back(new TxParameterSamples(this, "Input2", false, 1));
  ((TxParameterSamples*)_params.back())->setIndex(1);
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
  const stk::StkFloat input1 = _params[INPUT1]->tick();
  const stk::StkFloat input2 = _params[INPUT2]->tick();
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

void TxArythmetic::_drawImpl(TxEditor* editor, bool* modified)
{
  TxParameter* input1 = _params[INPUT1];
  input1->draw(editor);
  TxParameter* input2 = _params[INPUT2];
  input2->draw(editor);
  TxNode::_drawAlignLeft(editor);
  TxNode::_drawAlignTop(editor);
  ImGui::BeginGroup();
  ImGui::SetNextItemWidth(TX_SLIDER_WIDTH * editor->scale());
  TxParameter* mode = _params[TxArythmetic::MODE];
  if(mode->draw(editor) && modified)*modified = true;
  if (mode->tick() == MIX) {
    ImGui::SetNextItemWidth(128);
    TxParameter* float1 = _params[TxArythmetic::FLOAT1];
    if(float1->draw(editor) && modified)*modified = true;
  }
  ImGui::EndGroup();

  TxNode::_drawOutput(editor);
}

void TxArythmetic::reset()
{
}