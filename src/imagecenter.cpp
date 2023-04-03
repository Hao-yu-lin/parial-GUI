#include "../include/imagecenter.h"
#include "./ui_mainwindow.h"


ImageCenter::ImageCenter(Ui::MainWindow *input_ui)
    :ui(input_ui)
    ,imgdata(nullptr)
{
    label_image_width = ui->label_image->width();
    label_image_height = ui->label_image->height();
}

void ImageCenter::open_img(const QString &fileName, bool &flag_open_image)
{

        std::string path = fileName.toStdString();
        imgSrc = cv::imread(path);

        if(!imgSrc.empty()){
            try {
                if(flag_open_image == true){
                    ui->label_image->resize(label_image_width, label_image_height);
                    imgdata->~DataBase();
                    imgdata = nullptr;
                }
                cv::cvtColor(imgSrc, imgSrc, cv::COLOR_BGR2RGB);
                // record data info

                imgdata = new DataBase();
                imgdata->set_shape(imgSrc.cols, imgSrc.rows, imgSrc.channels());

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
                     imgdata->set_origin_ratio_rate( ui->label_image->height()/height);
                }

                orig_qimg = tmp_img.copy();
                ui->label_image->setAlignment(Qt::AlignCenter);
                set_img();
                cv::cvtColor(imgSrc, imgSrc, cv::COLOR_RGB2BGR);
                flag_open_image = true;

            }  catch (std::exception &e) {
                std::cout << "exception: " << e.what() << "\n";

            }

        }else{
            std::cout << "img read error!" << std::endl;

        }


}

void ImageCenter::slider_zoom(const int value)
{
    double rate = value_to_rate(value);
    imgdata->set_ratio(rate);
    set_img();
}

void ImageCenter::zoom_in()
{
    int value = std::min(101, imgdata->get_ratio_value()+1);
    double ratio = value_to_rate(value);
    imgdata->set_ratio(ratio, value);
    set_img();
}

void ImageCenter::zoom_out()
{
    int value = std::max(0, imgdata->get_ratio_value()-1);
    double ratio = value_to_rate(value);
    imgdata->set_ratio(ratio, value);
    set_img();
}

void ImageCenter::rest_view()
{
    imgdata->rest_ratio();
    set_img();
}

void ImageCenter::set_img()
{
    double ratio = imgdata->get_ratio_rate();
    int qimg_height = ratio * imgdata->get_orig_height();
    int qimg_width = ratio * imgdata->get_orig_width();
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
    std::cout << imgdata->get_ratio_value() << std::endl;
}

QPointF ImageCenter::zoomevent(const double &new_rate)
{
    double old_rate = imgdata->get_old_ratio_rate();
    QPointF scrollbarpos = QPointF(ui->scrollArea->horizontalScrollBar()->value(), ui->scrollArea->verticalScrollBar()->value());
    QPointF new_barpos = (scrollbarpos / old_rate) * new_rate;
    return new_barpos;
}


