#include "node.h"
#include "sequencer.h"
#include "graph.h"
#include "factory.h"

const int TxSequencer::Flags =
ImGuiWindowFlags_NoResize |
ImGuiWindowFlags_NoCollapse |
ImGuiWindowFlags_NoMove |
ImGuiWindowFlags_NoNav |
ImGuiWindowFlags_NoBackground |
ImGuiWindowFlags_NoTitleBar;

void updateBPM(TxSequencer* sequencer)
{
  TxTime& time = TxTime::instance();
  time.setBPM(sequencer->bpm());
}

TxSequencer::TxSequencer(uint32_t numTracks, uint32_t bpm, uint64_t length)
  : TxNode(NULL, TxNode::SEQUENCER, "TxSequencer", TX_NUM_CHANNELS)
  , _bpm(bpm)
  , _length(length)
  , _volume(1.f)
  , _running(false)
{
  TxTime& time = TxTime::instance();
  time.setBPM(_bpm);
  setLength(length);
  _params.push_back(new TxParameterBool(this, "Running", &_running));
  _params.push_back(new TxParameterInt(this, "Bpm", 1, 440, &_bpm, TxParameter::KNOB));
  _params.back()->setCallback(new TxCallback((CALLBACK_FN)&updateBPM, this));
  _params.push_back(new TxParameterFloat(this, "Volume", 0.f, 2.f, &_volume));
  _params.push_back(new TxParameterFloat(this, "Time", -10000.f, 10000.f, &_time, TxParameter::FLOAT));

  _size = ImVec2(200, 200);

  _tracks.resize(numTracks);
  for (size_t i = 0; i < numTracks; ++i)
    _tracks[i].basicGraph();
}

TxSequencer::TxSequencer(uint32_t numTracks)
  : TxSequencer(numTracks, 60, 8)
{
}

TxSequencer::~TxSequencer()
{

}

void TxSequencer::setLength(uint64_t length)
{
  _length = length;
  for (size_t i = 0; i < _tracks.size(); ++i) {
    std::cout << "resize track sequence " << i << std::endl;
    _tracks[i].setLength(_length);
  }
}

void TxSequencer::setBPM(uint32_t bpm)
{
  _bpm = bpm;
  TxTime& time = TxTime::instance();
  time.setBPM(_bpm);
}

void TxSequencer::setBeat(uint32_t trackIdx, uint64_t beatIdx, const Beat& beat)
{
  if(_length <= beatIdx) {
    std::cerr << "Invalid beat index : " << beatIdx << 
    " (max = " << (_length - 1) << ")" << std::endl;
    return;
  }
  _tracks[trackIdx].setBeat(beatIdx, beat);
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
  return _size;
}

Beat* TxSequencer::beat(uint32_t trackIdx, size_t beatIdx)
{
  return _tracks[trackIdx].beat(beatIdx);
}

TxTrack* TxSequencer::track(size_t index)
{
  if (index < _tracks.size())return &_tracks[index];
  else return NULL;
}

stk::StkFloat TxSequencer::tick(unsigned int channel)
{
  
  float sample = 0.f;
  //int nActiveTrack = 0;
  for (size_t i = 0; i < _tracks.size(); ++i) {
    if (_tracks[i].active() && _tracks[i].graph()) {
      //nActiveTrack++;
      const Index index = TxTime::instance().index(_tracks[i].length());
      sample += _tracks[i].graph()->tick() * _volume;
      //sample += channel ? beat->second : BIT_CHECK(beat->first, index.second);
    }
  }
  
  return sample;
}

stk::StkFrames& TxSequencer::tick(stk::StkFrames& frames, unsigned int channel)
{
  memset(&frames[0], 0, frames.size() * sizeof(stk::StkFloat));
  
  Index index = TxTime::instance().index(_tracks[0].length());

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

bool TxSequencer::drawBeat(TxTrack* track, uint32_t beatIdx, uint32_t bitIdx, bool current)
{
  const size_t beatWidth = _size.x / track->length();
  const size_t bitWidth = beatWidth / NumBits;
  bool modified = false;
  const std::string hiddenPrefix = "##" + std::to_string(beatIdx);
  
  Beat* beat = track->beat(beatIdx);
  ImGui::BeginGroup();
  if (ImGui::VSliderFloat((hiddenPrefix + "Slider").c_str(), ImVec2(beatWidth, 120),
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
    if (ImGui::Button((hiddenPrefix + "Btn" + std::to_string(i)).c_str(), ImVec2(bitWidth, 12))) {
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


void TxSequencer::draw()
{  
  TxTime& time = TxTime::instance();
  float t = time.get();

  Index index = TxTime::instance().index(_tracks[0].length());

  static float h = 0.f;

  ImGuiIO& io = ImGui::GetIO();
  ImGuiStyle& style = ImGui::GetStyle();
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, style.ItemSpacing);
  ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, style.ItemInnerSpacing);
  const ImVec2 pos(0, h);
  _size = io.DisplaySize - pos;
  ImGui::SetNextWindowPos(pos);
  ImGui::SetNextWindowSize(_size);
  ImGui::Begin(_name.c_str(), NULL, Flags);

  ImGui::SetWindowFontScale(1.f);
  
  ImGui::Checkbox("Running", &_running);
  ImGui::SameLine();
  
  if (ImGuiKnobs::KnobInt("Bpm", &_bpm, 1, 220, 1, "%ibpm",
    ImGuiKnobVariant_WiperDot, 0.f, ImGuiKnobFlags_DragHorizontal))
      updateBPM(this);
  
  ImGui::SameLine();

  ImGuiKnobs::Knob("Volume", &_volume, 0.f, 2.f, 0.f, 0,
    ImGuiKnobVariant_WiperDot, 0.f, ImGuiKnobFlags_DragHorizontal);

  ImGui::SameLine();

  ImGui::BeginGroup();
  
  if (ImGui::Button("Reset Time")) time.reset();
  ImGui::SetNextItemWidth(100);
  ImGui::Text("Time : %fs", t);

  ImGui::Text("Rate : %fs", time.rate());
  ImGui::Text("Index : %i", index.first);
  ImGui::EndGroup();

  for (size_t i = 0; i < _tracks.size(); ++i) {
    for (size_t j = 0; j < _tracks[i].length(); ++j) {
      drawBeat(&_tracks[i], j, index.second, (j == index.first));
      if (i < _tracks[i].length() - 1)  ImGui::SameLine();
    }
  }

  ImGui::End();
  ImGui::PopStyleVar(2);
}

void TxSequencer::reset()
{
  
}