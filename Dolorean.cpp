#include "Dolorean.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "IKeyboardControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,
  kKeybX = 1,
  kKeybY = 0
};

Dolorean::Dolorean(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
{
  TRACE;

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(BG_ID, BG_FN);

  IBitmap whiteKeyImage = pGraphics->LoadIBitmap(WHITE_KEY_ID, WHITE_KEY_FN, 6);
  IBitmap blackKeyImage = pGraphics->LoadIBitmap(BLACK_KEY_ID, BLACK_KEY_FN);

  int keyCoordinates[12] = { 0, 7, 12, 20, 24, 36, 43, 48, 56, 60, 69, 72 };
  this->_virtualKeyboard = new IKeyboardControl(this, kKeybX, kKeybY, k_virtualKeyboardMinimumNoteNumber, /* octaves: */ 5, &whiteKeyImage, &blackKeyImage, keyCoordinates);

  pGraphics->AttachControl(_virtualKeyboard);

  AttachGraphics(pGraphics);
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

Dolorean::~Dolorean() {}

void Dolorean::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames) {
  // Mutex is already locked for us.

  double *leftOutput = outputs[0];
  double *rightOutput = outputs[1];

  this->processVirtualKeyboard();

  for (int i = 0; i < nFrames; ++i) {
    this->_MIDIReceiver.advance();
    int velocity = this->_MIDIReceiver.getLastVelocity();
    if (velocity > 0) {
      this->_oscillator.setFrequency(this->_MIDIReceiver.getLastFrequency());
      this->_oscillator.setMute(false);
    } else {
      this->_oscillator.setMute(true);
    }
    leftOutput[i] = rightOutput[i] = this->_oscillator.nextSample() * velocity / 127.0;
  }

  this->_MIDIReceiver.Flush(nFrames);
}

void Dolorean::Reset() {
  TRACE;
  IMutexLock lock(this);

  this->_oscillator.setSampleRate(GetSampleRate());
}

void Dolorean::OnParamChange(int paramIdx) {
  IMutexLock lock(this);

}

void Dolorean::ProcessMidiMsg(IMidiMsg* pMsg) {
  this->_MIDIReceiver.onMessageReceived(pMsg);
  this->_virtualKeyboard->SetDirty();
}

void Dolorean::processVirtualKeyboard() {
  IKeyboardControl* virtualKeyboard = (IKeyboardControl*) this->_virtualKeyboard; // TODO: Cast
  int virtualKeyboardNoteNumber = virtualKeyboard->GetKey() + k_virtualKeyboardMinimumNoteNumber;

  if(_lastVirtualKeyboardNoteNumber >= k_virtualKeyboardMinimumNoteNumber && virtualKeyboardNoteNumber != _lastVirtualKeyboardNoteNumber) {
    // The note number has changed from a valid key to something else (valid key or nothing). Release the valid key:
    IMidiMsg midiMessage;
    midiMessage.MakeNoteOffMsg(_lastVirtualKeyboardNoteNumber, 0);
    _MIDIReceiver.onMessageReceived(&midiMessage);
  }

  if (virtualKeyboardNoteNumber >= k_virtualKeyboardMinimumNoteNumber && virtualKeyboardNoteNumber != _lastVirtualKeyboardNoteNumber) {
    // A valid key is pressed that wasn't pressed the previous call. Send a "note on" message to the MIDI receiver:
    IMidiMsg midiMessage;
    midiMessage.MakeNoteOnMsg(virtualKeyboardNoteNumber, virtualKeyboard->GetVelocity(), 0);
    _MIDIReceiver.onMessageReceived(&midiMessage);
  }

  _lastVirtualKeyboardNoteNumber = virtualKeyboardNoteNumber;
}