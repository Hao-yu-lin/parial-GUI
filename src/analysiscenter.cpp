#include "../include/analysiscenter.h"
#include "./ui_mainwindow.h"

AnalysisCenter::AnalysisCenter(Ui::MainWindow *input_ui, ImageCenter *input_imgCenter)
    :ui(input_ui)
    , imgCenter(input_imgCenter)
    , dataBase(input_imgCenter->dataBase)
{
}

void AnalysisCenter::test()
{
//    const std::vector<cv::Point2i>& test = dataBase->get_refer_vector();
}

void AnalysisCenter::draw_img(const std::vector<cv::Point2i> &vector_pts, cv::Scalar color)
{
    cv::Mat tmp_img = imgCenter->imgSrc.clone();
    update_pts_img(tmp_img, vector_pts, color);
}

void AnalysisCenter::draw_img(const std::vector<cv::Point2i> &vector_pts)
{
    cv::Mat imgsrc = imgCenter->imgSrc.clone();
    QImage tmp_img(imgsrc.data,
               imgsrc.cols, // width
               imgsrc.rows, // height
               imgsrc.step,
               QImage::Format_RGB888);

    dataBase->set_origimg(tmp_img);
    imgCenter->set_img();
}

void AnalysisCenter::update_pts_img(cv::Mat &imgsrc, const std::vector<cv::Point2i> &vector_pts, const cv::Scalar color)
{
    if(vector_pts.size() > 0)
    {
        double ratio = std::ceil(dataBase->get_ratio_rate() * 100);

        int linethickness = 1;

        if(ratio < 20)
        {
            linethickness = 8;
        }
        else if(ratio >= 20 && ratio < 50)
        {
            linethickness = 6;
        }
        else if(ratio >= 50 && ratio < 80)
        {
            linethickness = 3;
        }else if(ratio >= 80 && ratio < 120)
        {
           linethickness = 2;
        }else
        {
            linethickness = 1;
        }

        cv::Point pts(vector_pts[0].x, vector_pts[0].y);
        cv::circle(imgsrc, pts, 10, color, linethickness);
        for(int i = 1; i < vector_pts.size(); i++)
        {
            cv::Point pts_1(vector_pts[i-1].x, vector_pts[i-1].y);
            cv::Point pts_2(vector_pts[i].x, vector_pts[i].y);
            cv::circle(imgsrc, pts_2, 10, color, linethickness);
            cv::line(imgsrc, pts_1, pts_2, color, linethickness);
        }
    }

    QImage tmp_img(imgsrc.data,
               imgsrc.cols, // width
               imgsrc.rows, // height
               imgsrc.step,
               QImage::Format_RGB888);

    dataBase->set_origimg(tmp_img);
    imgCenter->set_img();

}

/*
flag num:
Reference Object     --->   flag_num = 1
Selection Particle   --->   flag_num = 2
*/

void AnalysisCenter::set_pts_vector(const QPointF &img_pos, const int &flag_num)
{
    auto w_d = (ui->label_image->width() - imgCenter->qimg_img.width())/2.0;
    auto h_d = (ui->label_image->height() - imgCenter->qimg_img.height()) / 2.0;

    auto x = img_pos.x()-w_d;
    auto y = img_pos.y()-h_d;

    if( x < 0 || x > imgCenter->qimg_img.width() || y < 0 || y > imgCenter->qimg_img.height()) return;

    cv::Point2i pos;
    pos.x = x / dataBase->get_ratio_rate();
    pos.y = y / dataBase->get_ratio_rate();

//    if(flag_num == 1){
//        dataBase->set_refer_point(pos);
//        draw_img(dataBase->get_refer_vector(), cv::Scalar( 0, 255, 0));
//    }else if(flag_num == 2){
//        dataBase->set_detect_point(pos);
//        draw_img(dataBase->get_detect_vector(), cv::Scalar( 0, 0, 255));
//    }else{
//        return;
//    }
    switch (flag_num)
    {
    case 1:
        dataBase->set_refer_point(pos);
        draw_img(*dataBase->get_refer_vector(), cv::Scalar( 0, 255, 0));
        break;
    case 2:
        dataBase->set_detect_point(pos);
        draw_img(*dataBase->get_detect_vector(), cv::Scalar( 0, 0, 255));
        break;
    default:
        break;
    }
}

void AnalysisCenter::del_pts_vector(const int &flag_num)
{
//    if(flag_num == 1)
//    {
//        dataBase->del_refer_vector();
//        draw_img(dataBase->get_refer_vector(), cv::Scalar( 0, 255, 0));
//    }else if(flag_num == 2)
//    {
//        dataBase->del_detect_vector();
//        draw_img(dataBase->get_detect_vector(), cv::Scalar( 0, 0, 255));
//    }
    switch (flag_num)
    {
    case 1:
        dataBase->del_refer_vector();
        draw_img(*dataBase->get_refer_vector(), cv::Scalar( 0, 255, 0));
        break;
    case 2:
        dataBase->del_detect_vector();
        draw_img(*dataBase->get_detect_vector(), cv::Scalar( 0, 0, 255));
        break;
    default:
        break;
    }
}

void AnalysisCenter::reset_refer()
{
    dataBase->del_refer_vector();
    dataBase->del_refer_vector();
    draw_img(*dataBase->get_refer_vector());
}

void AnalysisCenter::cal_refer_obj()
{
    draw_img(*dataBase->get_refer_vector(), cv::Scalar(255,0,0));
    const std::vector<cv::Point2i>*refer_pts = dataBase->get_refer_vector();
    if(refer_pts->size() < 1)
    {
        std::cout << "Need two reference object point" << std::endl;
        return;
    }

    double obj_pixel_len = cal::dist(refer_pts->at(0), refer_pts->at(1)); // (*refer_pts)[0]
    if(!ui->lineEdit_object_length->text().isEmpty())
    {
        double phy_length = ui->lineEdit_object_length->text().toDouble();
        double pixel_scale = phy_length/obj_pixel_len;
        dataBase->set_pixel_sacle(pixel_scale);
        QString text = QStringLiteral("%1 ").arg(pixel_scale, 0, 'f', 3);
        ui->lineEdit_pixel_scale_value->setText(text);
        reset_refer();
    }
    refer_pts = nullptr;
    delete refer_pts;
}

void AnalysisCenter::chose_detect_obj()
{
    roi_mask = cv::Mat();
    bool roi_reverse = ui->checkBox_roi_reverse->isChecked();
    const std::vector<cv::Point2i> *detect_vector = dataBase->get_detect_vector();
    if(detect_vector->size() > 2){
        cv::Point2i pts = dataBase->get_detect_vector()->at(0);
        dataBase->set_detect_point(pts);
        draw_img(*dataBase->get_detect_vector(), cv::Scalar( 255, 0, 0));
            // roi mask
        if(roi_reverse == false)
        {
            roi_mask = cv::Mat(imgCenter->imgSrc.size(), CV_8UC1, cv::Scalar(0));
            cv::polylines(roi_mask, *dataBase->get_detect_vector(), true, cv::Scalar(255));
            cv::fillPoly(roi_mask,  *dataBase->get_detect_vector(), cv::Scalar(255), 8, 0);
        }else
        {
            roi_mask = cv::Mat(imgCenter->imgSrc.size(), CV_8UC1, cv::Scalar(255));
            cv::polylines(roi_mask, *dataBase->get_detect_vector(), true, cv::Scalar(0));
            cv::fillPoly(roi_mask,  *dataBase->get_detect_vector(), cv::Scalar(0), 8, 0);
        }

    }else if(detect_vector->size() == 2 || detect_vector->size() == 1)
    {
        std::cout << "you need select at least 3 points." << std::endl;
        return;
    }else
    {
        if(roi_reverse == false)
        {
            roi_mask = cv::Mat(imgCenter->imgSrc.size(), CV_8UC1, cv::Scalar(255));
        }else
        {
            roi_mask = cv::Mat(imgCenter->imgSrc.size(), CV_8UC1, cv::Scalar(0));
        }
    }
    detect_vector = nullptr;
    delete detect_vector;
}

void AnalysisCenter::reset_detect()
{
    dataBase->del_all_detect_vector();
    roi_mask = cv::Mat();
    dataBase->del_threshold();

    draw_img(*dataBase->get_detect_vector());
}

void AnalysisCenter::detect_particle()
{
    bool roi_reverse = ui->checkBox_roi_reverse->isChecked();
    if(roi_mask.empty())
    {
        if(roi_reverse == false)
        {
            roi_mask = cv::Mat(imgCenter->imgSrc.size(), CV_8UC1, cv::Scalar(255));
        }else
        {
            roi_mask = cv::Mat(imgCenter->imgSrc.size(), CV_8UC1, cv::Scalar(0));
        }
    }
    dataBase->del_threshold();
//    bool hsv_h = ui->checkBox_hsv_h->isChecked();
//    bool hsv_s = ui->checkBox_hsv_s->isChecked();
//    bool hsv_v = ui->checkBox_hsv_v->isChecked();

    bool rgb_r = ui->checkBox_red->isChecked();
    bool rgb_g = ui->checkBox_green->isChecked();
    bool rgb_b = ui->checkBox_blue->isChecked();

    int red_text_value = ui->lineEdit_red_value->text().toInt();
    int green_text_value = ui->lineEdit_green_value->text().toInt();
    int blue_text_value = ui->lineEdit_blue_value->text().toInt();

    int red_threshold;
    int green_threshold;
    int blue_threshold;

    std::vector<cv::Mat> rgb_channels(3);
    cv::split(imgCenter->imgSrc, rgb_channels);

    if(red_text_value == -1 || green_text_value == -1 || blue_text_value == -1)
    {
        cv::Scalar rgb_mean;


        rgb_mean = cv::mean(imgCenter->imgSrc, roi_mask);
        rgb_mean = (rgb_mean * 44/100);


        QString text;
        cv::Mat src_value;
        cv::Scalar mean;

        if(red_text_value == -1)
        {
            red_threshold = (int)(rgb_mean[0]);
            text = QString("%1 ").arg(red_threshold);
            ui->lineEdit_red_value->setText(text);
        }else
        {
            red_threshold = red_text_value;
        }


        if(green_text_value == -1)
        {
            green_threshold = (int)(rgb_mean[1]);
            text = QString("%1 ").arg(green_threshold);
            ui->lineEdit_green_value->setText(text);
        }else
        {
            green_threshold = green_text_value;
        }


        if(blue_text_value == -1)
        {
            blue_threshold = (int)(rgb_mean[2]);
            text = QString("%1 ").arg(blue_threshold);
            ui->lineEdit_blue_value->setText(text);
        }else
        {
            blue_threshold = blue_text_value;
        }
    }else
    {
        red_threshold = red_text_value;
        green_threshold = green_text_value;
        blue_threshold = blue_text_value;
    }

    std::vector<cv::Mat> threshold_vector;
    /*
     * THRESH_BINARY
     * src_value > threshold ---> 255
     * src_value < threshold ---> 0
     *
     * THRESH_BINARY_INV
     * src_value > threshold ---> 0
     * src_value < threshold ---> 255
     */

    if(rgb_r)
    {
        cv::Mat red;
        cv::threshold(rgb_channels[0], red, (double) red_threshold, 255, 1);     // 0:THRESH_BINARY, 1:THRESH_BINARY_INV
        threshold_vector.push_back(red);
        red.release();
    }

    if(rgb_g)
    {
        cv::Mat green;
        cv::threshold(rgb_channels[1], green, (double) green_threshold, 255, 1);  // 0:THRESH_BINARY, 1:THRESH_BINARY_INV
        threshold_vector.push_back(green);
        green.release();
    }

    if(rgb_b)
    {
        cv::Mat blue;
        cv::threshold(rgb_channels[2], blue, (double) blue_threshold, 255, 1);    // 0:THRESH_BINARY, 1:THRESH_BINARY_INV
        threshold_vector.push_back(blue);
        blue.release();
    }

    rgb_channels.clear();
    std::vector<cv::Mat>().swap(rgb_channels);
    /*
     * if threshold_length <= 2 -> both of 2 selection need true;
     * else avg need avg > 255 * 0.7
     */
    double thr_len = threshold_vector.size() ;
    int thresh_value;
    if(thr_len < 3)
    {
        thresh_value = 254;
    }else{
         thresh_value = (int)(255.0 * (thr_len - thr_len/3)/thr_len) - 1;
    }

    threshold_vector[0].convertTo(threshold_vector[0], CV_32S);
    cv::Mat thresh_sum = threshold_vector[0];
    for(int i = 1; i < thr_len; i++)
    {
        threshold_vector[i].convertTo(threshold_vector[i], CV_32S);
        thresh_sum = thresh_sum + threshold_vector[i];
    }
    thresh_sum = thresh_sum/thr_len;
    thresh_sum.convertTo(thresh_sum, CV_8UC1);
    cv::Mat detect_threshold;
    cv::threshold(thresh_sum, detect_threshold, (double)thresh_value, 253, 0);
    detect_threshold = detect_threshold + 2;
    thresh_sum.release();
    detect_threshold.convertTo(detect_threshold, CV_8UC1);


    cv::Mat mask_thrshold;
    /*
     * if src1 == src2 ---> 255
     * else            ---> 0
     */
    cv::compare(detect_threshold, roi_mask, mask_thrshold, 0);

    detect_threshold.release();
    dataBase->set_threshold(mask_thrshold);

    mask_thrshold.release();
//    const cv::Mat *test;
//    test = dataBase->get_threshold();
//    std::cout << "test: "<< test << std::endl;

}































