//
//  Oscillator.hpp
//  Dolorean
//
//  Created by Morris on 20/05/16.
//
//

#pragma once

#include <cmath>
#include "EnvelopeGenerator.hpp"
#include "Filter.hpp"

class Oscillator {
    static constexpr double k_PI = M_PI;
    static constexpr double k_2PI = 2 * M_PI;

public:
    enum class Mode: size_t {
        Sine = 0,
        Saw,
        Square,
        Triangle,
        kNumModes
    };

public:
    Oscillator();

public:
    inline bool         isMuteded() const { return _muted; }
    inline void         toogleMute() { _muted = !_muted; }
    inline void         setMute(bool state) { _muted = state; }

    void                setMode(Mode mode);
    void                setFrequency(double frequency);
    void                setSampleRate(double sampleRate);
    void                generate(double* buffer, int nFrames);
    double              nextSample();

protected:
    void                updateIncrement();

private:
    Mode                _mode                   = Mode::Sine;
    bool                _muted                  = true;
    double              _frequency              = 440.0;
    double              _phase                  = 0;
    double              _sampleRate             = 44100.0;
    double              _phaseIncrement         = 0.0;

};