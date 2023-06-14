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
//    double x = std::pow((p1.x - p2.x), 2);
//    double y = std::pow((p1.y - p2.y), 2);
//    return std::sqrt(x+y);
    double x = p1.x - p2.x;
    double y = p1.y - p2.y;
    return std::hypot(x, y);
}

double diameterfix(const double &pixel_scal, const double &diameter)
{
    double a = 1.0/(pixel_scal + 0.25);
    double t = std::pow(2.88, a);
    double f = -0.375 * (1.0/t) + 0.0075;

    double p = 2.453 * pixel_scal + 0.92;
    double diameter2 = diameter/100.0;
    double new_sacle = f * diameter2 + p;

    return diameter * new_sacle;
}

double surfacefix(const double &pixel_scal, const double &surface)
{
//    double a = 1.0/(pixel_scal + 0.25);
//    double t = std::pow(2.68, a);
//    double f = -0.555 * (1.0/t) + 0.012;

//    double p = 5.4521* pixel_scal + 0.8309;
//    double surface2 = surface*11.0;
//    double new_sacle = f * surface2 + p;

//    return surface * new_sacle;
    double a = 1.0/(pixel_scal + 0.25);
    double t = std::pow(2.8, a);
    double f = -0.53 * (1.0/t) + 0.012;

    double p = 5.4521* pixel_scal + 0.8399;
    double surface2 = surface*10.0;
    double new_sacle = f * surface2 + p;

    return surface * new_sacle;
}


}
