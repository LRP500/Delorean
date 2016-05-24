#include "Dolorean.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "IKeyboardControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
  mWaveform = 0,
  mAttack,
  mDecay,
  mSustain,
  mRelease,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,
  kKeybX = 1,
  kKeybY = 230
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

  GetParam(mWaveform)->InitEnum("Waveform", static_cast<size_t>(Oscillator::Mode::Sine), static_cast<size_t>(Oscillator::Mode::kNumModes));
  GetParam(mWaveform)->SetDisplayText(0, "Sine");

  IBitmap waveformBitmap = pGraphics->LoadIBitmap(WAVEFORM_ID, WAVEFORM_FN, 4);
  pGraphics->AttachControl(new ISwitchControl(this, 24, 53, mWaveform, &waveformBitmap));

  // Knob bitmap for ADSR
  IBitmap knobBitmap = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, 64);
  // Attack knob:
  GetParam(mAttack)->InitDouble("Attack", 0.01, 0.01, 10.0, 0.001);
  GetParam(mAttack)->SetShape(3);
  pGraphics->AttachControl(new IKnobMultiControl(this, 95, 34, mAttack, &knobBitmap));
  // Decay knob:
  GetParam(mDecay)->InitDouble("Decay", 0.5, 0.01, 15.0, 0.001);
  GetParam(mDecay)->SetShape(3);
  pGraphics->AttachControl(new IKnobMultiControl(this, 177, 34, mDecay, &knobBitmap));
  // Sustain knob:
  GetParam(mSustain)->InitDouble("Sustain", 0.1, 0.001, 1.0, 0.001);
  GetParam(mSustain)->SetShape(2);
  pGraphics->AttachControl(new IKnobMultiControl(this, 259, 34, mSustain, &knobBitmap));
  // Release knob:
  GetParam(mRelease)->InitDouble("Release", 1.0, 0.001, 15.0, 0.001);
  GetParam(mRelease)->SetShape(3);
  pGraphics->AttachControl(new IKnobMultiControl(this, 341, 34, mRelease, &knobBitmap));

  AttachGraphics(pGraphics);
  MakeDefaultPreset((char *) "-", kNumPrograms);

  _MIDIReceiver.setHandler(MIDIReceiver::Handler::KeyPressed, std::bind(&Oscillator::onNoteOn, &_oscillator, std::placeholders::_1, std::placeholders::_2));
  _MIDIReceiver.setHandler(MIDIReceiver::Handler::KeyReleased, std::bind(&Oscillator::onNoteOff, &_oscillator, std::placeholders::_1, std::placeholders::_2));
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
  switch(paramIdx) {
    case mWaveform:
      _oscillator.setMode(static_cast<Oscillator::Mode>(GetParam(mWaveform)->Int()));
      break;
    case mAttack:
    case mDecay:
    case mSustain:
    case mRelease:
      _oscillator.setEnveloppeStageValue(static_cast<EnvelopeGenerator::Stage>(paramIdx), GetParam(paramIdx)->Value());
      break;
  }
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