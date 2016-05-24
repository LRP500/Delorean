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

                if (this->_keyHandlers[0]) this->_keyHandlers[0](noteNumber, velocity);
            }
        } else {
            if(_keyStatus[noteNumber] == true) {
                _keyStatus[noteNumber] = false;
                _numKeys -= 1;
            }
            // If the last note was released, nothing should play:
            if (noteNumber == _lastNoteNumber) {
                _lastNoteNumber = -1;

                if (this->_keyHandlers[1]) this->_keyHandlers[1](noteNumber, velocity);
            }
        }
        this->_midiQueue.Remove();
    }
    _offset += 1;
}

void MIDIReceiver::setHandler(Handler h, std::function<void (int, int)> &&fn) {
    this->_keyHandlers[static_cast<size_t>(h)] = std::move(fn);
}
