#include "../include/analysiscenter.h"
#include "./ui_mainwindow.h"

AnalysisCenter::AnalysisCenter(Ui::MainWindow *input_ui, ImageCenter *input_imgCenter)
    :ui(input_ui)
    , imgCenter(input_imgCenter)
    , dataBase(input_imgCenter->dataBase)
{

}

void AnalysisCenter::test(){
    const std::vector<cal::DataPoint>& test = dataBase->get_refer_vector();
}

void AnalysisCenter::set_pts_vector(const QPointF &img_pos, const int &flag_num)
{
    auto w_d = (ui->label_image->width() - imgCenter->qimg_img.width())/2.0;
    auto h_d = (ui->label_image->height() - imgCenter->qimg_img.height()) / 2.0;

    auto x = img_pos.x()-w_d;
    auto y = img_pos.y()-h_d;

    if(x < 0 || x > imgCenter->qimg_img.width() || y < 0 || y > imgCenter->qimg_img.height()) return;

    cal::DataPoint pos;
    pos.x = x / dataBase->get_ratio_rate();
    pos.y = y / dataBase->get_ratio_rate();

    if(flag_num == 1){
        dataBase->set_refer_point(pos);
        draw_img(dataBase->get_refer_vector(), cv::Scalar(0,255,0));
    }

}

void AnalysisCenter::del_pts_vector(const int &flag_num)
{
    if(flag_num == 1){
        dataBase->del_refer_vector();
        draw_img(dataBase->get_refer_vector(), cv::Scalar(0,255,0));
    }

}

void AnalysisCenter::reset_pts()
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
    std::cout << obj_pixel_len << std::endl;
}

void AnalysisCenter::draw_img(const std::vector<cal::DataPoint> &vector_pts, cv::Scalar color)
{
    cv::Mat tmp_img = imgCenter->imgSrc.clone();
    update_pts_img(tmp_img, vector_pts, color);
}

void AnalysisCenter::update_pts_img(cv::Mat &imgsrc, const std::vector<cal::DataPoint> &vector_pts, const cv::Scalar color)
{
    cv::Point pts(vector_pts[0].x, vector_pts[0].y);
    cv::circle(imgsrc, pts, 10, color, 4);
    for(int i = 1; i < vector_pts.size(); i++)
    {
        cv::Point pts_1(vector_pts[i-1].x, vector_pts[i-1].y);
        cv::Point pts_2(vector_pts[i].x, vector_pts[i].y);
        cv::circle(imgsrc, pts_2, 10, color, 4);
        cv::line(imgsrc, pts_1, pts_2, color, 3);
    }


    QImage tmp_img(imgsrc.data,
               imgsrc.cols, // width
               imgsrc.rows, // height
               imgsrc.step,
               QImage::Format_RGB888);

    dataBase->set_origimg(tmp_img);
    imgCenter->set_img();

}






