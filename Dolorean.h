#ifndef __DOLOREAN__
#define __DOLOREAN__

#include "IPlug_include_in_plug_hdr.h"

class Dolorean : public IPlug
{
public:
  Dolorean(IPlugInstanceInfo instanceInfo);
  ~Dolorean();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  double mGain;
};

#endif
