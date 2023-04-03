#ifndef DATABASE_H
#define DATABASE_H
#include <iostream>
#include <string.h>
#include <opencv2/opencv.hpp>
#include <cmath>
#include "include/calculate.h"



//imgSrc.cols, ---> width
//imgSrc.rows, ---> height

class DataBase
{
public:
    DataBase();
    ~DataBase(){};

    // settr
    void set_shape();
    void set_shape(const double &cols, const double &rows, const int &channel);
    void set_origin_ratio_rate(const double &rate);
    void set_ratio(const double &rate);
    void set_ratio(const int &value);
    void set_ratio(const double &rate, const int &value);
    void rest_ratio();

    // gettr
    double get_orig_width();
    double get_orig_height();
    int get_ratio_value();
    double get_ratio_rate();
    double get_old_ratio_rate();


private:
    // image info
    int orig_channel;
    double orig_width, orig_height;


    // zoom in out
    double origin_ratio_rate = 0;
    double ratio_rate = 0;
    double old_ratio_rate = 0;
    int ratio_value = 0;

};

#endif // DATABASE_H
