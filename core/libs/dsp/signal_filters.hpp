#pragma once

#include <math.h>
#include <vector>
#include <iostream>

#include "./special_types.hpp"

using namespace std;

float VectorAverage(vector<float>*);

class window
{
    public:

    void Push(vector<float>*);
    float Pop();
    vector<vector<float>> *Values;
    int size;
};

class SlidingWindow
{
    public:

    void Push(float);
    void Pop();
    vector<float> *Values;
    int size;
};

class DSP
{
    public:

    bool SlidingWindowAverage(vector<float>*, int);
    void GroupedSamplesAverage(vector<float>*, vector<vector<float>>*, int);
    void RemoveOutliers(vector<float>*, int);
    void LowPassFilter(vector<float>*, int);
    void SmoothingFilterAverage(vector<float>*, float);
    void SmoothingFilterZero(vector<float>*, int);
    void SmoothingFilterFirst(vector<float>*, int);
    void BandRejectFFT(vector<complexNum>*, int, int);
    void BandPassFFT(vector<complexNum>*, int, int);
};

