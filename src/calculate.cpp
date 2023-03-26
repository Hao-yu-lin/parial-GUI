#include "../include/calculate.h"
#include <cmath>


int rate_to_value(double rate){
    return std::ceil(std::log10(rate) * 50) + 50;
}

double value_to_rate(int rate){
    return std::pow(10, double(rate - 50)/50);
}

