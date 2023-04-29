#include "../include/imagecenter.h"
#include "./ui_mainwindow.h"





ImageCenter::ImageCenter(Ui::MainWindow *input_ui)
    :ui(input_ui)
    ,dataBase(nullptr)
{
    label_image_width = ui->label_image->width();
    label_image_height = ui->label_image->height();
}



void ImageCenter::open_img(const QString &fileName, bool &flag_open_image)
{
        img_path = fileName.toStdString();

        imgSrc = cv::imread(img_path);

        if(!imgSrc.empty())
        {
            try {
                if(flag_open_image == true)
                {
                    ui->label_image->resize(label_image_width, label_image_height);
                    dataBase->~DataBase();
                    dataBase = nullptr;
                }
                cv::cvtColor(imgSrc, imgSrc, cv::COLOR_BGR2RGB);
                // record data info

                dataBase = new DataBase();
                dataBase->set_shape(imgSrc.cols, imgSrc.rows, imgSrc.channels());

                QImage tmp_img(imgSrc.data,
                           imgSrc.cols, // width
                           imgSrc.rows, // height
                           imgSrc.step,
                           QImage::Format_RGB888);
               // img_scaled
                double width = imgSrc.cols;
                double height = imgSrc.rows;


                if((width / ui->label_image->width())
                        >= (height / ui->label_image->height()))
                {
                    dataBase->set_origin_ratio_rate(ui->label_image->width()/width);
                }else
                {
                     dataBase->set_origin_ratio_rate( ui->label_image->height()/height);
                }

                dataBase->set_origimg(tmp_img);

                set_img();
                set_sroll_area();
//                cv::cvtColor(imgSrc, imgSrc, cv::COLOR_RGB2BGR);
                flag_open_image = true;

            }  catch (std::exception &e)
            {
                std::cout << "exception: " << e.what() << "\n";
                flag_open_image = false;
                return;
            }

        }else
        {
            std::cout << "img read error!" << std::endl;
            flag_open_image = false;
            return;
        }
}

/*
 *       10%   ratio value = 0
 *       100%  ratio value = 50
 *      1000%  ratio value = 100
 *      we get the formula that ratio rate = 10^((ratio_value-50)/50)
*/

void ImageCenter::slider_zoom(const int value)
{
    double rate = cal::value_to_rate(value);
    dataBase->set_ratio(rate);
    set_img();
    set_sroll_area();
}

void ImageCenter::zoom_in()
{
    int value = std::min(80, dataBase->get_ratio_value()+1);
    double ratio = cal::value_to_rate(value);
    dataBase->set_ratio(ratio, value);
    set_img();
    set_sroll_area();
}

void ImageCenter::zoom_out()
{
    int value = std::max(0, dataBase->get_ratio_value()-1);
    double ratio = cal::value_to_rate(value);
    dataBase->set_ratio(ratio, value);
    set_img();
    set_sroll_area();
}

void ImageCenter::rest_view()
{
    QImage tmp_img(imgSrc.data,
               imgSrc.cols, // width
               imgSrc.rows, // height
               imgSrc.step,
               QImage::Format_RGB888);
    dataBase->set_origimg(tmp_img);
    dataBase->rest_ratio();
    set_img();
    set_sroll_area();
}

void ImageCenter::set_img()
{
    const double &ratio = dataBase->get_ratio_rate();
    double qimg_height, qimg_width;

    if(this->flag_num == flag_hist){
        qimg_height = ratio * dataBase->get_hist_height();
        qimg_width = ratio * dataBase->get_hist_width();
        const QImage &hist_qimg = dataBase->get_hist_img();
        qimg_img = hist_qimg.scaledToHeight(qimg_height);

        ui->label_image->setPixmap(QPixmap::fromImage(qimg_img));
        ui->label_image->resize(qimg_width + 20, qimg_height + 20);
        ui->label_image->setAlignment(Qt::AlignLeft | Qt::AlignTop);

//        qimg_img = hist_qimg.scaledToHeight(qimg_height);
    }else{
        qimg_height = ratio * dataBase->get_orig_height();
        qimg_width = ratio * dataBase->get_orig_width();
        const QImage &orig_qimg = dataBase->get_orig_img();

        qimg_img = orig_qimg.scaledToHeight(qimg_height);
        ui->label_image->setPixmap(QPixmap::fromImage(qimg_img));
        ui->label_image->resize(qimg_width + 20, qimg_height + 20);
        ui->label_image->setAlignment(Qt::AlignCenter);
    }


    // set slider
    int value = cal::rate_to_value(ratio);
    ui->slider_zoom->setValue(value);

    QString text = QString("%1 %").arg(std::ceil(ratio * 100));
    ui->label_ratio->setText(text);
}

void ImageCenter::set_sroll_area()
{
    const double &ratio = dataBase->get_ratio_rate();
    // sroll area
    QPointF new_barpos = zoomevent(ratio);
    ui->scrollArea->takeWidget();
    ui->scrollArea->setWidget(ui->label_image);
    ui->scrollArea->horizontalScrollBar()->setValue(new_barpos.x() - ui->scrollArea->horizontalScrollBar()->pageStep()/4);
    ui->scrollArea->verticalScrollBar()->setValue(new_barpos.y() - ui->scrollArea->verticalScrollBar()->pageStep()/2);
}

QPointF ImageCenter::zoomevent(const double &new_rate)
{
    const double &old_rate = dataBase->get_old_ratio_rate();
    QPointF scrollbarpos = QPointF(ui->scrollArea->horizontalScrollBar()->value() + ui->scrollArea->horizontalScrollBar()->pageStep()/4, ui->scrollArea->verticalScrollBar()->value() + ui->scrollArea->verticalScrollBar()->pageStep()/2);

    QPointF new_barpos = (scrollbarpos / old_rate) * new_rate;

    return new_barpos;
}



void ImageCenter::shadow_removal()
{


//    m_callpy = new CallPy;
//    m_child_thread = new QThread;
//    m_callpy->set_addr(img_path);

//    m_callpy->moveToThread(m_child_thread);

//    std::cout << "start!!" << std::endl;
//    m_child_thread->start();

//    imgSrc = m_callpy->start_python();


//    QImage tmp_img(imgSrc.data,
//                      imgSrc.cols, // width
//                      imgSrc.rows, // height
//                      imgSrc.step,
//                      QImage::Format_RGB888);
//    dataBase->set_origimg(tmp_img);
//    set_img();

//    std::cout << "finish!!" << std::endl;

//    m_callpy->~CallPy();
//    m_callpy = nullptr;
}



