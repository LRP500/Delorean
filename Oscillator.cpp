//
//  Oscillator.cpp
//  Dolorean
//
//  Created by Morris on 20/05/16.
//
//

#include "Oscillator.hpp"

Oscillator::Oscillator()
{
    updateIncrement();
}

void Oscillator::setMode(Mode mode) {
    this->_mode = mode;
}

void Oscillator::setFrequency(double frequency) {
    this->_frequency = frequency;
    this->updateIncrement();
}

void Oscillator::setSampleRate(double sampleRate) {
    this->_sampleRate = sampleRate;
    this->updateIncrement();
}

void Oscillator::updateIncrement() {
    this->_phaseIncrement = this->_frequency * 2 * k_PI / this->_sampleRate;
}

void Oscillator::generate(double* buffer, int nFrames) {
    const double twoPI = 2 * k_PI;
    switch (this->_mode) {
        case Mode::Sine:
            for (int i = 0; i < nFrames; i++) {
                buffer[i] = sin(_phase);
                _phase += _phaseIncrement;
                while (_phase >= twoPI) {
                    _phase -= twoPI;
                }
            }
            break;
        case Mode::Saw:
            for (int i = 0; i < nFrames; i++) {
                buffer[i] = 1.0 - (2.0 * _phase / twoPI);
                _phase += _phaseIncrement;
                while (_phase >= twoPI) {
                    _phase -= twoPI;
                }
            }
            break;
        case Mode::Square:
            for (int i = 0; i < nFrames; i++) {
                if (_phase <= k_PI) {
                    buffer[i] = 1.0;
                } else {
                    buffer[i] = -1.0;
                }
                _phase += _phaseIncrement;
                while (_phase >= twoPI) {
                    _phase -= twoPI;
                }
            }
            break;
        case Mode::Triangle:
            for (int i = 0; i < nFrames; i++) {
                double value = -1.0 + (2.0 * _phase / twoPI);
                buffer[i] = 2.0 * (fabs(value) - 0.5);
                _phase += _phaseIncrement;
                while (_phase >= twoPI) {
                    _phase -= twoPI;
                }
            }
            break;
    }
}

double Oscillator::nextSample() {
    double value = 0.0;
    if(this->isMuteded()) return value;

    switch (this->_mode) {
        case Mode::Sine:
            value = sin(this->_phase);
            break;
        case Mode::Saw:
            value = 1.0 - (2.0 * this->_phase / k_2PI);
            break;
        case Mode::Square:
            if (this->_phase <= k_PI) {
                value = 1.0;
            } else {
                value = -1.0;
            }
            break;
        case Mode::Triangle:
            value = -1.0 + (2.0 * this->_phase / k_2PI);
            value = 2.0 * (fabs(value) - 0.5);
            break;
    }
    this->_phase += this->_phaseIncrement;
    while (this->_phase >= k_2PI) {
        this->_phase -= k_2PI;
    }
    return value;
}