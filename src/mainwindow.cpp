#include "../include/mainwindow.h"
#include "./ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , imgdata(nullptr)
{
    ui->setupUi(this);
    ui->scrollArea->setWidgetResizable(false);

}

MainWindow::~MainWindow()
{
    imgdata->~Database();
    delete imgdata;
    delete ui;
}

void MainWindow::on_btn_open_img_clicked()
{
//    ui->btn_open_img->setText("Open Image");
    if(!(imgdata == nullptr)){
        imgdata->~Database();
    }

    fileName = QFileDialog::getOpenFileName(this, tr("Open Image"));

    std::string path = fileName.toStdString();

    imgSrc = cv::imread(path);

    if(!imgSrc.empty()){
        cv::cvtColor(imgSrc, imgSrc, cv::COLOR_BGR2RGB);
        // record data info
        imgdata = new Database();
        imgdata->set_imgshape(imgSrc.cols, imgSrc.rows, imgSrc.channels());

        QImage tmp_img(imgSrc.data,
                   imgSrc.cols, // width
                   imgSrc.rows, // height
                   imgSrc.step,
                   QImage::Format_RGB888);
       // img_scaled
        double width = imgSrc.cols;
        double height = imgSrc.rows;

        if((width / ui->label_image->width())
                >= (height / ui->label_image->height())){
            imgdata->set_origin_ratio_rate(ui->label_image->width()/width);
        }else{
             imgdata->set_origin_ratio_rate(ui->label_image->height()/height);
        }
        qimg_img = tmp_img;
        orig_qimg = qimg_img.copy();
        ui->label_image->setAlignment(Qt::AlignCenter);
        set_img();
        cv::cvtColor(imgSrc, imgSrc, cv::COLOR_RGB2BGR);
    }
}

void MainWindow::on_slider_zoom_valueChanged(int value)
{
    double rate = value_to_rate(value);
    imgdata->set_ratio(rate);
    set_img();
}

void MainWindow::on_btn_zoom_in_clicked()
{
    int value = std::min(101, imgdata->get_ratio_value()+1);
    double ratio = value_to_rate(value);
    imgdata->set_ratio(ratio, value);
    set_img();
}

void MainWindow::on_btn_zoom_out_clicked()
{
    int value = std::max(0, imgdata->get_ratio_value()-1);
    double ratio = value_to_rate(value);
    imgdata->set_ratio(ratio, value);
    set_img();
}

void MainWindow::on_btn_reset_view_clicked()
{
    imgdata->rest_ratio();
    set_img();
}

void MainWindow::set_img(){
    double ratio = imgdata->get_ratio_rate();
    int qimg_height = ratio * imgdata->get_orig_h();
    int qimg_width = ratio * imgdata->get_orig_w();
    qimg_img = orig_qimg.scaledToHeight(qimg_height);
    ui->label_image->setPixmap(QPixmap::fromImage(qimg_img));
    ui->label_image->resize(qimg_width + 20, qimg_height + 20);

    // sroll area
    QPointF new_barpos = zoomevent(ratio);
    ui->scrollArea->takeWidget();
    ui->scrollArea->setWidget(ui->label_image);
    ui->scrollArea->horizontalScrollBar()->setValue(new_barpos.x());
    ui->scrollArea->verticalScrollBar()->setValue(new_barpos.y());

    // set slider
    int value = rate_to_value(ratio);
    ui->slider_zoom->setValue(value);

    QString text = QString("%1 %").arg(std::ceil(ratio * 100));
    ui->label_ratio->setText(text);

}

QPointF MainWindow::zoomevent(double new_rate){
    double old_rate = imgdata->get_old_ratio_rate();
    QPointF scrollbarpos = QPointF(ui->scrollArea->horizontalScrollBar()->value(), ui->scrollArea->verticalScrollBar()->value());
    QPointF new_barpos = (scrollbarpos / old_rate) * new_rate;
    return new_barpos;
}




void MainWindow::on_btn_shadow_removal_clicked()
{
    std::string addr = fileName.toStdString();
    call_python(addr);
}

