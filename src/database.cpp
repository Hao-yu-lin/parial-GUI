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

double DataBase::get_orig_width(){
    return orig_width;
}

double DataBase::get_orig_height(){
    return orig_height;
}

int DataBase::get_ratio_value(){
    return ratio_value;
}

double DataBase::get_ratio_rate(){
    return ratio_rate;
}

double DataBase::get_old_ratio_rate(){
    return old_ratio_rate;
}



