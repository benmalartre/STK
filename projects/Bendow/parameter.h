#include <Stk.h>
#include <vector>
#include "callback.h"

#ifndef TX_PARAMETER_H
#define TX_PARAMETER_H


class TxNode;

struct TxParameterPreset {
  const char*   name;
  stk::StkFloat minimum;
  stk::StkFloat maximum;
};

class TxParameter {
public:
  enum Type {
    NONE,
    BOOL,
    INT,
    ENUM,
    FLOAT,
    STRING,
    SAMPLES
  };

  enum Flag {
    HORIZONTAL  = 1,
    VERTICAL    = 2,
    KNOB        = 4,
    EVEN        = 8,
    ODD         = 16
  };

  TxParameter(const std::string& name, void* data, 
    short type=NONE, int flags=HORIZONTAL);
  virtual ~TxParameter();
  const std::string& name();
  const std::string& label();
  void setLabel(const std::string& label);
  void connect(TxNode* node, short channel=0);
  void disconnect();
  virtual void set(stk::StkFloat value) = 0;
  virtual stk::StkFloat tick() = 0;
  virtual bool draw() = 0;
  void setCallback(TxCallback* callback);

protected:
  short             _type;
  int               _flags;
  std::string       _name;
  std::string       _label;
  TxNode*           _input;
  short             _channel;
  void*             _data;
  TxCallback*       _callback;
};

class TxParameterBool : public TxParameter {
public:
  TxParameterBool(const std::string& name, bool* data);

  void set(stk::StkFloat value) override;
  stk::StkFloat tick() override;
  bool draw() override;
};

class TxParameterInt : public TxParameter {
public:
  TxParameterInt(const std::string& name, int minimum, int maximum, int* data,
    int flags=HORIZONTAL);

  void set(stk::StkFloat value) override;
  void setMinimum(int value);
  void setMaximum(int value);
  stk::StkFloat tick() override;
  bool draw() override;

private:
  int     _minimum;
  int     _maximum;
};

class TxParameterEnum : public TxParameter {
public:
  TxParameterEnum(const std::string& name, const char** names, int num, int* data);

  void set(stk::StkFloat value) override;
  stk::StkFloat tick() override;
  bool draw() override;

private:
  const char**  _names;
  int           _num;
};

class TxParameterFloat : public TxParameter {
public:
  TxParameterFloat(const std::string& name, stk::StkFloat minimum, stk::StkFloat maximum, 
    stk::StkFloat* data, int flags=HORIZONTAL);

  void set(stk::StkFloat value) override;
  void setMinimum(stk::StkFloat value);
  void setMaximum(stk::StkFloat value);
  stk::StkFloat tick() override;
  bool draw() override;

private:
  stk::StkFloat     _minimum;
  stk::StkFloat     _maximum;
};

class TxParameterString : public TxParameter {
public:
  TxParameterString(const std::string& name, std::string* data, int flags=HORIZONTAL);

  void set(stk::StkFloat value) override;
  const std::string& get();
  stk::StkFloat tick() override;
  bool draw() override;

private:
  std::string       _value;
};

class TxParameterSamples : public TxParameter {
public:
  TxParameterSamples(const std::string& name);

  void set(stk::StkFloat value) override;
  stk::StkFloat tick() override;
  bool draw() override;

private:
  stk::StkFrames*    _frames;;
};

#endif // TX_PARAMETER_H