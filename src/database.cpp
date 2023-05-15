#include "../include/database.h"

DataBase::DataBase(setting_t *input_flag):set_flag(input_flag)
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

const QString &DataBase::get_filename() const
{
    return fileName;
}

void DataBase::set_filename(const QString &newFileName)
{
    fileName = newFileName;
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

const std::vector<cv::Point2i>* DataBase::get_refer_vector() const
{
    return &refer_point_vector;
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

const std::vector<cv::Point2i> *DataBase::get_detect_vector() const
{
    return &detect_point_vector;
}

void DataBase::del_detect_vector()
{
    if(detect_point_vector.size() != 0)
    {
        detect_point_vector.pop_back();
    }
}

void DataBase::del_detect_vector(cv::Point2i pos)
{
    double pos_value;
    for(auto it = detect_contours.begin(); it != detect_contours.end();)
    {
        // 0 onside 1 inside -1 outside
        pos_value = cv::pointPolygonTest( *it, pos, false);
        if(pos_value == 1 || pos_value == 0)
        {
            it = detect_contours.erase(it);
            if(detect_contours.empty()){
                set_flag->flag_contours = false;
            }
            return;
        }else{
            ++it;
        }
    }
}

void DataBase::del_all_detect_vector()
{
    if(this->detect_point_vector.empty()){
        return;
    }
    detect_point_vector.clear();
    std::vector<cv::Point2i>().swap(detect_point_vector);
}

/*  ----------- Contours -----------  */

void DataBase::set_threshold(const cv::Mat &threshold)
{
    this->threshold = threshold.clone();
}

const cv::Mat *DataBase::get_threshold() const
{
    return &threshold;
}

void DataBase::del_threshold()
{
    threshold = cv::Mat();
}

const std::vector<std::vector<cv::Point> > *DataBase::get_detect_contours() const
{
    return &detect_contours;
}

void DataBase::set_detect_contours(const std::vector<std::vector<cv::Point> > &detect_contours)
{
    this->detect_contours.assign(detect_contours.begin(), detect_contours.end());
    set_flag->flag_contours = true;
}

void DataBase::del_contours(){
    if(this->detect_contours.empty()){
        return;
    }
    this->detect_contours.clear();
    std::vector<std::vector<cv::Point>>().swap(this->detect_contours);
    set_flag->flag_contours = false;
}

/*  ----------- Area -----------  */

const std::vector<float> *DataBase::get_data1_area() const
{
    return &data1_area;
}

void DataBase::set_data1_area(const float &area)
{
    this->data1_area.push_back(area);
}

void DataBase::del_area()
{
    this->data1_area.clear();
    std::vector<float>().swap(this->data1_area);
}

void DataBase::sort_area()
{
    std::sort(this->data1_area.begin(), this->data1_area.end());
}

/*  ----------- diameter -----------  */

const std::vector<float> *DataBase::get_data1_diameter() const
{
    return &data1_diameter;
}

void DataBase::set_data1_diameter(const float &area)
{
    this->data1_diameter.push_back(area);
    set_flag->flag_data1 = true;
}

void DataBase::del_diameter()
{
    this->data1_diameter.clear();
    std::vector<float>().swap(this->data1_diameter);
}

void DataBase::sort_diameter()
{
    std::sort(this->data1_diameter.begin(), this->data1_diameter.end());
}

/*  ----------- Histogram -----------  */

void DataBase::set_hist_qimage(const QImage &hist_img)
{
    this->hist_qimage = hist_img.copy();
    this->hist_width = hist_img.width();
    this->hist_height = hist_img.height();
}

const double &DataBase::get_hist_width() const
{
    return this->hist_width;
}

const double &DataBase::get_hist_height() const
{
    return this->hist_height;
}

const QImage &DataBase::get_hist_img() const
{
    return hist_qimage;
}

void DataBase::del_hist_qimg(){
    hist_qimage = QImage();
}

/*  ----------- Histogram2 -----------  */

void DataBase::set_hist2_qimage(const QImage &hist_img)
{
    this->hist2_qimage = hist_img.copy();
    this->hist2_width = hist_img.width();
    this->hist2_height = hist_img.height();
}

const double &DataBase::get_hist2_width() const
{
    return this->hist2_width;
}

const double &DataBase::get_hist2_height() const
{
    return this->hist2_height;
}

const QImage &DataBase::get_hist2_img() const
{
    return hist2_qimage;
}

void DataBase::del_hist2_qimg(){
    hist2_qimage = QImage();
}

/*  ----------- Histogram3 -----------  */

void DataBase::set_hist3_qimage(const QImage &hist_img)
{
    this->hist3_qimage = hist_img.copy();
    this->hist3_width = hist_img.width();
    this->hist3_height = hist_img.height();
}

const double &DataBase::get_hist3_width() const
{
    return this->hist3_width;
}

const double &DataBase::get_hist3_height() const
{
    return this->hist3_height;
}

const QImage &DataBase::get_hist3_img() const
{
    return hist3_qimage;
}

void DataBase::del_hist3_qimg(){
    hist3_qimage = QImage();
}

/*  ----------- Particle -----------  */

void DataBase::set_particle(const int &idx, const float &surface, const float &diameter)
{
   particle_t data;
   data.idx = idx;
   data.surface = surface;
   data.diameter = diameter;
   particle_data.push_back(data);
}

const std::vector<particle_t>* DataBase::get_particle_data() const
{
    return &particle_data;
}


