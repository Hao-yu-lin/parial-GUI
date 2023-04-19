#include "../include/mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->scrollArea->setWidgetResizable(false);
    ui->label_image->installEventFilter(this);
    imgCenter = new ImageCenter(ui);
    analysisCenter = new AnalysisCenter(ui, imgCenter);
    ui->label_image->setText("No Image Loaded");
}

MainWindow::~MainWindow()
{
    delete imgCenter;
    delete ui;
}

void MainWindow::on_btn_open_img_clicked()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open Image"));
    if(flag_open_img)
    {
        analysisCenter->~AnalysisCenter();
    }
    imgCenter->open_img(fileName, flag_open_img);
    analysisCenter = new AnalysisCenter(ui, imgCenter);
}

void MainWindow::on_slider_zoom_sliderReleased()
{
    if(flag_open_img)
    {
        int value = ui->slider_zoom->value();
        imgCenter->slider_zoom(value);
    }else
    {
        std::cout <<"please open new image!" << std::endl;
    }
}

void MainWindow::on_btn_zoom_in_clicked()
{
     if(flag_open_img)
     {
         imgCenter->zoom_in();
     }else
     {
         std::cout <<"please open new image!" << std::endl;
     }
}

void MainWindow::on_btn_zoom_out_clicked()
{
    if(flag_open_img)
    {
        imgCenter->zoom_out();
    }else
    {
        std::cout <<"please open new image!" << std::endl;
    }
}

void MainWindow::on_btn_reset_view_clicked()
{
    if(flag_open_img)
    {
        imgCenter->rest_view();
        imgCenter->flag_num = flag_off;
    }else
    {
        std::cout <<"please open new image!" << std::endl;
    }
}

void MainWindow::on_btn_shadow_removal_clicked()
{
    if(flag_open_img)
    {
        imgCenter->shadow_removal();
    }else
    {
        std::cout <<"please open new image!" << std::endl;
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->label_image)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if(imgCenter->flag_num == flag_refer_obj)
            {
                if(mouseEvent->button() == Qt::LeftButton)
                {
                    QPointF img_pos = mouseEvent->pos();
                    analysisCenter->set_pts_vector(img_pos);
                    return true;
                }else if(mouseEvent->button() == Qt::RightButton)
                {
                    analysisCenter->del_pts_vector();
                    return true;
                }else
                {
                    return false;
                }
            }else if(imgCenter->flag_num == flag_select_roi){
                if(mouseEvent->button() == Qt::LeftButton)
                {
                    QPointF img_pos = mouseEvent->pos();
                    analysisCenter->set_pts_vector(img_pos);
                    return true;
                }else if(mouseEvent->button() == Qt::RightButton)
                {
                    analysisCenter->del_pts_vector();
                    return true;
                }else
                {
                    return false;
                }

            }else
            {
                return false;
            }
        }else
        {
            return false;
        }
    }else
    {
        return QWidget::eventFilter(obj, event);
    }
}

void MainWindow::on_btn_refer_obj_clicked()
{
    if(flag_open_img)
    {
        if(imgCenter->flag_num != flag_refer_obj){
            imgCenter->flag_num = flag_refer_obj;
        }else
        {
            imgCenter->flag_num = flag_off;
        }
    }else
    {
        std::cout <<"please open new image!" << std::endl;
    }
}

void MainWindow::on_btn_refer_obj_calculate_clicked()
{
    imgCenter->flag_num = flag_off;
    analysisCenter->cal_refer_obj();
}

void MainWindow::on_btn_refer_obj_rest_clicked()
{
    imgCenter->flag_num = flag_off;
    analysisCenter->reset_refer();
    ui->lineEdit_pixel_scale_value->clear();
    ui->lineEdit_object_length->clear();
}

void MainWindow::on_btn_roi_select_clicked()
{
    if(flag_open_img)
    {
        if(imgCenter->flag_num != flag_select_roi)
        {
            imgCenter->flag_num = flag_select_roi;
        }else
        {
            imgCenter->flag_num = flag_off;
        }
    }else
    {
        std::cout <<"please open new image!" << std::endl;
    }
}

void MainWindow::on_btn_roi_choose_clicked()
{
    imgCenter->flag_num = flag_off;
    analysisCenter->chose_detect_obj();
}

void MainWindow::on_btn_particle_reset_clicked()
{
    imgCenter->flag_num = flag_off;
    analysisCenter->reset_detect();
}

void MainWindow::on_btn_detect_particle_clicked()
{
    imgCenter->flag_num = flag_off;
    analysisCenter->detect_particle();
}

void MainWindow::on_btn_particle_analysis_clicked()
{

    if(imgCenter->flag_num == flag_hist){
        imgCenter->flag_num = flag_off;
        imgCenter->set_img();
        ui->btn_particle_analysis->setText("Analysis");

    }else{
        imgCenter->flag_num = flag_hist;
        analysisCenter->cal_contours();
        ui->btn_particle_analysis->setText("image");

    }

}


