#include <Stk.h>
#include <vector>

#ifndef TX_NODE_H
#define TX_NODE_H

class TxNode;

class TxParameter {
public:
  TxParameter(const std::string& name, const stk::StkFloat& minimum=0.f,
    const stk::StkFloat& maximum=1.f, const stk::StkFloat& value=1.f, TxNode* input=NULL);

  const std::string& name();
  void connect(TxNode* node);
  void disconnect();
  stk::StkFloat tick();

private:
  std::string       _name;
  stk::StkFloat     _minimum;
  stk::StkFloat     _maximum;
  stk::StkFloat     _value;
  TxNode*           _input;
};

class TxNode {
public:
  TxNode(const std::string& name, uint32_t numChannels=1);
  ~TxNode();
  virtual stk::StkFrames& tick() = 0;
  const stk::StkFrames& frames() const;
  stk::StkFloat lastSample();
  void setActive(bool state);
  void setDirty(bool state);
  void clearSamples();
  void setStereo(const stk::StkFloat& stereo);
  stk::StkFloat stereoWeight(uint32_t channelIdx);
  bool connect(TxNode* node, const std::string& parameter);
  virtual void draw() = 0;

protected:
  std::string               _name;
  bool                      _active;
  bool                      _dirty;
  stk::StkFloat             _volume;
  stk::StkFloat             _stereo;
  stk::StkFrames            _frames;
  std::vector<TxParameter>  _inputs;
};

class TxValue : public TxNode {
public:
  TxValue(const std::string& name);
  ~TxValue();
  stk::StkFrames& tick() override;
  void setValue(const stk::StkFloat& value);
  void draw() override;
private:
  stk::StkFloat  _value;
};

#endif // TX_NODE_H