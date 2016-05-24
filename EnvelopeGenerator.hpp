//
//  EnvelopeGenerator.hpp
//  Dolorean
//
//  Created by Morris on 20/05/16.
//
//

#pragma once

#include <cmath>

class EnvelopeGenerator {
    static constexpr double k_minimumLevel = 0.0001;

public:
    enum class Stage: size_t {
        Off = 0,
        Attack,
        Decay,
        Sustain,
        Release,
        kNumStages
    };

public:
    EnvelopeGenerator();

public:
    void                    enterStage(Stage newStage);
    double                  nextSample();
    void                    setSampleRate(double);
    inline Stage            getCurrentStage() const { return this->_currentStage; };
    void                    setStageValue(Stage, double);

protected:
    void                   calculateMultiplier(double startLevel, double endLevel, unsigned long long lengthInSamples);

private:
    Stage                   _currentStage = Stage::Off;
    double                  _currentLevel = k_minimumLevel;
    double                  _multiplier = 1.0;
    double                  _sampleRate = 44100.0; // TODO: Define
    double                  _stageValue[static_cast<size_t>(Stage::kNumStages)] = {0.0, 0.01, 0.5, 0.1, 1.0};
    unsigned long long      _currentSampleIndex = 0;
    unsigned long long      _nextStageSampleIndex = 0;
};
