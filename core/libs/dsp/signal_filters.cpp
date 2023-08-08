#include "./signal_filters.hpp"
#include <cmath>      // For mathematical functions
#include <deque>      // For the deque container
#include <vector>     // For the vector container

float VectorAverage(vector<float>* signal)
{
    //get the average of a vector

    float lf_Average = 0; // Variable to store the sum of vector elements

    for (int C1 = 0; C1 < static_cast<int>(signal->size()); C1++)
    {
        lf_Average = lf_Average + (*signal)[C1]; // Accumulate the sum of vector elements
    }

    lf_Average /= signal->size(); // Divide the sum by the size of the vector to calculate the average

    return lf_Average; // Return the calculated average
}

void SlidingWindow::Push(float pf_Value)
{
    // Inserts the pf_Value at the beginning of the Values vector
    this->Values->insert(this->Values->begin(), pf_Value);
    
    // Checks if the size of the Values vector is greater than the size variable
    if (static_cast<int>(this->Values->size()) > this->size)
    {
        // Calls the Pop() function to remove the last element from the vector
        this->Pop();
    }
}

void window::Push(vector<float>* pf_Value)
{

}

void SlidingWindow::Pop()
{
    // Removes the last element from the Values vector
    this->Values->pop_back();
}

bool DSP::SlidingWindowAverage(vector<float>* signal, int groupSize)
{
    int li_DataSize = signal->size();

    for (int C1 = 0; C1 < li_DataSize - groupSize + 1;C1++)
    {
        float lf_Sum = 0.0;
        for (int C2 = C1; C2 < C1 + groupSize; C2++)
        {
            lf_Sum += signal->at(C2);
        }
        float lf_Average = lf_Sum / groupSize;

        (*signal)[C1] = lf_Average;
    }
    return true;
}

void DSP::GroupedSamplesAverage(vector<float>* signal, vector<vector<float>>* parr_StoredSignals, int groupSize)
{
    parr_StoredSignals->insert(parr_StoredSignals->begin(),*signal);

    if (static_cast<int>(parr_StoredSignals->size()) > groupSize)
    {
        parr_StoredSignals->pop_back();

        for (size_t C1 = 0; C1 < parr_StoredSignals->at(0).size(); C1++)
        {
            float lf_Sum = 0.0;
            for (size_t C2 = 0; C2 < parr_StoredSignals->size(); C2++)
            {
                lf_Sum += (*parr_StoredSignals)[C2][C1];
            }
            float lf_Average = lf_Sum / parr_StoredSignals->size();
            (*signal)[C1] = lf_Average;
        }
    }
}

void DSP::RemoveOutliers(vector<float>* signal, int tuner)
{

}

void DSP::LowPassFilter(vector<float>* signal, int tuner)
{

}

void DSP::SmoothingFilterFirst(vector<float>* signal, int tuner)
{
    float lf_CurrentValue = (*signal)[0]; // Variable to store the current value, initialized with the first value of the signal

    for (int C1 = 1; C1 < static_cast<int>(signal->size()); C1++)
    {
        lf_CurrentValue = ((*signal)[C1] * (1 - tuner) + (lf_CurrentValue * tuner)); // Calculate the smoothed value using the Formula function
        (*signal)[C1] = lf_CurrentValue; // Replace the current signal value with the smoothed value
    }

}

void DSP::SmoothingFilterZero(vector<float>* signal, int tuner)
{
    float lf_CurrentValue = 0; // Variable to store the current value during smoothing, initially set to 0

    for (int C1 = 0; C1 < static_cast<int>(signal->size()); C1++)
    {
        lf_CurrentValue = (((*signal)[C1] * (1 - tuner) + lf_CurrentValue * tuner)); // Calculate the smoothed value using the Formula function
        (*signal)[C1] = lf_CurrentValue; // Replace the current value in the signal vector with the smoothed value
    }

}

void DSP::SmoothingFilterAverage(vector<float>* signal, float tuner)
{
    tuner /= 100;
    float lf_CurrentValue = 0; // Variable to store the current value during smoothing, initially set to 0

    lf_CurrentValue = VectorAverage(signal); // Calculate the initial current value by calling the VectorAverage function

    for (int C1 = 0; C1 < static_cast<int>(signal->size()); C1++)
    {
        lf_CurrentValue = (((*signal)[C1] * (1 - tuner) + lf_CurrentValue * tuner));  // Calculate the smoothed value using the SmoothingFormula function
        (*signal)[C1] = lf_CurrentValue; // Replace the vector's value with its smoothed current value
    }

}

void DSP::BandRejectFFT(vector<complexNum>* fftSignal, int start, int stop)
{
    if(stop <= start)                   return;
    if(start < 0)                       return;
    if(stop > (int)fftSignal->size())   return;

    for(int x = start; x < stop ; x++)
    {
        fftSignal->at(x).real = 0;
        fftSignal->at(x).imag = 0;
    }
}

void DSP::BandPassFFT(vector<complexNum>* fftSignal, int start, int stop)
{
    if(stop <= start)                   return;
    if(start < 0)                       return;
    if(stop > (int)fftSignal->size())   return;

    for(int x = 0; x < start; x++)
    {
        fftSignal->at(x).real = 0;
        fftSignal->at(x).imag = 0;
    }

    for(int x = stop; x < (int)fftSignal->size(); x++)
    {
        fftSignal->at(x).real = 0;
        fftSignal->at(x).imag = 0;
    }
}

