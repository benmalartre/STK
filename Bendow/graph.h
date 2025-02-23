#include <vector>
#include "common.h"
#include "node.h"

#ifndef TX_GRAPH_H
#define TX_GRAPH_H

class TxSequencer;
class TxTrack;
class TxGraph : public TxNode {
public:
  enum Parameters {
    SAMPLES = TxNode::LAST
  };
  TxGraph(TxNode* parent, const std::string& name);
  ~TxGraph();

  void                    setCurrent(TxNode* node);
  void                    setVolume(float volume);
  void                    addNode(TxNode* node);
  void                    removeNode(TxNode* node);
  void                    addConnexion(TxConnexion* connexion);
  void                    removeConnexion(TxConnexion* connexion);
  TxConnexion*            connexion(TxParameter* dst);
  
  stk::StkFloat           tick(unsigned int=0) override;

  const ImVec2&           size() override;
  void                    reset() override;

  float                   volume(){return _volume;};
  TxNode*                 current();
  void                    basic(TxNode * input);
  void                    sequenced(TxNode * input);

  std::vector<TxNode*>&   nodes() {return _nodes;};
  std::vector<TxConnexion*>& connexions(){return _connexions;};

protected:            
  bool                     contains(const TxNode* node);
  int                      index(const TxNode* node);

private:
  void                      _createNodeByType(int type);
  void                      _drawImpl(TxEditor* editor, bool* modified) override {};

  std::string               _name;
  std::vector<TxNode*>      _nodes;
  std::vector<TxConnexion*> _connexions;
  std::vector<TxParameter*> _inputs;
  float                     _volume;
  TxNode*                   _current;
};

#endif // TX_GRAPH_H