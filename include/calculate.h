#ifndef CALCULATE_H
#define CALCULATE_H
#include <opencv2/core/types.hpp>

namespace cal {
int rate_to_value(const double &ratio);
double value_to_rate(const int &value);
double dist(const cv::Point2i &p1, const cv::Point2i &p2);
}



#endif // CALCULATE_H
