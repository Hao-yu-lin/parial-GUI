#ifndef CALCULATE_H
#define CALCULATE_H

namespace cal {
struct DataPoint
{
    int x;
    int y;
};

int rate_to_value(const double &ratio);
double value_to_rate(const int &value);
double dist(const DataPoint &p1, const DataPoint &p2);
}



#endif // CALCULATE_H
