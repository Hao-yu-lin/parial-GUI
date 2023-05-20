#include "../include/analysiscenter.h"
#include "./ui_mainwindow.h"

AnalysisCenter::AnalysisCenter(Ui::MainWindow *input_ui, ImageCenter *input_imgCenter, setting_t *input_flag)
    :ui(input_ui)
    , imgCenter(input_imgCenter)
    , dataBase(input_imgCenter->dataBase)
    , set_flag(input_flag)
{}

void AnalysisCenter::draw_img(const std::vector<cv::Point2i> &vector_pts, cv::Scalar color)
{
    cv::Mat tmp_img = imgCenter->imgSrc.clone();
    update_pts_img(tmp_img, vector_pts, color);
}

void AnalysisCenter::draw_img()
{
    cv::Mat imgsrc = imgCenter->imgSrc.clone();
    QImage tmp_img(imgsrc.data,
               imgsrc.cols, // width
               imgsrc.rows, // height
               imgsrc.step,
               QImage::Format_RGB888);

    dataBase->set_origimg(tmp_img);
    imgCenter->set_img();
}

void AnalysisCenter::update_pts_img(cv::Mat &imgsrc, const std::vector<cv::Point2i> &vector_pts, const cv::Scalar color)
{
    if(vector_pts.size() > 0)
    {
        double ratio = std::ceil(dataBase->get_ratio_rate() * 100);

        int linethickness = 1;

        if(ratio < 20)
        {
            linethickness = 8;
        }
        else if(ratio >= 20 && ratio < 50)
        {
            linethickness = 6;
        }
        else if(ratio >= 50 && ratio < 80)
        {
            linethickness = 3;
        }else if(ratio >= 80 && ratio < 120)
        {
           linethickness = 2;
        }else
        {
            linethickness = 1;
        }

        cv::Point pts(vector_pts[0].x, vector_pts[0].y);
        cv::circle(imgsrc, pts, 10, color, linethickness);
        for(int i = 1; i < vector_pts.size(); i++)
        {
            cv::Point pts_1(vector_pts[i-1].x, vector_pts[i-1].y);
            cv::Point pts_2(vector_pts[i].x, vector_pts[i].y);
            cv::circle(imgsrc, pts_2, 10, color, linethickness);
            cv::line(imgsrc, pts_1, pts_2, color, linethickness);
        }
    }

    QImage tmp_img(imgsrc.data,
               imgsrc.cols, // width
               imgsrc.rows, // height
               imgsrc.step,
               QImage::Format_RGB888);

    dataBase->set_origimg(tmp_img);
    imgCenter->set_img();

}



/*
flag num:
Reference Object     --->   flag_num = 1
Selection Particle   --->   flag_num = 2
*/

void AnalysisCenter::set_pts_vector(const QPointF &img_pos)
{
    auto w_d = (ui->label_image->width() - imgCenter->qimg_img.width())/2.0;
    auto h_d = (ui->label_image->height() - imgCenter->qimg_img.height()) / 2.0;

    auto x = img_pos.x()-w_d;
    auto y = img_pos.y()-h_d;

    if( x < 0 || x > imgCenter->qimg_img.width() || y < 0 || y > imgCenter->qimg_img.height()) return;

    cv::Point2i pos;
    pos.x = x / dataBase->get_ratio_rate();
    pos.y = y / dataBase->get_ratio_rate();

    switch (set_flag->flag_num)
    {
    case 1:
        dataBase->set_refer_point(pos);
        draw_img(*dataBase->get_refer_vector(), cv::Scalar( 0, 255, 0));
        break;

    case 2:
        dataBase->set_detect_point(pos);
        draw_img(*dataBase->get_detect_vector(), cv::Scalar( 0, 0, 255));
        break;

    case 4:
        if(set_flag->flag_contours == false){
            return;
        }
        dataBase->del_detect_vector(pos);
        draw_contours_img();
        break;
    default:
        break;
    }
}

void AnalysisCenter::del_pts_vector()
{
    switch (set_flag->flag_num)
    {
    case 1:
        dataBase->del_refer_vector();
        draw_img(*dataBase->get_refer_vector(), cv::Scalar( 0, 255, 0));
        break;

    case 2:
        dataBase->del_detect_vector();
        draw_img(*dataBase->get_detect_vector(), cv::Scalar( 0, 0, 255));
        break; 

    default:
        break;
    }
}

void AnalysisCenter::reset_refer()
{
    dataBase->del_refer_vector();
    dataBase->del_refer_vector();
    draw_img();
}

void AnalysisCenter::cal_refer_obj()
{
    draw_img(*dataBase->get_refer_vector(), cv::Scalar(255,0,0));
    const std::vector<cv::Point2i>*refer_pts = dataBase->get_refer_vector();
    if(dataBase->get_refer_vector()->size() != 2)
    {
        std::cout << "Need two reference object point" << std::endl;
        return;
    }

    double obj_pixel_len = cal::dist(refer_pts->at(0), refer_pts->at(1)); // (*refer_pts)[0]

    if(!ui->lineEdit_object_length->text().isEmpty())
    {
        double phy_length = ui->lineEdit_object_length->text().toDouble();
        double pixel_scale = phy_length/obj_pixel_len;
        dataBase->set_pixel_sacle(pixel_scale);
        QString text = QStringLiteral("%1 ").arg(pixel_scale, 0, 'f', 3);
        ui->lineEdit_pixel_scale_value->setText(text);
        reset_refer();
    }

    refer_pts = nullptr;
    delete refer_pts;
}

void AnalysisCenter::chose_detect_obj()
{
    roi_mask = cv::Mat();
    bool roi_reverse = ui->checkBox_roi_reverse->isChecked();
    const std::vector<cv::Point2i> *detect_vector = dataBase->get_detect_vector();
    if(detect_vector->size() > 2){
        cv::Point2i pts = dataBase->get_detect_vector()->at(0);
        dataBase->set_detect_point(pts);
        draw_img(*dataBase->get_detect_vector(), cv::Scalar( 255, 0, 0));
            // roi mask
        if(roi_reverse == false)
        {
            roi_mask = cv::Mat(imgCenter->imgSrc.size(), CV_8UC1, cv::Scalar(0));
            cv::polylines(roi_mask, *dataBase->get_detect_vector(), true, cv::Scalar(255));
            cv::fillPoly(roi_mask,  *dataBase->get_detect_vector(), cv::Scalar(255), 8, 0);
//            cv::imwrite("/Users/haoyulin/Desktop/new_qt/roi_mask.png", roi_mask);

        }else
        {
            roi_mask = cv::Mat(imgCenter->imgSrc.size(), CV_8UC1, cv::Scalar(255));
            cv::polylines(roi_mask, *dataBase->get_detect_vector(), true, cv::Scalar(0));
            cv::fillPoly(roi_mask,  *dataBase->get_detect_vector(), cv::Scalar(0), 8, 0);
//            cv::imwrite("/Users/haoyulin/Desktop/new_qt/roi_mask.png", roi_mask);
        }

    }else if(detect_vector->size() == 2 || detect_vector->size() == 1)
    {
        std::cout << "you need select at least 3 points." << std::endl;
        return;
    }else
    {
        if(roi_reverse == false)
        {
            roi_mask = cv::Mat(imgCenter->imgSrc.size(), CV_8UC1, cv::Scalar(255));
        }else
        {
            roi_mask = cv::Mat(imgCenter->imgSrc.size(), CV_8UC1, cv::Scalar(0));
        }
    }
    detect_vector = nullptr;
    delete detect_vector;
}

void AnalysisCenter::reset_detect()
{
    dataBase->init_detect_vector();
    roi_mask = cv::Mat();
    dataBase->del_threshold();
    dataBase->del_contours();

    draw_img();
}

void AnalysisCenter::detect_particle()
{
    if(ui->lineEdit_pixel_scale_value->text().isEmpty())
    {
        std::cout << "you need check refer_obj " << std::endl;
        return;
    }

    bool roi_reverse = ui->checkBox_roi_reverse->isChecked();
    if(roi_mask.empty())
    {
        if(roi_reverse == false)
        {
            roi_mask = cv::Mat(imgCenter->imgSrc.size(), CV_8UC1, cv::Scalar(255));
        }else
        {
            roi_mask = cv::Mat(imgCenter->imgSrc.size(), CV_8UC1, cv::Scalar(0));
        }
    }

    dataBase->del_threshold();

    white_balance();

//    std::vector<cv::Mat> rgb_channels(3);
//    cv::split(imgsrc, rgb_channels);


//    std::vector<cv::Mat> threshold_vector;
    double channel_value = ui->lineEdit_rgb_value->text().toDouble();
    if(channel_value == -1){
        cv::Scalar b_mean;
        b_mean = cv::mean(binary_map, roi_mask);
//        b_mean = (b_mean * 58/100);
        channel_value = (int)(b_mean[0]);
        QString text = QStringLiteral("%1 ").arg(channel_value, 0, 'f', 1);
        ui->lineEdit_rgb_value->setText(text);
    }

    cv::imwrite("/Users/haoyulin/Desktop/new_qt/binary_map.png", binary_map);
//    cv::Mat blue;
    cv::Mat detect_threshold;
    cv::threshold(binary_map, detect_threshold, (double) channel_value, 255, cv::THRESH_BINARY_INV);    // 0:THRESH_BINARY, 1:THRESH_BINARY_INV

//    rgb_channels.clear();
//    std::vector<cv::Mat>().swap(rgb_channels);


//    cv::cvtColor(imgsrc, imgsrc, cv::COLOR_HSV2RGB);
    /*
     * if threshold_length <= 2 -> both of 2 selection need true;
     * else avg need avg > 255 * 0.7
     */
//    double thresh_value = 254;

//    blue.convertTo(blue, CV_8UC1);

//    cv::threshold(blue, detect_threshold, (double)thresh_value, 253, cv::THRESH_BINARY);
//    detect_threshold = detect_threshold + 2;
//    blue.release();
    detect_threshold.convertTo(detect_threshold, CV_8UC1);

    /*
     * if src1 == src2 ---> 255
     * else            ---> 0
     */
    cv::Mat mask_thrshold;
//    cv::compare(detect_threshold, roi_mask, mask_thrshold, cv::CMP_EQ);
    cv::bitwise_and(detect_threshold, roi_mask, mask_thrshold);
    /*
     * Not need---> 255
     * need ---> 0
     */
    detect_threshold.release();

    std::vector<std::vector<cv::Point>> detect_contours;
    cv::findContours(mask_thrshold, detect_contours, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

    detect_contours.erase(std::remove_if(detect_contours.begin(), detect_contours.end(), [](std::vector<cv::Point>& contour) {
           return contour.size() < 5; }), detect_contours.end());


    cal_contours(&detect_contours);
    dataBase->del_contours();
    dataBase->set_detect_contours(detect_contours);
    set_flag->flag_contours = true;

    mask_thrshold.release();
    detect_contours.clear();
    std::vector<std::vector<cv::Point>>().swap(detect_contours);
    draw_contours_img();
}

void AnalysisCenter::white_balance()
{
    cv::Mat imgsrc = imgCenter->imgSrc.clone();
    cv::Scalar means = cv::mean(imgsrc);

    std::vector<cv::Mat> rgb_channels(3);
    cv::split(imgsrc, rgb_channels);
    cv::Mat& r_channel = rgb_channels[0];
    cv::Mat& g_channel = rgb_channels[1];
    cv::Mat& b_channel = rgb_channels[2];

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

    cv::multiply(b_channel, bCoef, b_channel);
    cv::multiply(g_channel, gCoef, g_channel);
    cv::multiply(r_channel, rCoef, r_channel);

    b_channel = min(b_channel, cv::Scalar(255));
    g_channel = min(g_channel, cv::Scalar(255));
    r_channel = min(r_channel, cv::Scalar(255));

    cv::merge(rgb_channels, imgsrc);

//    cv::cvtColor(imgsrc, imgsrc, cv::COLOR_BGR2RGB);
//    cv::imwrite("/Users/haoyulin/Desktop/new_qt/blue_channel1.png", imgsrc);


    cv::Scalar new_means = cv::mean(imgsrc);

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

    cv::Mat new_mask_1;
    cv::compare(b_channel, b_mean_new, new_mask_1, cv::CMP_LE);
    new_mask_1 = new_mask_1 * 0.5;

    cv::Mat new_mask_2;
    cv::compare(b_channel, b_mean, new_mask_2, cv::CMP_LE);
    new_mask_2 = new_mask_2 * 0.5;

    cv::Mat new_mask = new_mask_1 + new_mask_2;

    binary_map = cv::Mat(b_channel.size(), CV_8UC1, cv::Scalar(255));

    for(int i = 0; i < b_channel.rows; i++){
        for(int j = 0; j < b_channel.cols; j++){
            uchar image_value = b_channel.at<uchar>(i, j);
            uchar mask_value = new_mask.at<uchar>(i, j);

            if (mask_value > 200) {
                binary_map.at<uchar>(i, j) = cv::saturate_cast<uchar>(new_coeff * new_coeff * new_coeff  *image_value);

            } else if(mask_value < 200 && mask_value > 100){
                binary_map.at<uchar>(i, j) = cv::saturate_cast<uchar>(new_coeff  * new_coeff  *image_value);

            }else {
                binary_map.at<uchar>(i, j) = cv::saturate_cast<uchar>(new_coeff  * image_value);
            }
        }
    }
    rgb_channels.clear();
    std::vector<cv::Mat>().swap(rgb_channels);
}



void AnalysisCenter::draw_contours_img()
{
    if(set_flag->flag_contours == false) return;
    cv::Mat imgsrc = imgCenter->imgSrc.clone();
    const std::vector<std::vector<cv::Point> >* contours = dataBase->get_detect_contours();
    cv::drawContours(imgsrc, *contours, -1, cv::Scalar(255, 0, 0), 1, cv::LINE_AA);

    QImage tmp_img(imgsrc.data,
               imgsrc.cols, // width
               imgsrc.rows, // height
               imgsrc.step,
               QImage::Format_RGB888);

//    cv::cvtColor(imgsrc,imgsrc, cv::COLOR_BGR2RGB);
//    std::vector<int> compression_params;
//    compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
//    compression_params.push_back(9);
//    cv::imwrite("/Users/haoyulin/Desktop/new_qt/CHAIN_APPROX_NONE_2.png", imgsrc, compression_params);

    dataBase->set_origimg(tmp_img);
    imgCenter->set_img();

    imgsrc.release();
    contours = nullptr;
    delete contours;
}

void AnalysisCenter::cal_contours(std::vector<std::vector<cv::Point>> *contours)
{
    int countout_idx;
    double pixel_sacle = ui->lineEdit_pixel_scale_value->text().toDouble();
    countout_idx = 0;
    double scaled = std::pow(pixel_sacle, 2);

    const statis_t* data1_area = dataBase->get_data1_area();
    const statis_t* data1_diameter = dataBase->get_data1_diameter();
    dataBase->init_statis_t(*data1_area);
    dataBase->init_statis_t(*data1_diameter);

    for(int i = 0; i < contours->size(); i++){
        double num_pixel = cv::contourArea(contours->at(i));
        double surface1 = (num_pixel * scaled);

        cv::RotatedRect rect = minAreaRect(contours->at(i));
        double surface2 = (rect.size.width  * rect.size.height) * scaled;
        cv::RotatedRect ellipse_rect = fitEllipse(contours->at(i));
        double surface3 = (CV_PI * ellipse_rect.size.width * ellipse_rect.size.height / 4.0) * scaled;


        double min_rect;
        if(surface2 == 0)
        {
            min_rect = surface3;
        }else if(surface3 == 3)
        {
             min_rect = surface2;
        }else
        {
            min_rect = std::min(surface2, surface3);
        }
        double surface = surface1 * 0.5 + min_rect * 0.5;
        double diameter = std::sqrt(surface/CV_PI) * 2;

        if(surface >= 0.01 && surface <= 20)
        {
            dataBase->set_data_value(*data1_area, (float)surface);
            dataBase->set_data_value(*data1_diameter, (float)diameter);
            countout_idx = countout_idx + 1;
            dataBase->set_particle(countout_idx, surface, diameter);

        }else
        {
            contours->erase(contours->begin() + i);
        }
    }
    dataBase->sort_statis_t(*data1_area);
    statistics(*data1_area);
    dataBase->sort_statis_t(*data1_diameter);
    statistics(*data1_diameter);
    set_flag->flag_data1 = true;
    update_label();

}

void AnalysisCenter::createbar_area(const statis_t& data)
{
    if(!dataBase->get_hist_img().isNull()){
        imgCenter->set_hist_img();
        return;
    }

    const std::vector<float>* area = &data.value;

//    float mean = data.avg;
    float d50 = data.d50;
    float sd = data.sd;
    int count = data.cont;

    const float max_area = std::min(d50 + 2 * sd, area->back());
    const float min_area = std::max(d50 - 2 * sd, area->front());
    const float area_range = max_area - min_area;
    float bin_size, max_count = 0;

    int nums_bins = ui->lineEdit_nums_bins->text().isEmpty() ? -1 : ui->lineEdit_nums_bins->text().toInt();

    if(nums_bins == -1 || nums_bins == 0)
    {
        bin_size = 0.01;
        nums_bins =  static_cast<int>(area_range / bin_size) + 1;
    }else
    {
        bin_size = area_range / nums_bins;
    }

    std::vector<int> counters(nums_bins);
    int acc_count = 0;

    for (auto& a : *area)
    {
        if (a < min_area || a > max_area) {
            continue;
        }
        int bin_index = static_cast<int>((a - min_area) / bin_size);
        bin_index = std::min(bin_index, nums_bins - 1);
        counters[bin_index]++;
        if (counters[bin_index] > max_count) {
            max_count = counters[bin_index];
        }
        acc_count ++;
    }

    QBarSet* p_bar_set = new QBarSet("Particle Histogram");
    QStringList partical_size;
    QLineSeries* p_line_series = new QLineSeries();

    p_line_series->setPointLabelsVisible(true);
    p_line_series->setPointLabelsFormat("@yPoint");
    float accmulate_percent = 0;
    int D_70 = 0;
    for (int i = 0; i < nums_bins; i++) {
        float bin_start = min_area + bin_size * i;
        float bin_end = bin_start + bin_size;
        float percentage = (static_cast<float>(counters[i]) / acc_count) * 100.0;
        percentage = std::round(percentage * 100.0) / 100.0;
        accmulate_percent += percentage;
        *p_bar_set << percentage;
        if(accmulate_percent <= 70 ){
           D_70 ++;
        }
        QString bin_label = QString::number(bin_start, 'f', 2) + " - " + QString::number(bin_end, 'f', 2);
        partical_size.append(bin_label);
        p_line_series->append(i+bin_size/2, accmulate_percent);
    }

    counters.clear();
    std::vector<int>().swap(counters);


    QBarSeries *series = new QBarSeries();
    series->append(p_bar_set);
    QColor label_color = QColor(Qt::black);
    p_bar_set->setLabelColor(label_color);

    series->setLabelsVisible(true);
    series->setLabelsPosition(QAbstractBarSeries::LabelsOutsideEnd);
//    series->setBarWidth(1);
    series->setUseOpenGL(true);



//    // for X
    QBarCategoryAxis *p_axisX = new QBarCategoryAxis();
//    QCategoryAxis *p_axisX = new QCategoryAxis();
//    for(int i = 0; i < partical_size.size(); i++){
//         p_axisX->append(partical_size[i], i+1);
//    }
    p_axisX->append(partical_size);
    p_axisX->setTitleText("Particle Size [ mm x mm ]");
    p_axisX->setLabelsFont(QFont("Arial", 60));

    QValueAxis *p_axisYL = new QValueAxis();
    int max_range = (int)(((static_cast<float>(max_count)/count)*100.0)/20 + 2) * 20;
    p_axisYL->setRange(0, max_range);
    p_axisYL->setTickType(QValueAxis::TickType::TicksDynamic);
    p_axisYL->setTickInterval(max_range/20);
    p_axisYL->setTickAnchor(0);
    p_axisYL->setLabelFormat("%d");
    p_axisYL->setTitleText("Particle Size Ratio [ % ]");
    p_axisYL->setLabelsFont(QFont("Arial", 60));


    QValueAxis *p_axisYR = new QValueAxis();
    p_axisYR->setRange(0, 100);
    p_axisYR->setTickType(QValueAxis::TickType::TicksDynamic);
    p_axisYR->setTickInterval(5);
    p_axisYR->setTickAnchor(0);
    p_axisYR->setLabelFormat("%d");
    p_axisYR->setTitleText("Accumulate Ratio [ % ]");
    p_axisYR->setLabelsFont(QFont("Arial", 60));


//    // setting Chart
    QChart *p_chart = new QChart();
    QChartView chartView;
    chartView.setChart(p_chart);

    p_chart->createDefaultAxes();
    p_chart->setTitle("Distribution");

    p_chart->addAxis(p_axisX, Qt::AlignBottom);
    p_chart->addAxis(p_axisYL, Qt::AlignLeft);
    p_chart->addAxis(p_axisYR, Qt::AlignRight);

    p_chart->setTheme(QChart::ChartThemeBrownSand);
    p_chart->addSeries(series);
    p_chart->addSeries(p_line_series);
    series->attachAxis(p_axisX);
    series->attachAxis(p_axisYL);
    p_line_series->attachAxis(p_axisX);
    p_line_series->attachAxis(p_axisYR);

    p_chart->legend()->setVisible(false);
    p_chart->legend()->setAlignment(Qt::AlignBottom);

//    p_chart->setTheme(QChart::ChartThemeBrownSand);



//    QChartView *chartView = new QChartView(p_chart);
    if(nums_bins == -1){
        nums_bins = partical_size.size();
    }
    chartView.resize(600 * nums_bins/5, 1080);



    QList<QGraphicsRectItem *> rect_items;
    for(QGraphicsItem * it : chartView.items()){
        if(QGraphicsRectItem *rect = qgraphicsitem_cast<QGraphicsRectItem *>(it)){
            if(rect->parentItem() != p_chart && rect->parentItem()->parentItem() == p_chart){
                rect_items << rect;
            }
        }
    }
    int cnt = nums_bins - D_70 - 1;
    for(QGraphicsRectItem * rect : rect_items){

        if(cnt > 0){
            cnt --;
        }else{
            QColor label_color = QColor("#FFC0CB");
            rect->setBrush(label_color);
        }

    }
//    std::cout << "chartView.width()" << chartView.width() << std::endl;
//    std::cout << "chartView.height() " << chartView.height() << std::endl;
    QPixmap p = chartView.grab();
//    QPixmap croppedPixmap = p.copy(0, 0, chartView.width(), chartView.height());

    //   ui->label_image->setPixmap(p);
//    std::cout << "p.width()" << p.width() << std::endl;
//    std::cout << "p.height() " << p.height() << std::endl;
    QImage hist_image = p.toImage();
//    hist_image.save("/Users/haoyulin/Desktop/new_qt/hist.jpg", "JPG", 100);
    dataBase->set_hist_qimage(hist_image);
    imgCenter->set_hist_img();
    delete p_chart;
}

void AnalysisCenter::createbar_diameter(const statis_t& data)
{
    if(!dataBase->get_hist_img().isNull()){
        imgCenter->set_hist_img();
        return;
    }

    const std::vector<float>* dimater = &data.value;

//    float mean = data.avg;
    float d50 = data.d50;
    float sd = data.sd;
    int count = data.cont;

    const float max_diameter = std::min(d50 + 2 * sd, dimater->back());
    const float min_diameter = std::max(d50 - 2 * sd,  dimater->front());
    const float diameter_range = max_diameter - min_diameter;
    float bin_size, max_count = 0;

    int nums_bins = ui->lineEdit_nums_bins->text().isEmpty() ? -1 : ui->lineEdit_nums_bins->text().toInt();

    if(nums_bins == -1 || nums_bins == 0)
    {
        bin_size = 0.05;
        nums_bins =  static_cast<int>(diameter_range / bin_size) + 1;
    }else
    {
        bin_size = diameter_range / nums_bins;
    }

    std::vector<int> counters(nums_bins);
    int acc_count = 0;
    for (auto& a : *dimater)
    {
        if (a < min_diameter || a > max_diameter) {
            continue;
        }
        int bin_index = static_cast<int>((a - min_diameter) / bin_size);
        bin_index = std::min(bin_index, nums_bins - 1);
        counters[bin_index]++;
        if (counters[bin_index] > max_count) {
            max_count = counters[bin_index];
        }
        acc_count ++;
    }

    QBarSet* p_bar_set = new QBarSet("Particle Histogram");
    QStringList partical_size;
    QLineSeries* p_line_series = new QLineSeries();
    p_line_series->setPointLabelsVisible(true);
    p_line_series->setPointLabelsFormat("@yPoint");
    float accmulate_percent = 0;
    int D_85 = 0;
    for (int i = 0; i < nums_bins; i++) {
        float bin_start = min_diameter + bin_size * i;
        float bin_end = bin_start + bin_size;
        float percentage = (static_cast<float>(counters[i]) / acc_count) * 100.0;
        percentage = std::round(percentage * 100.0) / 100.0;
        accmulate_percent += percentage;
        *p_bar_set << percentage;
        if(accmulate_percent <= 85 ){
           D_85 ++;
        }
        QString bin_label = QString::number(bin_start, 'f', 2) + " - " + QString::number(bin_end, 'f', 2);
        partical_size.append(bin_label);
        p_line_series->append(i+bin_size/2, accmulate_percent);
    }

    counters.clear();
    std::vector<int>().swap(counters);


    QBarSeries *series = new QBarSeries();
    series->append(p_bar_set);
    QColor label_color = QColor(Qt::black);
    p_bar_set->setLabelColor(label_color);

    series->setLabelsVisible(true);
    series->setLabelsPosition(QAbstractBarSeries::LabelsOutsideEnd);
//    series->setBarWidth(1);
    series->setUseOpenGL(true);



//    // for X
    QBarCategoryAxis *p_axisX = new QBarCategoryAxis();

    p_axisX->append(partical_size);
    p_axisX->setTitleText("Particle Diameter [ mm ]");
    p_axisX->setLabelsFont(QFont("Arial", 60));

    QValueAxis *p_axisYL = new QValueAxis();
    int max_range = (int)(((static_cast<float>(max_count)/count)*100.0)/20 + 2) * 20;
    p_axisYL->setRange(0, max_range);
    p_axisYL->setTickType(QValueAxis::TickType::TicksDynamic);
    p_axisYL->setTickInterval(max_range/20);
    p_axisYL->setTickAnchor(0);
    p_axisYL->setLabelFormat("%d");
    p_axisYL->setTitleText("Particle Size Ratio [ % ]");
    p_axisYL->setLabelsFont(QFont("Arial", 60));


    QValueAxis *p_axisYR = new QValueAxis();
    p_axisYR->setRange(0, 100);
    p_axisYR->setTickType(QValueAxis::TickType::TicksDynamic);
    p_axisYR->setTickInterval(5);
    p_axisYR->setTickAnchor(0);
    p_axisYR->setLabelFormat("%d");
    p_axisYR->setTitleText("Accumulate Ratio [ % ]");
    p_axisYR->setLabelsFont(QFont("Arial", 60));


//    // setting Chart
    QChart *p_chart = new QChart();
    QChartView chartView;
    chartView.setChart(p_chart);

    p_chart->createDefaultAxes();
    p_chart->setTitle("Distribution");

    p_chart->addAxis(p_axisX, Qt::AlignBottom);
    p_chart->addAxis(p_axisYL, Qt::AlignLeft);
    p_chart->addAxis(p_axisYR, Qt::AlignRight);

    p_chart->setTheme(QChart::ChartThemeBrownSand);
    p_chart->addSeries(series);
    p_chart->addSeries(p_line_series);
    series->attachAxis(p_axisX);
    series->attachAxis(p_axisYL);
    p_line_series->attachAxis(p_axisX);
    p_line_series->attachAxis(p_axisYR);

    p_chart->legend()->setVisible(false);
    p_chart->legend()->setAlignment(Qt::AlignBottom);

//    p_chart->setTheme(QChart::ChartThemeBrownSand);



//    QChartView *chartView = new QChartView(p_chart);
    if(nums_bins == -1){
        nums_bins = partical_size.size();
    }
    chartView.resize(600 * nums_bins/5, 1080);



    QList<QGraphicsRectItem *> rect_items;
    for(QGraphicsItem * it : chartView.items()){
        if(QGraphicsRectItem *rect = qgraphicsitem_cast<QGraphicsRectItem *>(it)){
            if(rect->parentItem() != p_chart && rect->parentItem()->parentItem() == p_chart){
                rect_items << rect;
            }
        }
    }
    int cnt = nums_bins - D_85 - 1;
    for(QGraphicsRectItem * rect : rect_items){

        if(cnt > 0){
            cnt --;
        }else{
            QColor label_color = QColor("#FFC0CB");
            rect->setBrush(label_color);
        }

    }

    QPixmap p = chartView.grab();
    QImage hist_image = p.toImage();
    dataBase->set_hist_qimage(hist_image);
    imgCenter->set_hist_img();
    delete p_chart;
}

void AnalysisCenter::producehist1()
{
    if(ui->comboBox_histstate->currentIndex() == 0){
        createbar_area(*dataBase->get_data1_area());
    }else{
        createbar_diameter(*dataBase->get_data1_diameter());
    }
    update_label();

}

void AnalysisCenter::producehist2()
{
    if(ui->comboBox_histstate->currentIndex() == 0){
        createbar_area(*dataBase->get_data2_area());
    }else{
        createbar_diameter(*dataBase->get_data2_diameter());
    }
    update_label();
}


void AnalysisCenter::reproducehist1()
{
    dataBase->del_hist_qimg();

    if(ui->comboBox_histstate->currentIndex() == 0){
        createbar_area(*dataBase->get_data1_area());
    }else{
        createbar_diameter(*dataBase->get_data1_diameter());
    }
    update_label();

}

void AnalysisCenter::reproducehist2()
{
    dataBase->del_hist_qimg();
    if(ui->comboBox_histstate->currentIndex() == 0){
        createbar_area(*dataBase->get_data2_area());
    }else{
        createbar_diameter(*dataBase->get_data2_diameter());
    }
    update_label();
}

void AnalysisCenter::statistics(const statis_t& data)
{
    std::map<float, int> counter;
    const int& total_cont = data.cont;
    float avg = 0.0;
    float sd = 0.0;
    float mode = 0.0;
    float d20 = 0.0;
    float d50 = 0.0;
    float d70 = 0.0;

    // d20
    int d20_index = static_cast<int>(total_cont * 0.2);
    d20 = data.value.at(d20_index);

    // d50
    if(total_cont % 2 == 0)
    {
         d50 =  (data.value.at(total_cont/2 - 1) + data.value.at(total_cont / 2)) / 2.0;
    }else{
         d50 = data.value.at(total_cont / 2);
    }

    // d70
    int d70_index = static_cast<int>(total_cont * 0.85) + 1;
    d70 = data.value.at(d70_index);

    int max_count = 0;
    int max_range = (int)(data.value.back() * 100 + 1);
    int min_range = (int)(data.value.front() * 100 - 1);

    for(int i = min_range; i <= max_range; i += 1)
    {
        float tmp = i/100.0;
        counter[tmp] = 0;
    }

    float sum = 0.0;
    for(auto value : data.value)
    {
        sum += value;
    }
    avg = (float)sum/total_cont;

    for(const auto &a : data.value)
    {
        sd += std::pow(a - avg, 2);
        double tmp_area = std::round(a*100.0)/100.0;
        counter[tmp_area] ++;

        if(counter[tmp_area] > max_count)
        {
            max_count = counter[tmp_area];
            mode = tmp_area;
        }
    }
    sd = std::sqrt(sd/total_cont);
    dataBase->set_data_statis(data, avg, sd, mode, d20, d50, d70);
//    statis_data->mode = mode;
//    statis_data->sd = sd;
    counter.clear();
    std::map<float, int>().swap(counter);
}

void AnalysisCenter::update_label()
{
    if(set_flag->flag_data1 == false) return;
    const statis_t* data1;
    const statis_t* data2;

    if(set_flag->flag_data2 == true)
    {
        if(ui->comboBox_histstate->currentIndex() == 0)
        {
            ui->label_property_title->setText("Properties of the Surface Distribution");
            data1 = dataBase->get_data1_area();
            data2 = dataBase->get_data2_area();
            ui->label_avgsurface->setText(QString("Average : (1) %1, (2) %2").arg(QString::number(data1->avg, 'f', 4),
                                          QString::number(data2->avg, 'f', 4)));
            ui->label_deviation->setText(QString("Standard Deviation : (1) %1, (2) %2").arg(QString::number(data1->sd, 'f', 4), QString::number(data2->sd, 'f', 4)));
            ui->label_mode->setText(QString("Mode : (1) %1, (2) %2").arg(QString::number(data1->mode, 'f', 2), QString::number(data2->mode, 'f', 2)));
            ui->label_nums->setText(QString("Nums Coffee Particle : (1) %1, (2) %2").arg(data1->cont).arg(data2->cont));
            ui->label_d20->setText(QString("D20 : (1) %1, (2) %2").arg(QString::number(data1->d20, 'f', 4), QString::number(data2->d20, 'f', 4)));
            ui->label_d50->setText(QString("D50 : (1) %1, (2) %2").arg(QString::number(data1->d50, 'f', 4), QString::number(data2->d50, 'f', 4)));
            ui->label_d70->setText(QString("D70 : (1) %1, (2) %2").arg(QString::number(data1->d70, 'f', 4), QString::number(data2->d70, 'f', 4)));
        }else
        {
            ui->label_property_title->setText("Properties of the Diameter Distribution");
            data1 = dataBase->get_data1_diameter();
            data2 = dataBase->get_data2_diameter();
            ui->label_avgsurface->setText(QString("Average : (1) %1, (2) %2").arg(QString::number(data1->avg, 'f', 4),
                                          QString::number(data2->avg, 'f', 4)));
            ui->label_deviation->setText(QString("Standard Deviation : (1) %1, (2) %2").arg(QString::number(data1->sd, 'f', 4), QString::number(data2->sd, 'f', 4)));
            ui->label_mode->setText(QString("Mode : (1) %1, (2) %2").arg(QString::number(data1->mode, 'f', 2), QString::number(data2->mode, 'f', 2)));
            ui->label_nums->setText(QString("Nums Coffee Particle : (1) %1, (2) %2").arg(data1->cont).arg(data2->cont));
            ui->label_d20->setText(QString("D20 : (1) %1, (2) %2").arg(QString::number(data1->d20, 'f', 4), QString::number(data2->d20, 'f', 4)));
            ui->label_d50->setText(QString("D50 : (1) %1, (2) %2").arg(QString::number(data1->d50, 'f', 4), QString::number(data2->d50, 'f', 4)));
            ui->label_d70->setText(QString("D85 : (1) %1, (2) %2").arg(QString::number(data1->d70, 'f', 4), QString::number(data2->d70, 'f', 4)));
        }

    }else
    {
        if(ui->comboBox_histstate->currentIndex() == 0)
        {
            ui->label_property_title->setText("Properties of the Surface Distribution");
            data1 = dataBase->get_data1_area();
            ui->label_avgsurface->setText(QString("Average: %1").arg(QString::number(data1->avg, 'f', 4)));
            ui->label_deviation->setText(QString("Standard Deviation : %1").arg(QString::number(data1->sd, 'f', 4)));
            ui->label_mode->setText(QString("Mode: %1").arg(QString::number(data1->mode, 'f', 2)));
            ui->label_nums->setText(QString("Nums Coffee Particle : %1").arg(data1->cont));
            ui->label_d20->setText(QString("D20 : %1").arg(QString::number(data1->d20, 'f', 4)));
            ui->label_d50->setText(QString("D50 : %1").arg(QString::number(data1->d50, 'f', 4)));
            ui->label_d70->setText(QString("D70 : %1").arg(QString::number(data1->d70, 'f', 4)));
        }else
        {
            ui->label_property_title->setText("Properties of the Diameter Distribution");
            data1 = dataBase->get_data1_diameter();
            ui->label_avgsurface->setText(QString("Average: %1").arg(QString::number(data1->avg, 'f', 4)));
            ui->label_deviation->setText(QString("Standard Deviation : %1").arg(QString::number(data1->sd, 'f', 4)));
            ui->label_mode->setText(QString("Mode: %1").arg(QString::number(data1->mode, 'f', 2)));
            ui->label_nums->setText(QString("Nums Coffee Particle : %1").arg(data1->cont));
            ui->label_d20->setText(QString("D20 : %1").arg(QString::number(data1->d20, 'f', 4)));
            ui->label_d50->setText(QString("D50 : %1").arg(QString::number(data1->d50, 'f', 4)));
            ui->label_d70->setText(QString("D85 : %1").arg(QString::number(data1->d70, 'f', 4)));
        }

    }

}

void AnalysisCenter::load_data1(const QString &fileName)
{
    QFile datafile(fileName);
    if(!datafile.open(QFile::ReadOnly | QFile::Text))
    {
        std::cout << "error" << std::endl;
        return;
    }
    QTextStream in(&datafile);
    bool firstline = true;
    int countout_idx = 0;
    double surface_average = 0;

    const statis_t* data1_area = dataBase->get_data1_area();
    const statis_t* data1_diameter = dataBase->get_data1_diameter();

    while(!in.atEnd())
    {
        QString line = in.readLine();
        if(firstline){
            firstline = false;
            continue;
        }

        QStringList values = line.split(",");
        float tmp_area = values[1].toFloat();
        float tmp_diameter = values[2].toFloat();
        dataBase->set_data_value(*data1_area, (float)tmp_area);
        dataBase->set_data_value(*data1_diameter, (float)tmp_diameter);

    }

    dataBase->sort_statis_t(*data1_area);
    statistics(*data1_area);
    dataBase->sort_statis_t(*data1_diameter);
    statistics(*data1_diameter);
    set_flag->flag_data1 = true;
    update_label();
    datafile.close();

}

void AnalysisCenter::load_data2(const QString &fileName)
{
    QFile datafile(fileName);
    if(!datafile.open(QFile::ReadOnly | QFile::Text))
    {
        std::cout << "error" << std::endl;
        return;
    }
    QTextStream in(&datafile);
    bool firstline = true;
    int countout_idx = 0;
    double surface_average = 0;

    const statis_t* data2_area = dataBase->get_data2_area();
    const statis_t* data2_diameter = dataBase->get_data2_diameter();


    while(!in.atEnd())
    {
        QString line = in.readLine();
        if(firstline){
            firstline = false;
            continue;
        }

        QStringList values = line.split(",");
        float tmp_area = values[1].toFloat();
//        dataBase->set_contours_area(area);
        float tmp_diameter = values[2].toFloat();
        dataBase->set_data_value(*data2_area, (float)tmp_area);
        dataBase->set_data_value(*data2_diameter, (float)tmp_diameter);

    }

    dataBase->sort_statis_t(*data2_area);
    statistics(*data2_area);
    dataBase->sort_statis_t(*data2_diameter);
    statistics(*data2_diameter);
    set_flag->flag_data2 = true;
    update_label();
    datafile.close();

}




























