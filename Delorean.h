#ifndef __DELOREAN__
#define __DELOREAN__

#define uint32 uint32_t // Dans le plus grand des calmes

#include "IPlug_include_in_plug_hdr.h"
#include "Synth.hpp"
#include "MIDIReceiver.hpp"

#include <vector>

class Delorean : public IPlug
{
  static constexpr int k_virtualKeyboardMinimumNoteNumber = 48;

public:
  Delorean(IPlugInstanceInfo instanceInfo);
  ~Delorean();

public:
  void                          Reset();
  void                          OnParamChange(int paramIdx);
  void                          ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

  void                          ProcessMidiMsg(IMidiMsg* pMsg);
  void                          processVirtualKeyboard();

  inline int                    GetNumKeys() const { return this->_MIDIReceiver.getNumKeys(); };
  inline bool                   GetKeyStatus(int key) const { return this->_MIDIReceiver.getKeyStatus(key); };

private:
  Synth                         _synth;
  MIDIReceiver                  _MIDIReceiver;
  IControl*                     _virtualKeyboard;
  int                           _lastVirtualKeyboardNoteNumber = k_virtualKeyboardMinimumNoteNumber - 1;

};

#endif
