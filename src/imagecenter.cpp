#include "../include/imagecenter.h"
#include "./ui_mainwindow.h"





ImageCenter::ImageCenter(Ui::MainWindow *input_ui, setting_t *input_flag)
    :ui(input_ui)
    ,dataBase(nullptr)
    ,set_flag(input_flag)
{
    label_image_width = ui->label_image->width();
    label_image_height = ui->label_image->height();
    dataBase = new DataBase(input_flag);
}



void ImageCenter::open_img(const QString &fileName)
{
        img_path = fileName.toStdString();

        imgSrc = cv::imread(img_path);

        if(!imgSrc.empty())
        {
            try {
                if(set_flag->flag_image == true)
                {
                    ui->label_image->resize(label_image_width, label_image_height);
                    dataBase->~DataBase();
                    dataBase = nullptr;
                }
                cv::cvtColor(imgSrc, imgSrc, cv::COLOR_BGR2RGB);
                // record data info

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
                set_flag->flag_image = true;

            }  catch (std::exception &e)
            {
                std::cout << "exception: " << e.what() << "\n";
                set_flag->flag_image = false;
                return;
            }

        }else
        {
            std::cout << "img read error!" << std::endl;
            set_flag->flag_image = false;
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
    if(set_flag->flag_num == num_hist){
        set_his_img();
    }else{
         set_img();
    }
    set_sroll_area();
}

void ImageCenter::zoom_in()
{
    int value = std::min(80, dataBase->get_ratio_value()+1);
    double ratio = cal::value_to_rate(value);
    dataBase->set_ratio(ratio, value);
    if(set_flag->flag_num == num_hist){
        set_his_img();
    }else{
         set_img();
    }

    set_sroll_area();
}

void ImageCenter::zoom_out()
{
    int value = std::max(0, dataBase->get_ratio_value()-1);
    double ratio = cal::value_to_rate(value);
    dataBase->set_ratio(ratio, value);
    if(set_flag->flag_num == num_hist){
        set_his_img();
    }else{
         set_img();
    }
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

//        qimg_img = hist_qimg.scaledToHeight(qimg_height);

    qimg_height = ratio * dataBase->get_orig_height();
    qimg_width = ratio * dataBase->get_orig_width();
    const QImage &orig_qimg = dataBase->get_orig_img();

    qimg_img = orig_qimg.scaledToHeight(qimg_height);
    ui->label_image->setPixmap(QPixmap::fromImage(qimg_img));
    ui->label_image->resize(qimg_width + 20, qimg_height + 20);
    ui->label_image->setAlignment(Qt::AlignCenter);

    // set slider
    int value = cal::rate_to_value(ratio);
    ui->slider_zoom->setValue(value);

    QString text = QString("%1 %").arg(std::ceil(ratio * 100));
    ui->label_ratio->setText(text);
}

void ImageCenter::set_his_img()
{
//    const double &ratio = dataBase->get_ratio_rate();
    double ratio = 0;
    double qimg_height, qimg_width;

    qimg_height = dataBase->get_hist_height();
    qimg_width = dataBase->get_hist_width();

    if(dataBase->get_ratio_rate() == 0){
        if((qimg_width / ui->label_image->width())
                >= (qimg_height / ui->label_image->height()))
        {
            ratio = ui->label_image->width()/qimg_width;
        }else
        {
             ratio = ui->label_image->height()/qimg_height;
        }
    }else{
        ratio = dataBase->get_ratio_rate();
    }


    qimg_height = ratio * qimg_height;
    qimg_width = ratio * qimg_width;

    const QImage &hist_qimg = dataBase->get_hist_img();
    qimg_img = hist_qimg.scaledToHeight(qimg_height);

    ui->label_image->setPixmap(QPixmap::fromImage(qimg_img));
    ui->label_image->resize(qimg_width + 20, qimg_height + 20);
    ui->label_image->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    // set slider
    int value = cal::rate_to_value(ratio);
    dataBase->set_ratio(ratio, value);
    ui->slider_zoom->setValue(value);
    QString text = QString("%1 %").arg(std::ceil(ratio * 100));
    ui->label_ratio->setText(text);
    set_sroll_area();

}

void ImageCenter::set_his2_img()
{
    double ratio = 0;
    double qimg_height, qimg_width;

    qimg_height = dataBase->get_hist2_height();
    qimg_width = dataBase->get_hist2_width();

    if(dataBase->get_ratio_rate() == 0){
        if((qimg_width / ui->label_image->width())
                >= (qimg_height / ui->label_image->height()))
        {
            ratio = ui->label_image->width()/qimg_width;
        }else
        {
             ratio = ui->label_image->height()/qimg_height;
        }
    }else{
        ratio = dataBase->get_ratio_rate();
    }

    qimg_height = ratio * qimg_height;
    qimg_width = ratio * qimg_width;

    const QImage &hist_qimg = dataBase->get_hist2_img();
    qimg_img = hist_qimg.scaledToHeight(qimg_height);

    ui->label_image->setPixmap(QPixmap::fromImage(qimg_img));
    ui->label_image->resize(qimg_width + 20, qimg_height + 20);
    ui->label_image->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    // set slider
    int value = cal::rate_to_value(ratio);
    dataBase->set_ratio(ratio, value);
    ui->slider_zoom->setValue(value);
    QString text = QString("%1 %").arg(std::ceil(ratio * 100));
    ui->label_ratio->setText(text);
    set_sroll_area();
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

void ImageCenter::white_balance()
{
    cv::Scalar means = cv::mean(imgSrc);

    std::vector<cv::Mat> rgb_channels(3);
    cv::split(imgSrc, rgb_channels);

    cv::Mat r_channel = rgb_channels[0];
    cv::Mat g_channel = rgb_channels[1];
    cv::Mat b_channel = rgb_channels[2];

    float r_mean = means[0];
    float g_mean = means[1];
    float b_mean = means[2];

    for (int i = 0; i < 3; i++) {
        cv::Mat mask;
        cv::compare(rgb_channels[i], means[i], mask, cv::CMP_GT);
        cv::Mat sum_masked;
        cv::bitwise_and(rgb_channels[i], mask, sum_masked);
        double sum = cv::sum(sum_masked)[0];
        int count = cv::countNonZero(mask);

        if (count > 0) {
            if (i == 0) {
                r_mean = sum / count;
            } else if (i == 1) {
                g_mean = sum / count;
            } else if (i == 2) {
                b_mean = sum / count;
            }
        }
    }

    float avg = pow(r_mean * g_mean * b_mean, 1.0/3);
    float rCoef = avg/r_mean;
    float gCoef = avg/g_mean;
    float bCoef = avg/b_mean;

    multiply(b_channel, bCoef, b_channel);
    multiply(g_channel, gCoef, g_channel);
    multiply(r_channel, rCoef, r_channel);

    b_channel = min(b_channel, cv::Scalar(255));
    g_channel = min(g_channel, cv::Scalar(255));
    r_channel = min(r_channel, cv::Scalar(255));

    merge(rgb_channels, imgSrc);

    cv::Scalar new_means = cv::mean(imgSrc);

    r_mean = new_means[0];
    g_mean = new_means[1];
    b_mean = new_means[2];

    float r_mean_new = 0, g_mean_new = 0, b_mean_new = 0;

    for (int i = 0; i < 3; i++) {
        cv::Mat mask;
        cv::compare(rgb_channels[i], means[i], mask, cv::CMP_GT);
        cv::Mat sum_masked;
        cv::bitwise_and(rgb_channels[i], mask, sum_masked);
        double sum = cv::sum(sum_masked)[0];
        int count = cv::countNonZero(mask);

        if (count > 0) {
            if (i == 0) {
                r_mean_new = sum / count;
            } else if (i == 1) {
                g_mean_new = sum / count;
            } else if (i == 2) {
                b_mean_new = sum / count;
            }
        }
    }

    float new_coeff1 =  255.0 / std::min(r_mean_new, std::min(g_mean_new, b_mean_new));
    float new_coeff2 =  255.0 / std::min(r_mean, std::min(g_mean, b_mean));
    float new_coeff = std::sqrt(new_coeff1 * new_coeff2);
    cv::Mat table(1, 256, CV_8U);
    for (int i = 0; i < 256; i++) {
        table.at<uchar>(i) = cv::saturate_cast<uchar>(i * new_coeff);
    }
    LUT(imgSrc, table, imgSrc);
    rgb_channels.clear();
    std::vector<cv::Mat>().swap(rgb_channels);
    QImage tmp_img(imgSrc.data,
               imgSrc.cols, // width
               imgSrc.rows, // height
               imgSrc.step,
               QImage::Format_RGB888);
    dataBase->set_origimg(tmp_img);
    set_img();

}

void ImageCenter::save_data(const QString &fileName)
{
//    const std::vector<float>* contours_area = dataBase->get_contours_area();
    const std::vector<particle_t>* particle_data = dataBase->get_particle_data();
    QFile datafile(fileName);
    if(!datafile.open(QFile::WriteOnly | QFile::Text))
    {
        std::cout << "error" << std::endl;
        return;
    }
    QTextStream out(&datafile);
    out << "idx" << "," << "area" << "," << "diameter" << "\n";
    for(int i=0; i < particle_data->size(); i++){
        out << particle_data->at(i).idx <<"," << particle_data->at(i).surface << "," <<  particle_data->at(i).diameter<< endl;
    }
    datafile.close();
}












