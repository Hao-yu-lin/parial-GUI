#ifndef DATABASE_H
#define DATABASE_H

#include "calculate.h"

#include <iostream>
#include <string.h>
#include <opencv2/core/types.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <vector>
#include <cstdio>
#include <QImage>


//imgSrc.cols, ---> width ----> x
//imgSrc.rows, ---> height ----> y


class DataBase
{
public:
    DataBase();
    ~DataBase(){};

    /* ------------- Settr ------------- */
    // settr img
    void set_shape();
    void set_shape(const double &cols, const double &rows, const int &channel);
    void set_origimg(const QImage &img);

    // settr ratio
    void set_origin_ratio_rate(const double &ratio_rate);
    void set_ratio(const double &ratio_rate);
    void set_ratio(const int &ratio_value);
    void set_ratio(const double &ratio_rate, const int &ratio_value);
    void rest_ratio();
    void set_pixel_sacle(const double &pixel_sacle);

    // settr array
    void set_refer_point(const cv::Point2i &pos);
    void set_detect_point(const cv::Point2i &pos);
    void set_threshold(const cv::Mat &newThreshold);

    /* ------------- Gettr ------------- */
    // gettr img
    const double& get_orig_width() const;
    const double& get_orig_height() const;
    const QImage& get_orig_img() const;

    // gettr ratio
    const int& get_ratio_value() const;
    const double& get_ratio_rate() const;
    const double& get_old_ratio_rate() const;
    const double& get_pixel_sacle() const;

    // gettr array
    const std::vector<cv::Point2i>& get_refer_vector() const;
    const std::vector<cv::Point2i>& get_detect_vector() const;
    const cv::Mat& get_threshold() const;


    // del array operate
    void del_refer_vector();
    void del_detect_vector();
    void del_all_detect_vector();
    void del_threshold();



private:
    // image info
    int orig_channel;
    double orig_width, orig_height;
    QImage orig_qimage;


    // zoom in out
    double origin_ratio_rate = 0;
    double ratio_rate = 0;
    double old_ratio_rate = 0;
    int ratio_value = 0;

    // cal
    double pixel_sacle=1;

    // points array
    std::vector<cv::Point2i> refer_point_vector;
    std::vector<cv::Point2i> detect_point_vector;
    cv::Mat threshold;
};

#endif // DATABASE_H
