#include "../include/database.h"

Database::Database()
{
    set_imgshape();
}

void Database::set_imgshape(){}

void Database::set_imgshape(double cols, double rows, int channel){
    orig_w = cols;
    orig_h = rows;
    orig_c = channel;
    signal_open_img = true;
}

void Database::set_origin_ratio_rate(double rate){
    origin_ratio_rate = rate;
    old_ratio_rate = rate;
    ratio_rate = rate;
}

void Database::set_ratio(double rate){
    old_ratio_rate = ratio_rate;
    ratio_rate = rate;
    ratio_value = rate_to_value(rate);
}

void Database::set_ratio(int value){
    ratio_value = value;
    old_ratio_rate = ratio_rate;
    ratio_rate = value_to_rate(value);
}

void Database::set_ratio(double rate, int value){
    ratio_rate = rate;
    old_ratio_rate = ratio_rate;
    ratio_value = value;
}

void Database::rest_ratio(){
    ratio_rate = origin_ratio_rate;
    old_ratio_rate = origin_ratio_rate;
    ratio_value = rate_to_value(ratio_rate);
}

double Database::get_orig_w(){
    return orig_w;
}

double Database::get_orig_h(){
    return orig_h;
}

int Database::get_ratio_value(){
    return ratio_value;
}

double Database::get_ratio_rate(){
    return ratio_rate;
}

double Database::get_old_ratio_rate(){
    return old_ratio_rate;
}



