#include "../include/analysiscenter.h"
#include "./ui_mainwindow.h"

AnalysisCenter::AnalysisCenter(Ui::MainWindow *input_ui, ImageCenter *input_imgCenter)
    :ui(input_ui)
    , imgCenter(input_imgCenter)
    , dataBase(input_imgCenter->dataBase)
{

}

void AnalysisCenter::test(){
    const std::vector<DataPoint>& test = dataBase->get_refer_vector();
}

void AnalysisCenter::set_refer_object(const QPointF &img_pos)
{
    auto w_d = (ui->label_image->width() - imgCenter->qimg_img.width())/2.0;
    auto h_d = (ui->label_image->height() - imgCenter->qimg_img.height()) / 2.0;

    auto x = img_pos.x()-w_d;
    auto y = img_pos.y()-h_d;

    if(x < 0 || x > imgCenter->qimg_img.width() || y < 0 || y > imgCenter->qimg_img.height()) return;

    DataPoint pos;
    pos.x = x / dataBase->get_old_ratio_rate();
    pos.y = y / dataBase->get_old_ratio_rate();

    dataBase->set_refer_point(pos);
    const std::vector<DataPoint>& refer_pts = dataBase->get_refer_vector();
//    for(auto pts:refer_pts){
//        std::cout << "(" << pts.x << "," << pts.y << ")";
//    }
//    std::cout << std::endl;


}

