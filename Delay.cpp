//
//  Delay.cpp
//  Dolorean
//
//  Created by Pierre Roy on 24/05/2016.
//
//

#include "Delay.h"

enum EParams
{
    kGain = 0,
    kTime,
    kFeedback,
};

Delay::Delay()
: _time(0.5), _feedback(0.5), _out(0.75), _cursor(0), _size(44100)
{
    _buffer = new float[_size];
    _delay = _time * (_size - 1);
}

Delay::~Delay()
{
    if (_buffer)
        delete[] _buffer;
}

void        Delay::ProcessDoubleReplacing(double** inputs, double** outputs, int sampleFrames)
{
    double  *in = inputs[0];
    double  *leftOut = outputs[0];
    double  *rightOut = outputs[1];
    
    while (--sampleFrames >= 0)
    {
        double x = *in++;
        float y = _buffer[_cursor];
        _buffer[_cursor++] = x + y * _feedback;
        if (_cursor >= _delay)
            _cursor = 0;
        *leftOut++ = y;
        if (rightOut)
            *rightOut++ = y;
    }
}
