#include "../include/calculate.h"
#include <cmath>

namespace cal {
int rate_to_value(const double &rate){
    return std::ceil(std::log10(rate) * 50) + 50;
}

double value_to_rate(const int &rate){
    return std::pow(10, double(rate - 50)/50);
}


double dist(const cv::Point2i &p1, const cv::Point2i &p2)
{
    double x = std::pow((p1.x - p2.x), 2);
    double y = std::pow((p1.y - p2.y), 2);
    return std::sqrt(x+y);
}
}
