#include "Dolorean.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "IKeyboardControl.h"
#include "resource.h"
#include "Oscillator.hpp"

const int kNumPrograms = 1;

enum EParams
{
  mWaveform = 0,
  mAttack,
  mDecay,
  mSustain,
  mRelease,
  mFilterMode,
  mFilterCutoff,
  mFilterResonance,
  mFilterAttack,
  mFilterDecay,
  mFilterSustain,
  mFilterRelease,
  mFilterEnvelopeAmount,
  kNumParams
};
enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,
  kKeybX = 1,
  kKeybY = 230
};

Dolorean::Dolorean(IPlugInstanceInfo instanceInfo) // TODO: Refactor
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
  pGraphics->AttachControl(new ISwitchControl(this, 24, 38, mWaveform, &waveformBitmap));

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

  GetParam(mFilterMode)->InitEnum("Filter Mode", static_cast<size_t>(Filter::Mode::LowPass), static_cast<size_t>(Filter::Mode::kNumFilterModes));
  IBitmap filtermodeBitmap = pGraphics->LoadIBitmap(FILTERMODE_ID, FILTERMODE_FN, 3);
  pGraphics->AttachControl(new ISwitchControl(this, 24, 123, mFilterMode, &filtermodeBitmap));

  // Knobs for filter cutoff and resonance
  IBitmap smallKnobBitmap = pGraphics->LoadIBitmap(KNOB_SMALL_ID, KNOB_SMALL_FN, 64);
  // Cutoff knob:
  GetParam(mFilterCutoff)->InitDouble("Cutoff", 0.99, 0.01, 0.99, 0.001);
  GetParam(mFilterCutoff)->SetShape(2);
  pGraphics->AttachControl(new IKnobMultiControl(this, 5, 177, mFilterCutoff, &smallKnobBitmap));
  // Resonance knob:
  GetParam(mFilterResonance)->InitDouble("Resonance", 0.01, 0.01, 1.0, 0.001);
  pGraphics->AttachControl(new IKnobMultiControl(this, 61, 177, mFilterResonance, &smallKnobBitmap));

  // Knobs for filter envelope
  // Attack knob
  GetParam(mFilterAttack)->InitDouble("Filter Env Attack", 0.01, 0.01, 10.0, 0.001);
  GetParam(mFilterAttack)->SetShape(3);
  pGraphics->AttachControl(new IKnobMultiControl(this, 139, 178, mFilterAttack, &smallKnobBitmap));
  // Decay knob:
  GetParam(mFilterDecay)->InitDouble("Filter Env Decay", 0.5, 0.01, 15.0, 0.001);
  GetParam(mFilterDecay)->SetShape(3);
  pGraphics->AttachControl(new IKnobMultiControl(this, 195, 178, mFilterDecay, &smallKnobBitmap));
  // Sustain knob:
  GetParam(mFilterSustain)->InitDouble("Filter Env Sustain", 0.1, 0.001, 1.0, 0.001);
  GetParam(mFilterSustain)->SetShape(2);
  pGraphics->AttachControl(new IKnobMultiControl(this, 251, 178, mFilterSustain, &smallKnobBitmap));
  // Release knob:
  GetParam(mFilterRelease)->InitDouble("Filter Env Release", 1.0, 0.001, 15.0, 0.001);
  GetParam(mFilterRelease)->SetShape(3);
  pGraphics->AttachControl(new IKnobMultiControl(this, 307, 178, mFilterRelease, &smallKnobBitmap));

  // Filter envelope amount knob:
  GetParam(mFilterEnvelopeAmount)->InitDouble("Filter Env Amount", 0.0, -1.0, 1.0, 0.001);
  pGraphics->AttachControl(new IKnobMultiControl(this, 363, 178, mFilterEnvelopeAmount, &smallKnobBitmap));


  AttachGraphics(pGraphics);
  MakeDefaultPreset((char *) "-", kNumPrograms);

  _MIDIReceiver.setHandler(MIDIReceiver::Handler::KeyPressed, std::bind(&Synth::onNoteOn, &_synth, std::placeholders::_1, std::placeholders::_2));
  _MIDIReceiver.setHandler(MIDIReceiver::Handler::KeyReleased, std::bind(&Synth::onNoteOff, &_synth, std::placeholders::_1, std::placeholders::_2));
}

Dolorean::~Dolorean() {}

void Dolorean::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames) {
  // Mutex is already locked for us.

  double *leftOutput = outputs[0];
  double *rightOutput = outputs[1];

  this->processVirtualKeyboard();

  for (int i = 0; i < nFrames; ++i) {
    this->_MIDIReceiver.advance();
    leftOutput[i] = rightOutput[i] = _synth.process(this->_MIDIReceiver.getLastFrequency(), this->_MIDIReceiver.getLastVelocity());
  }

  this->_MIDIReceiver.Flush(nFrames);
}

void Dolorean::Reset() {
  TRACE;
  IMutexLock lock(this);

  this->_synth.reset(GetSampleRate());
}

void Dolorean::OnParamChange(int paramIdx) {
  IMutexLock lock(this);
  switch(paramIdx) {
    case mWaveform:
      _synth.setOscillatorMode(static_cast<Oscillator::Mode>(GetParam(mWaveform)->Int()));
      break;
    case mAttack:
    case mDecay:
    case mSustain:
    case mRelease:
      _synth.setOscEnveloppeStageValue(static_cast<EnvelopeGenerator::Stage>(paramIdx), GetParam(paramIdx)->Value());
      break;
    case mFilterCutoff:
      _synth.setFilterCutoff(GetParam(paramIdx)->Value());
      break;
    case mFilterResonance:
      _synth.setFilterResonance(GetParam(paramIdx)->Value());
      break;
    case mFilterMode:
      _synth.setFilterMode(static_cast<Filter::Mode>(GetParam(paramIdx)->Int()));
      break;
    case mFilterAttack:
      _synth.setFilterStageValue(EnvelopeGenerator::Stage::Attack, GetParam(paramIdx)->Value());
      break;
    case mFilterDecay:
      _synth.setFilterStageValue(EnvelopeGenerator::Stage::Decay, GetParam(paramIdx)->Value());
      break;
    case mFilterSustain:
      _synth.setFilterStageValue(EnvelopeGenerator::Stage::Sustain, GetParam(paramIdx)->Value());
      break;
    case mFilterRelease:
      _synth.setFilterStageValue(EnvelopeGenerator::Stage::Release, GetParam(paramIdx)->Value());
      break;
    case mFilterEnvelopeAmount:
      _synth.setFilterEnvelopeAmount(GetParam(paramIdx)->Value());
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