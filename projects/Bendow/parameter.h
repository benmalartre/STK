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

#define TX_SEVENSEGMENTS_WIDTH 15.f
#define TX_SEVENSEGMENTS_HEIGHT 7.f

class TxSevenSegments {
public:
  static void drawDigit(ImDrawList* drawList, int n, ImVec2 e, ImVec2 p, ImVec4 c);
  static void drawDigits(ImDrawList* d, ImVec2 a, ImVec2 b, ImVec2 sz, ImVec4 c, float t);
  static void setWidth(float  width);
  static void setHeight(float height);

  static char _kd[];
  static float W;
  static float H;
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
    HORIZONTAL    = 1,
    VERTICAL      = 2,
    KNOB          = 4,
    SEVENSEGMENTS = 8
  };

  TxParameter(TxNode* node, const std::string& name, void* data, 
    short type=NONE, int flags=HORIZONTAL);
  virtual ~TxParameter();
  TxNode* node();
  short type();
  const int& flags();
  const std::string& name();
  const std::string& label();
  const ImVec2& plug(short channel);
  const float& radius(short channel);
  void setLabel(const std::string& label);
  void connect(TxNode* node, short channel=0);
  void disconnect();
 
  virtual void set(stk::StkFloat value) = 0;
  virtual stk::StkFloat tick() = 0;
  virtual stk::StkFloat tick(unsigned int channel) { return 0.f; };
  virtual bool draw() = 0;
  void setCallback(TxCallback* callback);

protected:
  void              _drawPlug(short channel);

  short             _type;
  int               _flags;
  std::string       _name;
  std::string       _label;
  TxNode*           _node;
  TxNode*           _input;
  short             _iChannel;
  void*             _data;
  TxCallback*       _callback;
  ImVec2            _plug[2];
  float             _radius[2];
};

class TxParameterBool : public TxParameter {
public:
  TxParameterBool(TxNode* node, const std::string& name, bool* data);

  void set(stk::StkFloat value) override;
  stk::StkFloat tick() override;
  bool draw() override;
};

class TxParameterInt : public TxParameter {
public:
  TxParameterInt(TxNode* node, const std::string& name, int minimum, int maximum, int* data,
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
  TxParameterEnum(TxNode* node, const std::string& name, const char** names, int num, int* data);

  void set(stk::StkFloat value) override;
  stk::StkFloat tick() override;
  bool draw() override;

private:
  const char**  _names;
  int           _num;
};

class TxParameterFloat : public TxParameter {
public:
  TxParameterFloat(TxNode* node, const std::string& name, stk::StkFloat minimum, stk::StkFloat maximum,
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
  TxParameterString(TxNode* node, const std::string& name, std::string* data, int flags=HORIZONTAL);

  void set(stk::StkFloat value) override;
  const std::string& get();
  stk::StkFloat tick() override;
  bool draw() override;

private:
  std::string       _value;
};

class TxParameterSamples : public TxParameter {
public:
  TxParameterSamples(TxNode* node, const std::string& name, bool io=true, int nChannels=1);

  void setIndex(int index);
  bool io();
  void set(stk::StkFloat value) override;
  stk::StkFloat tick() override;
  bool draw() override;

private:
  stk::StkFrames*    _frames;
  bool               _io;
  int                _nChannels;
  int                _index;
};

#endif // TX_PARAMETER_H