//
//  Synth.hpp
//  Delorean
//
//  Created by Morris on 25/05/16.
//
//

#pragma once

#include "Oscillator.hpp"
#include "Filter.hpp"
#include "EnvelopeGenerator.hpp"

class Synth {
public:
    Synth();
    ~Synth();

public:
    inline void         onNoteOn(const int noteNumber, const int velocity)
    { _oscGenerator.enterStage(EnvelopeGenerator::Stage::Attack); _filterGenerator.enterStage(EnvelopeGenerator::Stage::Attack); };
    inline void         onNoteOff(const int noteNumber, const int velocity)
    { _oscGenerator.enterStage(EnvelopeGenerator::Stage::Release); _filterGenerator.enterStage(EnvelopeGenerator::Stage::Release); };
    inline void         onBeganEnvelopeCycle() { _oscillator.setMute(false); }
    inline void         onFinishedEnvelopeCycle() { _oscillator.setMute(true); }

public:
    void                reset(double);
    double              process(double, double);

    // MARK: Setters
    inline void         setOscillatorMode(Oscillator::Mode newMode) { _oscillator.setMode(newMode); }
    inline void         setOscEnveloppeStageValue(EnvelopeGenerator::Stage s, double value) { _oscGenerator.setStageValue(s, value); }
    inline void         setFilterCutoff(double value) { _filter.setCutoff(value); }
    inline void         setFilterResonance(double value) { _filter.setResonance(value); }
    inline void         setFilterMode(Filter::Mode newMode) { _filter.setFilterMode(newMode); }
    inline void         setFilterStageValue(EnvelopeGenerator::Stage stage, double value) { _filterGenerator.setStageValue(stage, value); }
    inline void         setFilterEnvelopeAmount(double value) { _filterEnvelopeAmount = value; }

protected:
    Oscillator          _oscillator;
    Filter              _filter;
    EnvelopeGenerator   _oscGenerator;
    EnvelopeGenerator   _filterGenerator;
    Oscillator          _LFO;
    double              _lfoFilterModAmount;
    double              _filterEnvelopeAmount       = 0;
};