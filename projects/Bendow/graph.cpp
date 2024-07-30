#include "common.h"
#include "graph.h"
#include "oscillator.h"
#include "lfo.h"
#include "adsr.h"
#include "random.h"
#include "filter.h"
#include "effect.h"
#include "arythmetic.h"
#include "sequencer.h"
#include "factory.h"


TxGraph::TxGraph(TxNode* parent, const std::string& name)
  : TxNode(parent, TxNode::GRAPH, name)
  , _current(NULL)
{
  _params.push_back(new TxParameterSamples(this, "Samples", false, 1));

};

TxGraph::~TxGraph()
{
  for(auto& connexion: _connexions)delete connexion;
  for(auto& node: _nodes)delete node;
};

bool TxGraph::contains(const TxNode* node)
{
  for(auto& n: _nodes) {
    if(node == n)return true;
  }
  return false;
}

int TxGraph::index(const TxNode* node)
{
  for(size_t i = 0; i < _nodes.size(); ++i) {
    if(_nodes[i] == node)return i;
  }
  return -1;
}

void TxGraph::setCurrent(TxNode* node)
{
  std::cout << "set current : " << node->name() << std::endl;
  _current = node;
}

TxNode* TxGraph::current()
{
  return _current;
}

void TxGraph::basic(TxNode* input)
{
  TxOscillator* oscillator = new TxOscillator(this, "Oscillator");
  oscillator->setHarmonics(7);
  TxAdsr* adsr = new TxAdsr(this, "Adsr");
  if(input && input->type() == TxNode::TRACK) {
    TxTrack* track = (TxTrack*)input;
    track->connect(adsr, "Trigger", 0);
    track->connect(oscillator, "Frequency", 1);
  }
  adsr->connect(oscillator, "Envelope");
  
  addNode(oscillator);
  addNode(adsr);
  setCurrent(oscillator);
}

void TxGraph::addNode(TxNode* node) 
{
  if(!contains(node)) {
    _nodes.push_back(node);
    _current = node;
    node->setSelected(true);
  }
}

void TxGraph::removeNode(TxNode* node) 
{
  int idx = index(node);
  if(idx >= 0) {
    _nodes.erase(_nodes.begin() + idx);
    delete node;
  }
}

void TxGraph::addConnexion(TxConnexion* connexion)
{
  _connexions.push_back(connexion);
}

void TxGraph::removeConnexion(TxConnexion* connexion)
{
  for (size_t c = 0; c < _connexions.size(); ++c) {
    if (_connexions[c] == connexion) {
      _connexions[c]->target->disconnect();
      _connexions.erase(_connexions.begin() + c);
      delete connexion;
      return;
    }
  }
}

TxConnexion* TxGraph::connexion(TxParameter* dst)
{
  for(auto& connexion: _connexions)
    if(connexion->target == dst)return connexion;
  return NULL;
}



void TxGraph::_createNodeByType(int type)
{
  TxNode* node = TxFactory::createNodeByType(this, type);
}

const ImVec2& TxGraph::size()
{
  return ImVec2();
}

void TxGraph::reset()
{

}

stk::StkFloat TxGraph::tick(unsigned int channel)
{
  if(_current)return _current->tick(channel);
  return 0;
}

