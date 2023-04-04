#include "../include/database.h"

DataBase::DataBase()
{
    set_shape();
}

void DataBase::set_shape(){
    orig_width = 0;
    orig_height = 0;
    orig_channel = 0;

}

void DataBase::set_shape(const double &cols, const double &rows, const int &channel){
    orig_width = cols;
    orig_height = rows;
    orig_channel = channel;

}

void DataBase::set_origimg(const QImage &img)
{
    orig_qimage = img.copy();
}

void DataBase::set_origin_ratio_rate(const double &rate){
    std::cout << "rate:" <<rate << std::endl;
    origin_ratio_rate = rate;
    old_ratio_rate = rate;
    ratio_rate = rate;
}

void DataBase::set_ratio(const double &rate){
    old_ratio_rate = ratio_rate;
    ratio_rate = rate;
    ratio_value = rate_to_value(rate);
}

void DataBase::set_ratio(const int &value){
    ratio_value = value;
    old_ratio_rate = ratio_rate;
    ratio_rate = value_to_rate(value);
}

void DataBase::set_ratio(const double &rate, const int &value){
    ratio_rate = rate;
    old_ratio_rate = ratio_rate;
    ratio_value = value;
}

void DataBase::rest_ratio(){
    ratio_rate = origin_ratio_rate;
    old_ratio_rate = origin_ratio_rate;
    ratio_value = rate_to_value(ratio_rate);
}

void DataBase::set_refer_point(const DataPoint &pos)
{
    if(refer_point_vector.size() < 2){
        refer_point_vector.push_back(pos);
    }else{
        refer_point_vector[1] = pos;
    }

}

const double& DataBase::get_orig_width()
{
    return orig_width;
}

const double& DataBase::get_orig_height()
{
    return orig_height;
}

const QImage &DataBase::get_orig_img()
{
    return orig_qimage;
}

const int& DataBase::get_ratio_value(){
    return ratio_value;
}

const double& DataBase::get_ratio_rate(){
    return ratio_rate;
}

const double& DataBase::get_old_ratio_rate(){
    return old_ratio_rate;
}


const std::vector<DataPoint>& DataBase::get_refer_vector(){
    return refer_point_vector;
}




