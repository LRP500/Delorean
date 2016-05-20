//
//  MIDIReceiver.hpp
//  Dolorean
//
//  Created by Morris on 20/05/16.
//
//

#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextra-tokens"
#include "IPlug_include_in_plug_hdr.h"
#pragma clang diagnostic pop

#include "IMidiQueue.h"

class MIDIReceiver {
    static constexpr int k_keyCount = 128;

public:
    MIDIReceiver();

public:
    void advance();
    void onMessageReceived(IMidiMsg* midiMessage);
    
    inline bool getKeyStatus(int keyIndex) const { return _keyStatus[keyIndex]; }
    inline int getNumKeys() const { return _numKeys; }
    inline int getLastNoteNumber() const { return _lastNoteNumber; }
    inline double getLastFrequency() const { return _lastFrequency; }
    inline int getLastVelocity() const { return _lastVelocity; }
    inline void Flush(int nFrames) { this->_midiQueue.Flush(nFrames); _offset = 0; }
    inline void Resize(int blockSize) { this->_midiQueue.Resize(blockSize); }

protected:
    inline double       noteNumberToFrequency(int noteNumber)
        { return 440.0 * pow(2.0, (noteNumber - 69.0) / 12.0); }


private:
    IMidiQueue                      _midiQueue;
    int                             _numKeys            = 0;    // how many keys are being played at the moment (via midi)
    bool                            _keyStatus[k_keyCount];     // array of on/off for each key (index is note number)
    int                             _lastNoteNumber     = -1;
    double                          _lastFrequency      = -1.0;
    int                             _lastVelocity       = 0;
    int                             _offset             = 0;

};