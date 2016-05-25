//
//  Synth.cpp
//  Delorean
//
//  Created by Morris on 25/05/16.
//
//

#include "Synth.hpp"

Synth::Synth() {
    _oscGenerator.setBeginCycleHandler(std::bind(&Synth::onBeganEnvelopeCycle, this));
    _oscGenerator.setEndCycleHandler(std::bind(&Synth::onFinishedEnvelopeCycle, this));

    _LFO.setMode(Oscillator::Mode::Triangle);
    _LFO.setFrequency(6.0);
    _LFO.setMute(false);
}

Synth::~Synth() {
    
}

double Synth::process(double frequency, double velocity) {
    double lfoFilterModulation = _LFO.nextSample() * _lfoFilterModAmount;

    _oscillator.setFrequency(frequency);
    _filter.updateCutoffMod((_filterGenerator.nextSample() * _filterEnvelopeAmount) + lfoFilterModulation);
    return this->_filter.process(_oscillator.nextSample() * _oscGenerator.nextSample() * velocity / 127.0);
}

void Synth::reset(double sampleRate) {
    this->_oscillator.setSampleRate(sampleRate);
    this->_oscGenerator.setSampleRate(sampleRate);
    this->_filterGenerator.setSampleRate(sampleRate);
    this->_LFO.setSampleRate(sampleRate);
}
