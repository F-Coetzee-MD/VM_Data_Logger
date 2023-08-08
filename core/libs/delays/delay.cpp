#include "./delay.hpp"

void delay::microSeconds(int time)
{
    usleep(time);
}

void delay::milliSeconds(int time)
{
     usleep(time*1000);
}

void delay::seconds(int time)
{
    usleep(time*1000000);
}