#include "sequencer.h"
#include "graph.h"

ImVec2 TxSequencer::Size(520, 300);

TxSequencer::TxSequencer(TxGraph* parent, const std::string& name)
  : TxNode(parent, name, TX_NUM_CHANNELS)
  , _bpm(60)
  , _length(8)
  , _running(false)
{
}

TxSequencer::TxSequencer(TxGraph* parent, const std::string& name, uint32_t bpm, uint64_t length)
  : TxNode(parent, name, TX_NUM_CHANNELS)
  , _bpm(bpm)
  , _length(length)
  , _running(false)
{
  _params.push_back(new TxParameterBool(this, "Running", &_running));
  _params.push_back(new TxParameterInt(this, "Bpm", 1, 440, &_bpm, TxParameter::KNOB));
}

TxSequencer::~TxSequencer()
{

}

void TxSequencer::setLength(uint64_t length)
{
  _length = length;
  _sequence.resize(_length);
}

void TxSequencer::setBPM(uint32_t bpm)
{
  _bpm = bpm;
}

void TxSequencer::setBeat(uint64_t beatIdx, const Beat& beat)
{
  if(_length <= beatIdx) {
    std::cerr << "Invalid beat index : " << beatIdx << 
    " (max = " << (_length - 1) << ")" << std::endl;
    return;
  }
  _sequence[beatIdx] = beat;
}

void TxSequencer::start()
{
  _running = true;
}

void TxSequencer::stop()
{
  _running = false;
}

const ImVec2& TxSequencer::size()
{
  return TxSequencer::Size;
}

TxSequencer::Index TxSequencer::timeToIndex(float time)
{
  const float relativeTime = time / 60.f * (float)_bpm * TxSequencer::NumBits;
  const int beatTime = (int)relativeTime;
  const int bitTime = (int)((float)(relativeTime - beatTime) * TxSequencer::NumBits);
  return {beatTime % _length, bitTime};
}

stk::StkFloat TxSequencer::tick(unsigned int channel)
{
  const TxSequencer::Index index = timeToIndex(TxTime::instance().get());
  const Beat* beat = &_sequence[index.first];
  return channel ? beat->second : BIT_CHECK(beat->first, index.second);
}

stk::StkFrames& TxSequencer::tick(stk::StkFrames& frames, unsigned int channel)
{
  memset(&frames[0], 0, frames.size() * sizeof(stk::StkFloat));
  
  TxSequencer::Index index = timeToIndex(TxTime::instance().get());

/*
  for(auto& track: _tracks) {
    stk::StkFloat* samples = &frames[0];
    stk::StkFloat channelWeights[TX_NUM_CHANNELS];
    for(size_t n = 0; n < TX_NUM_CHANNELS; ++n) {
      channelWeights[n] = track->getNode()->stereoWeight(n);
    }
    //const stk::StkFloat sample = track->tick(timeIdx % _length);
    for(size_t frameIdx = 0; frameIdx < stk::RT_BUFFER_SIZE;  ++frameIdx) {
      for(size_t channelIdx = 0; channelIdx < TX_NUM_CHANNELS; ++channelIdx) {
        *samples++ += track->tick(timeIdx % _length) * channelWeights[channelIdx];
      }
    }
  }
  */

  return frames;
}

bool TxSequencer::drawBeat(uint32_t beatIdx, uint32_t bitIdx, bool current)
{
  bool modified = false;
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  const std::string hiddenPrefix = "##" + std::to_string(beatIdx);

  Beat* beat = &_sequence[beatIdx];
  ImGui::BeginGroup();
  if (ImGui::VSliderFloat((hiddenPrefix + "Slider").c_str(), ImVec2(20, 120),
    &beat->second, 0, 255))modified = true;

  ImGui::BeginGroup();
  
  const ImGuiStyle& style = ImGui::GetStyle();
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 1));
  for(size_t i = 0; i < 4; ++i) {
    bool bit = BIT_CHECK(beat->first, i);
    const ImVec4 btnColor = (current && (i == bitIdx))? 
      (bit ? ImVec4(1, 0, 0, 1): ImVec4(1, 0.75, 0, 1)) 
      : (bit ? ImVec4(1, 0.75, 0, 1) : style.Colors[ImGuiCol_FrameBg]);

    ImGui::PushStyleColor(ImGuiCol_Button, btnColor);
    if (ImGui::Button((hiddenPrefix + "Btn" + std::to_string(i)).c_str(), ImVec2(4, 12))) {
      BIT_FLIP(beat->first, i);
      modified = true;
    }
    ImGui::PopStyleColor();
    if(i < 3)ImGui::SameLine();
  }
  ImGui::PopStyleVar();

  ImGui::EndGroup();

  ImGui::EndGroup();
  return modified;
}

void TxSequencer::_drawImpl(bool* modified)
{  
  TxTime& time = TxTime::instance();
  float t = time.get();

  TxSequencer::Index index = timeToIndex(t);
  ImGui::Checkbox("Running", &_running);
  ImGui::SameLine();
  if(ImGui::Button("Reset Time")) time.reset();
  ImGui::SameLine();
  //if(ImGui::Button("Incr Time")) time.incr100();
  //ImGui::SameLine();
  ImGui::SetNextItemWidth(100);
  ImGui::Text("Time : %fs", t);
  /*
  ImGui::SameLine();
  ImGui::Text("Rate : %fs", time.rate());
  ImGui::SameLine();
  ImGui::Text("Index : %i", index.first);
  */
  TxNode::_drawAlignLeft();
  if (ImGuiKnobs::KnobInt("Bpm", &_bpm, 1, 220, 1, "%ibpm",
    ImGuiKnobVariant_WiperDot, 0.f, ImGuiKnobFlags_DragHorizontal) && modified)* modified = true;
  
  ImGui::SameLine();

  for(size_t i = 0; i < _sequence.size(); ++i) {
    if (drawBeat(i, index.second, (i == index.first)))*modified = true;
    if (i < _sequence.size() - 1)  ImGui::SameLine();
  }

  TxNode::_drawCommonControls();
}

void TxSequencer::reset()
{
  
}