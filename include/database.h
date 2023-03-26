#ifndef DATABASE_H
#define DATABASE_H
#include <iostream>
#include <string.h>
#include <opencv2/opencv.hpp>
#include <cmath>
#include "include/calculate.h"



//imgSrc.cols, ---> width
//imgSrc.rows, ---> height

class Database
{
public:
    Database();
    // settr
    void set_imgshape();
    void set_imgshape(double cols, double rows, int channel);
    void set_origin_ratio_rate(double rate);
    void set_ratio(double rate);
    void set_ratio(int value);
    void set_ratio(double rate, int value);
    void rest_ratio();

    ~Database(){};

    // gettr
    double get_orig_w();
    double get_orig_h();
    int get_ratio_value();
    double get_ratio_rate();
    double get_old_ratio_rate();


    // function


    bool signal_open_img = false;

private:
    // image info
    int orig_c=3;
    double orig_w=0, orig_h=0;


    // zoom in out
    double origin_ratio_rate=0;
    double ratio_rate=0;
    double old_ratio_rate=0;
    int ratio_value = 0;

};

#endif // DATABASE_H
