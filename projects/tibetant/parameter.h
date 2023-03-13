#include <Stk.h>
#include <vector>

#ifndef TX_PARAMETER_H
#define TX_PARAMETER_H

class TxNode;

class TxParameter {
public:
  TxParameter(const std::string& name, TxNode* input=NULL);

  const std::string& name();
  void connect(TxNode* node);
  void disconnect();
  virtual void set(stk::StkFloat value) = 0;
  virtual stk::StkFloat tick() = 0;

protected:
  std::string       _name;
  stk::StkFloat     _minimum;
  stk::StkFloat     _maximum;
  stk::StkFloat     _value;
  TxNode*           _input;
};

class TxParameterBool : public TxParameter {
public:
  TxParameterBool(const std::string& name, bool value=true, TxNode* input=NULL);

  void set(stk::StkFloat value) override;
  stk::StkFloat tick() override;

private:
  bool     _value;
};

class TxParameterInt : public TxParameter {
public:
  TxParameterInt(const std::string& name, int minimum, int maximum, 
    int value=0, TxNode* input=NULL);

  void set(stk::StkFloat value) override;
  stk::StkFloat tick() override;

private:
  int     _minimum;
  int     _maximum;
  int     _value;
};

class TxParameterFloat : public TxParameter {
public:
  TxParameterFloat(const std::string& name, stk::StkFloat minimum, stk::StkFloat maximum,
    stk::StkFloat value, TxNode* input=NULL);

  void set(stk::StkFloat value) override;
  stk::StkFloat tick() override;

private:
  stk::StkFloat     _minimum;
  stk::StkFloat     _maximum;
  stk::StkFloat     _value;
};

class TxParameterSamples : public TxParameter {
public:
  TxParameterSamples(const std::string& name, TxNode* input=NULL);

  void set(stk::StkFloat value) override;
  stk::StkFloat tick() override;

private:
  stk::StkFrames*    _frames;;
};

#endif // TX_PARAMETER_H