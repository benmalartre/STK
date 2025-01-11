#include "common.h"

#ifndef TX_CALLBACK_H
#define TX_CALLBACK_H


typedef void (*CALLBACK_FN)(void* data, void* value);

class TxCallback {
public:
  TxCallback(CALLBACK_FN func, void* data, void* value=NULL)
    : _func(func)
    , _data(data)
    , _value(value)
  {
  };
  void execute() {
    _func(_data, _value);
  };
private:
  CALLBACK_FN _func;
  void*       _data;
  void*       _value;
};

#endif // TX_CALLBACK_H