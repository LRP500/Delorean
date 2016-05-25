//
//  Filter.hpp
//  Dolorean
//
//  Created by Morris on 25/05/16.
//
//

#pragma once

#include <cmath>

class Filter {
public:
    enum class Mode: std::size_t {
        LowPass = 0,
        HighPass,
        BandPass,
        kNumFilterModes
    };

public:
    Filter();
    ~Filter();

public:
    double          process(double);
    inline void     setCutoff(double newCutoff) { _cutoff = newCutoff; this->calculateFeedbackAmount(); };
    inline void     setResonance(double newResonance) { _resonance = newResonance; this->calculateFeedbackAmount(); };
    inline void     setFilterMode(Mode newMode) { _mode = newMode; }
    inline void     updateCutoffMod(double newCutoffMod) { _cutoffMod = newCutoffMod; this->calculateFeedbackAmount(); }

protected:
    inline void     calculateFeedbackAmount() { _feedbackAmount = _resonance + _resonance / (1.0 - this->getCalculatedCutoff()); }
    inline double   getCalculatedCutoff() const { return fmax(fmin(_cutoff + _cutoffMod, 0.99), 0.01); };

private:
    double          _cutoff         = 0.99;
    double          _cutoffMod      = 0.0;
    double          _resonance      = 0.0;
    Mode            _mode           = Mode::LowPass;
    double          _buf0           = 0.0;
    double          _buf1           = 0.0;
    double          _buf2           = 0.0;
    double          _buf3           = 0.0;
    double          _feedbackAmount;
};
