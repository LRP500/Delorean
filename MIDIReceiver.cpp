//
//  MIDIReceiver.cpp
//  Dolorean
//
//  Created by Morris on 20/05/16.
//
//

#include "MIDIReceiver.hpp"

MIDIReceiver::MIDIReceiver()  {
    for (size_t i = 0; i < k_keyCount; i++) {
        this->_keyStatus[i] = false;
    }
}

void MIDIReceiver::onMessageReceived(IMidiMsg* midiMessage) {
    IMidiMsg::EStatusMsg status = midiMessage->StatusMsg();
    // We're only interested in Note On/Off messages (not CC, pitch, etc.)
    if(status == IMidiMsg::kNoteOn || status == IMidiMsg::kNoteOff) {
        this->_midiQueue.Add(midiMessage);
    }
}

void MIDIReceiver::advance() {
    while (!this->_midiQueue.Empty()) {
        IMidiMsg* midiMessage = this->_midiQueue.Peek();
        if (midiMessage->mOffset > _offset) break;

        IMidiMsg::EStatusMsg status = midiMessage->StatusMsg();
        int noteNumber = midiMessage->NoteNumber();
        int velocity = midiMessage->Velocity();
        // There are only note on/off messages in the queue, see ::OnMessageReceived
        if (status == IMidiMsg::kNoteOn && velocity) {
            if(_keyStatus[noteNumber] == false) {
                _keyStatus[noteNumber] = true;
                _numKeys += 1;
            }
            // A key pressed later overrides any previously pressed key:
            if (noteNumber != _lastNoteNumber) {
                _lastNoteNumber = noteNumber;
                _lastFrequency = noteNumberToFrequency(_lastNoteNumber);
                _lastVelocity = velocity;
            }
        } else {
            if(_keyStatus[noteNumber] == true) {
                _keyStatus[noteNumber] = false;
                _numKeys -= 1;
            }
            // If the last note was released, nothing should play:
            if (noteNumber == _lastNoteNumber) {
                _lastNoteNumber = -1;
                _lastFrequency = -1;
                _lastVelocity = 0;
            }
        }
        this->_midiQueue.Remove();
    }
    _offset += 1;
}