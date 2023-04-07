#include "../include/database.h"

DataBase::DataBase()
{
    set_shape();
}

/*  ----------- image information -----------  */
void DataBase::set_shape(){
    this->orig_width = 0;
    this->orig_height = 0;
    this->orig_channel = 0;

}

void DataBase::set_shape(const double &cols, const double &rows, const int &channel){
    this->orig_width = cols;
    this->orig_height = rows;
    this->orig_channel = channel;

}

const double& DataBase::get_orig_width() const
{
    return orig_width;
}

const double& DataBase::get_orig_height() const
{
    return orig_height;
}

void DataBase::set_origimg(const QImage &orig_qimage)
{
    this->orig_qimage = orig_qimage.copy();
}

const QImage &DataBase::get_orig_img() const
{
    return orig_qimage;
}

/*  ----------- image zoom in/out -----------  */

void DataBase::set_origin_ratio_rate(const double &ratio_rate){
//    std::cout << "rate:" <<rate << std::endl;
    this->origin_ratio_rate = ratio_rate;
    this->old_ratio_rate = ratio_rate;
    this->ratio_rate = ratio_rate;
}

void DataBase::set_ratio(const double &ratio_rate){
    this->old_ratio_rate = this->ratio_rate;
    this->ratio_rate = ratio_rate;
    ratio_value = cal::rate_to_value(ratio_rate);
}

void DataBase::set_ratio(const int &ratio_value){
    this->ratio_value = ratio_value;
    this->old_ratio_rate = this->ratio_rate;
    this->ratio_rate = cal::value_to_rate(ratio_value);
}

void DataBase::set_ratio(const double &ratio_rate, const int &ratio_value){
    this->ratio_rate = ratio_rate;
    this->old_ratio_rate = ratio_rate;
    this->ratio_value = ratio_value;
}

void DataBase::rest_ratio(){
    ratio_rate = origin_ratio_rate;
    old_ratio_rate = origin_ratio_rate;
    ratio_value = cal::rate_to_value(ratio_rate);
}

const int& DataBase::get_ratio_value() const
{
    return ratio_value;
}

const double& DataBase::get_ratio_rate() const
{
    return ratio_rate;
}

const double& DataBase::get_old_ratio_rate() const
{
    return old_ratio_rate;
}

/*  ----------- reference object -----------  */

void DataBase::set_refer_point(const cv::Point2i &pos)
{
    if(refer_point_vector.size() < 2){
        refer_point_vector.push_back(pos);
    }else{
        refer_point_vector[1] = pos;
    }
}

const std::vector<cv::Point2i>& DataBase::get_refer_vector() const
{

    return refer_point_vector;
}

void DataBase::set_pixel_sacle(const double &pixel_sacle)
{
    this->pixel_sacle = pixel_sacle;
}

const double& DataBase::get_pixel_sacle() const
{
    return pixel_sacle;
}

void DataBase::del_refer_vector()
{
    if(refer_point_vector.size() != 0)
    {
        refer_point_vector.pop_back();
    }
}

/*  ----------- Particle -----------  */

void DataBase::set_detect_point(const cv::Point2i &pos)
{
   detect_point_vector.push_back(pos);
}

const std::vector<cv::Point2i> &DataBase::get_detect_vector() const
{
    return detect_point_vector;
}

void DataBase::del_detect_vector()
{
    if(detect_point_vector.size() != 0)
    {
        detect_point_vector.pop_back();
    }
}

void DataBase::del_all_detect_vector()
{
    detect_point_vector.clear();
    std::vector<cv::Point2i>().swap(detect_point_vector);
}

void DataBase::set_threshold(const cv::Mat &newThreshold)
{
    threshold = newThreshold;
}

const cv::Mat& DataBase::get_threshold() const
{
    return threshold;
}

void DataBase::del_threshold()
{
    threshold = cv::Mat();
}


