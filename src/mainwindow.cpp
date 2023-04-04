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
}

MainWindow::~MainWindow()
{
    delete imgCenter;
    delete ui;
}

void MainWindow::on_btn_open_img_clicked()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open Image"));
    if(flag_open_img){
        analysisCenter->~AnalysisCenter();
    }
    imgCenter->open_img(fileName, flag_open_img);
    analysisCenter = new AnalysisCenter(ui, imgCenter);
}

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

void MainWindow::on_btn_refer_obj_clicked()
{
    if(flag_open_img){
        flag_refer_obj = !flag_refer_obj;
    }else{
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
            if(mouseEvent->button() == Qt::LeftButton && flag_refer_obj)
            {
                QPointF img_pos = mouseEvent->pos();
                analysisCenter->set_refer_object(img_pos);
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
        return QWidget::eventFilter(obj, event);
    }

}


