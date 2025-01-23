#include "imgui/imgui.h"
#include "record.h"

TxRecorder::TxRecorder() 
  : _output(nullptr)
  , _recording(false)
  , _filename("")
{}

TxRecorder::~TxRecorder()
{
  if(_output){
    _output->closeFile();
    delete _output;
  }
}

void TxRecorder::setFileName(const char* filename)
{
  _filename = filename;

  if(_output != nullptr) {
    _output->closeFile();
    delete _output;
    _output = nullptr;
  }
}

void TxRecorder::start()
{
  if(_filename != "") {
    _output = new stk::FileWvOut(
      _filename, 1, stk::FileWrite::FILE_WAV, stk::Stk::STK_FLOAT32, stk::RT_BUFFER_SIZE);
      _recording = true;
  }
  else
    std::cerr << "[TxRecorder]] need a valid file name, recording not started!" << std::endl;

}

void TxRecorder::stop()
{
  if(_output) {
    std::cout << "[TxRecorder] closed file : " << _filename << std::endl;
    _output->closeFile();
    delete _output;
    _output = nullptr;
  }

  _recording = false;
}

void TxRecorder::tick(const stk::StkFloat& sample)
{
  if(!_output)return;
  _output->tick(sample);
}

void TxRecorder::draw()
{
  static const size_t BUFFER_SIZE = 512;
  static char buffer[BUFFER_SIZE];
  ImGui::Text("filename: ");
  ImGui::SameLine();
  ImGui::InputText("##filename", buffer, BUFFER_SIZE);
  if(ImGui::IsItemDeactivatedAfterEdit())
    setFileName(buffer);
  ImGui::SameLine();

  if(_recording){
    if(ImGui::Button("Stop Recording"))
      stop();
  } else {
    if(ImGui::Button("Start Recording"))
      start();
  }

}
