//
//  Filter.cpp
//  Delorean
//
//  Created by Morris on 25/05/16.
//
//

#include "Filter.hpp"

Filter::Filter() {
    this->calculateFeedbackAmount();
};

Filter::~Filter() {
}

double Filter::process(double inputValue) {
    if (inputValue == 0.0) return inputValue;
    double calculatedCutoff = getCalculatedCutoff();
    _buf0 += calculatedCutoff * (inputValue - _buf0 + _feedbackAmount * (_buf0 - _buf1));
    _buf1 += calculatedCutoff * (_buf0 - _buf1);
    _buf2 += calculatedCutoff * (_buf1 - _buf2);
    _buf3 += calculatedCutoff * (_buf2 - _buf3);
    switch (this->_mode) {
        case Mode::LowPass:
            return _buf3;
        case Mode::HighPass:
            return inputValue - _buf3;
        case Mode::BandPass:
            return _buf0 - _buf3;
        default:
            return 0.0;
    }
}