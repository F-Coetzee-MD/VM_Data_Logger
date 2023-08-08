#include "./fft_ifft.hpp"

#define pi 3.1415926535

float w[2];

float* fft::W(int k, int m)
{
    /*if m equals 0*/
    if (m == 2)
    {
        w[0] = 1;
        w[1] = 0;
        return &w[0];
    } 

    /*if m is larger than 0*/
    else
    {
        float val = 2*k*pi/m;

        w[0] = cos(val);
        w[1] = -sin(val);
        return &w[0];
    }
}

vector<float> fft::MAG(vector<complexNum> list)
{
    vector<float> mag;
    
    for (int i = 0; i < (int)list.size(); i++)
        mag.push_back(sqrt(list.at(i).real*list.at(i).real + list.at(i).imag*list.at(i).imag));
    
    return mag;
}

vector<float> fft::PHA(vector<complexNum> list)
{
    vector<float> pha;
    
    for (int i = 0; i < (int)list.size(); i++)
        pha.push_back(atan(list.at(i).real/list.at(i).imag));
    
    return pha;
}

vector<complexNum> fft::FFT(vector<float> data)
{
    vector<float> odd;
    vector<float> even;
    vector<vector<float>> temp2D;
    vector<vector<float>> signals;
    vector<complexNum> fftSignal;
    
    float val = log2(data.size());

    while (ceil(val) != floor(val)) // add the avarage value instead of 0.....
    {
        data.push_back(0);
        val = log2(data.size());
    }

    signals.push_back(data);

    while (signals.at(0).size() > 1)
    {
        temp2D = signals;
        signals.clear();

        for (int i = 0; i < (int)temp2D.size(); i++)
        {
            even.clear();
            odd.clear();

            int counter = 0;

            /*get even parts of signal*/
            for (int e = 0; e < (int)(temp2D.at(i).size())/2; e++)
            {
                even.push_back(temp2D.at(i).at(counter));
                counter += 2;
            }

            counter = 1;

            /*get odd parts of signal*/
            for (int o = 0; o < (int)(temp2D.at(i).size())/2; o++)
            {
                odd.push_back(temp2D.at(i).at(counter));
                counter += 2;
            }

            signals.push_back(even);
            signals.push_back(odd);
        }
    }

    vector<float> realA;
    vector<float> imagA;
    vector<float> realB;
    vector<float> imagB;

    for (int i = 0; i < (int)signals.size(); i++)
    {
        realA.push_back(signals.at(i).at(0)); // look into this later
        imagA.push_back(0);
    }
    
    /*actually compute the fft*/

    int containerSize           = 2;
    int containerInnerPosition  = 0;
    int currentContainerNumber;

    int a_Temp;
    int a_Value;
    int b_Value;
    int c_Value;
    int k_Value;
    int m_Value;

    bool c_Flag = false;

    float* w;

    while (containerSize <= (int)signals.size())
    {
        a_Temp  = 0;
        a_Value = 0;
        k_Value = 0;
        m_Value = containerSize;
        currentContainerNumber = 0;

        for (int i = 0; i < (int)signals.size(); i++)
        {
            if (containerInnerPosition >= (int)containerSize/2) b_Value = -1;   

            else b_Value = 1;

            c_Value = a_Value + containerSize/2;

            /*this is where the magic happens*/
            /**************************************************************************************/
            w = this->W(k_Value, m_Value);
            realB.push_back(realA.at(a_Value) + b_Value*(*w*realA.at(c_Value) - *(w+1)*imagA.at(c_Value)));
            imagB.push_back(imagA.at(a_Value) + b_Value*(*w*imagA.at(c_Value) + *(w+1)*realA.at(c_Value)));
            /**************************************************************************************/

            a_Value++;
            k_Value++;
            containerInnerPosition++;

            if ((containerInnerPosition >= (int)containerSize/2)&&(c_Flag == false))
            {
                a_Value = a_Temp;
                c_Flag = true;
            }

            if (k_Value >= (int)containerSize/2) k_Value = 0;
            
            if (containerInnerPosition >= containerSize)
            {
                containerInnerPosition = 0;  
                a_Temp += containerSize;
                a_Value = a_Temp;
                currentContainerNumber++;
                c_Flag = false;
            }
        }

        for (int i = 0; i < (int)signals.size(); i++)
        {
            realA.at(i) = realB.at(i);
            imagA.at(i) = imagB.at(i);
        }

        containerSize *= 2;

        if(containerSize > (int)signals.size()) break;
        
        realB.clear();
        imagB.clear();
    }

    complexNum freqBin;

    for (int i = 0; i < (int)signals.size(); i++)
    {
        freqBin.real = realB.at(i);
        freqBin.imag = imagB.at(i);
        fftSignal.push_back(freqBin);
    }

    return fftSignal;
}

vector<float> fft::IFFT(vector<complexNum> data)
{
    vector<complexNum> odd;
    vector<complexNum> even;
    vector<float> ifftSignal;
    vector<vector<complexNum>> temp2D;
    vector<vector<complexNum>> signals;

    for (int i = 0; i < (int)data.size(); i++)
        data.at(i).imag *= -1;
    
    signals.push_back(data);

    while (signals.at(0).size() > 1)
    {
        temp2D = signals;
        signals.clear();

        for (int i = 0; i < (int)temp2D.size(); i++)
        {
            even.clear();
            odd.clear();

            int counter = 0;

            /*get even parts of signal*/
            for (int e = 0; e < (int)(temp2D.at(i).size())/2; e++)
            {
                even.push_back(temp2D.at(i).at(counter));
                counter += 2;
            }

            counter = 1;

            /*get odd parts of signal*/
            for (int o = 0; o < (int)(temp2D.at(i).size())/2; o++)
            {
                odd.push_back(temp2D.at(i).at(counter));
                counter += 2;
            }

            signals.push_back(even);
            signals.push_back(odd);
        }
    }

    vector<float> realA;
    vector<float> imagA;
    vector<float> realB;
    vector<float> imagB;

    for (int i = 0; i < (int)signals.size(); i++)
    {
        realA.push_back(signals.at(i).at(0).real); // look into this later
        imagA.push_back(signals.at(i).at(0).imag);
    }
    
    /*actually compute the fft*/

    int containerSize           = 2;
    int containerInnerPosition  = 0;
    int currentContainerNumber;

    int a_Temp;
    int a_Value;
    int b_Value;
    int c_Value;
    int k_Value;
    int m_Value;

    bool c_Flag = false;

    float* w;

    while (containerSize <= (int)signals.size())
    {
        a_Temp  = 0;
        a_Value = 0;
        k_Value = 0;
        m_Value = containerSize;
        currentContainerNumber = 0;

        for (int i = 0; i < (int)signals.size(); i++)
        {
            if (containerInnerPosition >= (int)containerSize/2) b_Value = -1;   

            else b_Value = 1;

            c_Value = a_Value + containerSize/2;

            /*this is where the magic happens*/
            /**************************************************************************************/
            w = this->W(k_Value, m_Value);
            realB.push_back(realA.at(a_Value) + b_Value*(*w*realA.at(c_Value) - *(w+1)*imagA.at(c_Value)));
            imagB.push_back(imagA.at(a_Value) + b_Value*(*w*imagA.at(c_Value) + *(w+1)*realA.at(c_Value)));
            /**************************************************************************************/

            a_Value++;
            k_Value++;
            containerInnerPosition++;

            if ((containerInnerPosition >= (int)containerSize/2)&&(c_Flag == false))
            {
                a_Value = a_Temp;
                c_Flag = true;
            }

            if (k_Value >= (int)containerSize/2) k_Value = 0;
            
            if (containerInnerPosition >= containerSize)
            {
                containerInnerPosition = 0;  
                a_Temp += containerSize;
                a_Value = a_Temp;
                currentContainerNumber++;
                c_Flag = false;
            }
        }

        for (int i = 0; i < (int)signals.size(); i++)
        {
            realA.at(i) = realB.at(i);
            imagA.at(i) = imagB.at(i);
        }

        containerSize *= 2;

        if(containerSize > (int)signals.size()) break;
        
        realB.clear();
        imagB.clear();
    }

    int size = (int)realB.size();

    for (int i = 0; i < (int)realB.size(); i++)
        ifftSignal.push_back(realB.at(i)/size);  

    return ifftSignal;
}