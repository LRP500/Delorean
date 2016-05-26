//
//  Delay.h
//  Dolorean
//
//  Created by Pierre Roy on 24/05/2016.
//
//

#ifndef __Dolorean__Delay__
#define __Dolorean__Delay__

class Delay
{

private:
    float   _time;
    float   _feedback;
    float   _out;
    
    float*  _buffer;
    
    long    _cursor;
    long    _size;
    int     _delay;

public:
    Delay();
    ~Delay();

    void    ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
    
};

#endif /* defined(__Dolorean__Delay__) */
