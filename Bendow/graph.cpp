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
  , _volume(1.f)
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
  _current = node;
}

TxNode* TxGraph::current()
{
  return _current;
}

void TxGraph::basic(TxNode* input)
{

  TxLfo* lfo = new TxLfo(this, "Lfo");
  TxOscillator* oscillator = new TxOscillator(this, "Oscillator");

  addNode(lfo);
  addNode(oscillator);

  lfo->connect(oscillator, "Freq");
  oscillator->connect(this, "Samples", 0);
  
  setCurrent(oscillator);
}

void TxGraph::sequenced(TxNode* input)
{
  TxOscillator* oscillator = new TxOscillator(this, "Oscillator");
  oscillator->setHarmonics(7);
  TxAdsr* adsr = new TxAdsr(this, "Adsr");
  if(input && input->type() == TxNode::TRACK) {
    TxTrack* track = (TxTrack*)input;
    track->connect(adsr, "Trigger", 0);
    track->connect(oscillator, "Freq", 1);
  }
  
  addNode(oscillator);
  addNode(adsr);

  adsr->connect(oscillator, "Env");
  oscillator->connect(this, "Samples", 0);
  
  setCurrent(oscillator);
}

void TxGraph::addNode(TxNode* node) 
{
  if(!contains(node)) {
    _nodes.push_back(node);
  }
}

void TxGraph::removeNode(TxNode* node) 
{
  std::cout << "remove node " << node << std::endl;
  int idx = index(node);
  if(idx >= 0) {
    std::vector<TxConnexion*> connexions;
    for(TxConnexion* connexion: _connexions)
      if(connexion->source->node() == node || connexion->target->node() == node)
        connexions.push_back(connexion);

    for(TxConnexion* connexion: connexions)
      removeConnexion(connexion);

    _nodes.erase(_nodes.begin() + idx);
    std::cout << "delete node" << std::endl;
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
      if(_connexions[c]->target->node()->type() == TxNode::GRAPH) {
        _current = NULL;
      }
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
  if(_current && _active)return _current->tick(channel) * _volume;
  return 0;
}

