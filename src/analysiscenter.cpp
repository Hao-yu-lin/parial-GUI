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
    const std::vector<cal::DataPoint>& test = dataBase->get_refer_vector();
}

void AnalysisCenter::draw_img(const std::vector<cal::DataPoint> &vector_pts, cv::Scalar color)
{
    cv::Mat tmp_img = imgCenter->imgSrc.clone();
    update_pts_img(tmp_img, vector_pts, color);
}

void AnalysisCenter::update_pts_img(cv::Mat &imgsrc, const std::vector<cal::DataPoint> &vector_pts, const cv::Scalar color)
{
    if(vector_pts.size() > 0){
        cv::Point pts(vector_pts[0].x, vector_pts[0].y);
        cv::circle(imgsrc, pts, 10, color, 4);
        for(int i = 1; i < vector_pts.size(); i++)
        {
            cv::Point pts_1(vector_pts[i-1].x, vector_pts[i-1].y);
            cv::Point pts_2(vector_pts[i].x, vector_pts[i].y);
            cv::circle(imgsrc, pts_2, 10, color, 4);
            cv::line(imgsrc, pts_1, pts_2, color, 3);
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

    cal::DataPoint pos;
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
    switch (flag_num) {
    case 1:
        dataBase->set_refer_point(pos);
        draw_img(dataBase->get_refer_vector(), cv::Scalar( 0, 255, 0));
        break;
    case 2:
        dataBase->set_detect_point(pos);
        draw_img(dataBase->get_detect_vector(), cv::Scalar( 0, 0, 255));
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
    switch (flag_num) {
    case 1:
        dataBase->del_refer_vector();
        draw_img(dataBase->get_refer_vector(), cv::Scalar( 0, 255, 0));
        break;
    case 2:
        dataBase->del_detect_vector();
        draw_img(dataBase->get_detect_vector(), cv::Scalar( 0, 0, 255));
        break;
    default:
        break;
    }
}

void AnalysisCenter::reset_refer()
{
    dataBase->del_refer_vector();
    dataBase->del_refer_vector();
    draw_img(dataBase->get_refer_vector(), cv::Scalar(0,255,0));
}

void AnalysisCenter::cal_refer_obj()
{
    draw_img(dataBase->get_refer_vector(), cv::Scalar(255,0,0));
    std::vector<cal::DataPoint> refer_pts = dataBase->get_refer_vector();
    if(refer_pts.size() < 1){
        std::cout << "Need two reference object point" << std::endl;
        return;
    }
    double obj_pixel_len = cal::dist(refer_pts[0], refer_pts[1]);
    if(!ui->lineEdit_object_length->text().isEmpty())
    {
        double phy_length = ui->lineEdit_object_length->text().toDouble();
        double pixel_scale = phy_length/obj_pixel_len;
        dataBase->set_pixel_sacle(pixel_scale);
        QString text = QStringLiteral("%1 ").arg(pixel_scale, 0, 'f', 3);
        ui->lineEdit_pixel_scale_value->setText(text);
        reset_refer();
    }
}

void AnalysisCenter::chose_detect_obj()
{
    cal::DataPoint pts = dataBase->get_detect_vector()[0];
    dataBase->set_detect_point(pts);
    draw_img(dataBase->get_detect_vector(), cv::Scalar( 255, 0, 0));
}

void AnalysisCenter::reset_detect()
{
    dataBase->del_all_detect_vector();
     draw_img(dataBase->get_detect_vector(), cv::Scalar(0,255,0));
}






