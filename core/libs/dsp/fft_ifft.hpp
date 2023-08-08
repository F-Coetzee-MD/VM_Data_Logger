#pragma once

#include <math.h>
#include <vector>
#include <iostream>

#include "./special_types.hpp"

using namespace std;

class fft
{
    private:

    float* W(int, int);
    

    public:

    vector<complexNum> FFT(vector<float>);
    vector<float> IFFT(vector<complexNum>);
    vector<float> MAG(vector<complexNum>);
    vector<float> PHA(vector<complexNum>);
};