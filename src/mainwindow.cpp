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
        flag_num = flag_off;
    }else
    {
        std::cout <<"please open new image!" << std::endl;
    }
}

void MainWindow::on_btn_shadow_removal_clicked()
{
    analysisCenter->test();
//    std::string addr = fileName.toStdString();
//    m_callpy = new CallPy;
//    m_child_thread = new QThread;
//    m_callpy->set_addr(addr);

//    m_callpy->moveToThread(m_child_thread);

//    std::cout << "start!!" << std::endl;
//    m_child_thread->start();
//    imgSrc = m_callpy->start_python();


//    QImage tmp_img(imgSrc.data,
//                      imgSrc.cols, // width
//                      imgSrc.rows, // height
//                      imgSrc.step,
//                      QImage::Format_RGB888);
//    orig_qimg = tmp_img;
//    set_img();

//    std::cout << "finish!!" << std::endl;

//    m_callpy->~CallPy();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->label_image)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if(flag_num == flag_refer_obj)
            {
                if(mouseEvent->button() == Qt::LeftButton)
                {
                    QPointF img_pos = mouseEvent->pos();
                    analysisCenter->set_pts_vector(img_pos, flag_num);
                    return true;
                }else if(mouseEvent->button() == Qt::RightButton)
                {
                    analysisCenter->del_pts_vector(flag_num);
                    return true;
                }else
                {
                    return false;
                }
            }else if(flag_num == flag_select_roi){
                if(mouseEvent->button() == Qt::LeftButton)
                {
                    QPointF img_pos = mouseEvent->pos();
                    analysisCenter->set_pts_vector(img_pos, flag_num);
                    return true;
                }else if(mouseEvent->button() == Qt::RightButton)
                {
                    analysisCenter->del_pts_vector(flag_num);
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
        if(flag_num != flag_refer_obj){
            flag_num = flag_refer_obj;
        }else
        {
            flag_num = flag_off;
        }
    }else
    {
        std::cout <<"please open new image!" << std::endl;
    }
}

void MainWindow::on_btn_refer_obj_calculate_clicked()
{
    flag_num = flag_off;
    analysisCenter->cal_refer_obj();
}

void MainWindow::on_btn_refer_obj_rest_clicked()
{
    flag_num = flag_off;
    analysisCenter->reset_refer();
    ui->lineEdit_pixel_scale_value->clear();
    ui->lineEdit_object_length->clear();
}

void MainWindow::on_btn_roi_select_clicked()
{
    if(flag_open_img)
    {
        if(flag_num != flag_select_roi)
        {
            flag_num = flag_select_roi;
        }else
        {
            flag_num = flag_off;
        }
    }else
    {
        std::cout <<"please open new image!" << std::endl;
    }
}

void MainWindow::on_btn_roi_choose_clicked()
{
    flag_num = flag_off;
    analysisCenter->chose_detect_obj();
}

void MainWindow::on_btn_particle_reset_clicked()
{
    flag_num = flag_off;
    analysisCenter->reset_detect();
}

void MainWindow::on_btn_detect_particle_clicked()
{
    flag_num = flag_off;
    analysisCenter->detect_particle();
}




void MainWindow::on_btn_parameter_reset_clicked()
{
    ui->lineEdit_red_value->setText("-1");
    ui->lineEdit_blue_value->setText("-1");
    ui->lineEdit_green_value->setText("-1");
}

