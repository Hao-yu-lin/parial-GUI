#include "../include/mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->scrollArea->setWidgetResizable(false);
    imgCenter = new ImageCenter(ui);
}

MainWindow::~MainWindow()
{
    delete imgCenter;
    delete ui;
}

void MainWindow::on_btn_open_img_clicked()
{
    std::cout << "flag open image:" << flag_open_img << std::endl;
    fileName = QFileDialog::getOpenFileName(this, tr("Open Image"));
    imgCenter->open_img(fileName, flag_open_img);
    std::cout << "flag open image:" << flag_open_img << std::endl;
}

//void MainWindow::on_slider_zoom_valueChanged(int value)
//{
//    if(flag_open_img){
//        imgCenter->slider_zoom(value);
//        std::cout << "1" << std::endl;
//    }else{
//        std::cout <<"1please open new image!" << std::endl;
//    }
//}

void MainWindow::on_slider_zoom_sliderReleased()
{
    if(flag_open_img){
        int value = ui->slider_zoom->value();
        imgCenter->slider_zoom(value);
    }else{
        std::cout <<"please open new image!" << std::endl;
    }
}


void MainWindow::on_btn_zoom_in_clicked()
{
     if(flag_open_img){
         imgCenter->zoom_in();
     }else{
         std::cout <<"please open new image!" << std::endl;
     }
}

void MainWindow::on_btn_zoom_out_clicked()
{
    if(flag_open_img){
        imgCenter->zoom_out();
    }else{
        std::cout <<"please open new image!" << std::endl;
    }
}

void MainWindow::on_btn_reset_view_clicked()
{
    if(flag_open_img){
        imgCenter->rest_view();
    }else{
        std::cout <<"please open new image!" << std::endl;
    }
}




void MainWindow::on_btn_shadow_removal_clicked()
{
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



