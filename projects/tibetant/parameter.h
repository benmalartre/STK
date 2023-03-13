#include <Stk.h>
#include <vector>
#include <functional>

#ifndef TX_PARAMETER_H
#define TX_PARAMETER_H

class TxNode;

class TxParameter {
public:
  enum Type {
    NONE,
    BOOL,
    INT,
    ENUM,
    FLOAT,
    SAMPLES
  };
  TxParameter(const std::string& name, void* data, short type=NONE);

  const std::string& name();
  void connect(TxNode* node);
  void disconnect();
  virtual void set(stk::StkFloat value) = 0;
  virtual stk::StkFloat tick() = 0;
  virtual void draw() = 0;

protected:
  short             _type;
  std::string       _name;
  TxNode*           _input;
  void*             _data;
};

class TxParameterBool : public TxParameter {
public:
  TxParameterBool(const std::string& name, bool* data);

  void set(stk::StkFloat value) override;
  stk::StkFloat tick() override;
  void draw() override;
};

class TxParameterInt : public TxParameter {
public:
  TxParameterInt(const std::string& name, int minimum, int maximum, int* data);

  void set(stk::StkFloat value) override;
  stk::StkFloat tick() override;
  void draw() override;

private:
  int     _minimum;
  int     _maximum;
};

class TxParameterEnum : public TxParameter {
public:
  TxParameterEnum(const std::string& name, const char** names, int num, int* data);

  void set(stk::StkFloat value) override;
  stk::StkFloat tick() override;
  void draw() override;

private:
  const char**  _names;
  int           _num;
};

class TxParameterFloat : public TxParameter {
public:
  TxParameterFloat(const std::string& name, stk::StkFloat minimum, stk::StkFloat maximum, stk::StkFloat* data);

  void set(stk::StkFloat value) override;
  stk::StkFloat tick() override;
  void draw() override;

private:
  stk::StkFloat     _minimum;
  stk::StkFloat     _maximum;
};

class TxParameterSamples : public TxParameter {
public:
  TxParameterSamples(const std::string& name);

  void set(stk::StkFloat value) override;
  stk::StkFloat tick() override;
  void draw() override;

private:
  stk::StkFrames*    _frames;;
};

#endif // TX_PARAMETER_H