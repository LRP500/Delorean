//
//  EnvelopeGenerator.cpp
//  Dolorean
//
//  Created by Morris on 20/05/16.
//
//

#include "EnvelopeGenerator.hpp"

EnvelopeGenerator::EnvelopeGenerator() { }

double EnvelopeGenerator::nextSample() {
    if (_currentStage != Stage::Off &&
        _currentStage != Stage::Sustain) {
        if (_currentSampleIndex == _nextStageSampleIndex) {
            Stage newStage = static_cast<Stage>((static_cast<size_t>(_currentStage) + 1) % static_cast<size_t>(Stage::kNumStages));
            this->enterStage(newStage);
        }
        _currentLevel *= _multiplier;
        _currentSampleIndex += 1;
    }
    return _currentLevel;
}

void EnvelopeGenerator::calculateMultiplier(double startLevel,
                                            double endLevel,
                                            unsigned long long lengthInSamples) {
    _multiplier = 1.0 + (std::log(endLevel) - std::log(startLevel)) / (lengthInSamples);
}

void EnvelopeGenerator::enterStage(Stage newStage) {
    _currentStage = newStage;
    _currentSampleIndex = 0;
    if (_currentStage == Stage::Off ||
        _currentStage == Stage::Sustain) {
        _nextStageSampleIndex = 0;
    } else {
        _nextStageSampleIndex = _stageValue[static_cast<size_t>(_currentStage)] * _sampleRate;
    }
    switch (newStage) {
        case Stage::Off:
            _currentLevel = 0.0;
            _multiplier = 1.0;
            break;
        case Stage::Attack:
            _currentLevel = k_minimumLevel;
            this->calculateMultiplier(_currentLevel,
                                1.0,
                                _nextStageSampleIndex);
            break;
        case Stage::Decay:
            _currentLevel = 1.0;
            this->calculateMultiplier(_currentLevel,
                                      fmax(_stageValue[static_cast<size_t>(Stage::Sustain)], k_minimumLevel),
                                _nextStageSampleIndex);
            break;
        case Stage::Sustain:
            _currentLevel = _stageValue[static_cast<size_t>(Stage::Sustain)];
            _multiplier = 1.0;
            break;
        case Stage::Release:
            // We could go from ATTACK/DECAY to RELEASE,
            // so we're not changing currentLevel here.
            calculateMultiplier(_currentLevel,
                                k_minimumLevel,
                                _nextStageSampleIndex);
            break;
        default:
            break;
    }
}

void EnvelopeGenerator::setSampleRate(double newSampleRate) {
    _sampleRate = newSampleRate;
}

void EnvelopeGenerator::setStageValue(Stage stage, double value) {
    _stageValue[static_cast<size_t>(stage)] = value;
    if (stage == _currentStage) {
        // Re-calculate the multiplier and nextStageSampleIndex
        if(_currentStage == Stage::Attack ||
           _currentStage == Stage::Decay ||
           _currentStage == Stage::Release) {
            double nextLevelValue;
            switch (_currentStage) {
                case Stage::Attack:
                    nextLevelValue = 1.0;
                    break;
                case Stage::Decay:
                    nextLevelValue = fmax(_stageValue[static_cast<size_t>(Stage::Sustain)], k_minimumLevel);
                    break;
                case Stage::Release:
                    nextLevelValue = k_minimumLevel;
                    break;
                default:
                    nextLevelValue = k_minimumLevel;
                    break;
            }
            // How far the generator is into the current stage:
            double currentStageProcess = (_currentSampleIndex + 0.0) / _nextStageSampleIndex;
            // How much of the current stage is left:
            double remainingStageProcess = 1.0 - currentStageProcess;
            unsigned long long samplesUntilNextStage = remainingStageProcess * value * _sampleRate;
            _nextStageSampleIndex = _currentSampleIndex + samplesUntilNextStage;
            this->calculateMultiplier(_currentLevel, nextLevelValue, samplesUntilNextStage);
        } else if (_currentStage == Stage::Sustain) {
            _currentLevel = value;
        }
    }

    if (_currentStage == Stage::Decay &&
        stage == Stage::Sustain) {
        // We have to decay to a different sustain value than before.
        // Re-calculate multiplier:
        unsigned long long samplesUntilNextStage = _nextStageSampleIndex - _currentSampleIndex;
        this->calculateMultiplier(_currentLevel,
                                  fmax(_stageValue[static_cast<size_t>(Stage::Sustain)], k_minimumLevel),
                                  samplesUntilNextStage);
    }
}
