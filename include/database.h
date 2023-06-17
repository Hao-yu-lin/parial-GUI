#ifndef DATABASE_H
#define DATABASE_H

#include "calculate.h"
#include "controlstruct.h"

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
    DataBase(setting_t *input_flag);
    ~DataBase(){};

    /* ------------- Settr ------------- */
    // settr img
    void set_shape();
    void set_shape(const double &cols, const double &rows, const int &channel);
    void set_origimg(const QImage &img);
    void set_filename(const QString &newFileName);
    void set_contours_image(const QImage &img);

    // settr hist_img
    void set_hist_qimage(const QImage &hist_img);

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
    void set_threshold(const cv::Mat &threshold);
    void set_detect_contours(const std::vector<std::vector<cv::Point> > &detect_contours);


    // settr data
    void set_data_value(const statis_t& data, float data_value);
    void set_data_statis(const statis_t& data, float avg, float sd, float mode, float d20, float d50, float d70, float d75, int cnt);
    void set_particle(const int &idx, const float &surface, const float &diameter);

    /* ------------- Gettr ------------- */
    // gettr img
    const double& get_orig_width() const;
    const double& get_orig_height() const;
    const QImage& get_orig_img() const;
    const QImage& get_contours_img() const;
    const QString &get_filename() const;

    //gettr his_img
    const double& get_hist_width() const;
    const double& get_hist_height() const;
    const QImage& get_hist_img() const;

    // gettr ratio
    const int& get_ratio_value() const;
    const double& get_ratio_rate() const;
    const double& get_old_ratio_rate() const;
    const double& get_pixel_sacle() const;

    // gettr array
    const std::vector<cv::Point2i>* get_refer_vector() const;
    const std::vector<cv::Point2i>* get_detect_vector() const;
    const cv::Mat* get_threshold() const;
    const std::vector<std::vector<cv::Point> >* get_detect_contours() const;

    // gettr data
    const statis_t *get_data1_area() const;
    const statis_t *get_data1_diameter() const;
    const statis_t *get_data2_area() const;
    const statis_t *get_data2_diameter() const;

    // gettr particle
//    const std::vector<particle_t>* get_particle_data() const;


    // del array operate
    void del_refer_vector();
    void del_detect_vector();
    void del_detect_vector(cv::Point2i pos);
    void init_detect_vector();
    void del_threshold();
    void del_contours();
    void del_hist_qimg();

    void init_statis_t(const statis_t& data);
//    void sort_statis_t(const statis_t& data);

    const std::vector<particle_t>* get_particle_data() const;



private:

    setting_t *set_flag;

    // image info
    QString fileName;

    int orig_channel;
    double orig_width, orig_height;
    QImage orig_qimage;
    QImage contours_qimage;

    double hist_width, hist_height;
    QImage hist_qimage;


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
    std::vector<std::vector<cv::Point>> detect_contours;

    // contours
//    std::vector<float> data1_area;
//    std::vector<float> data1_diameter;
    std::vector<particle_t> particle_data1;
    statis_t data1_area;
    statis_t data1_diameter;
    statis_t data2_area;
    statis_t data2_diameter;


};

#endif // DATABASE_H
