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
        }else
        {
            roi_mask = cv::Mat(imgCenter->imgSrc.size(), CV_8UC1, cv::Scalar(255));
            cv::polylines(roi_mask, *dataBase->get_detect_vector(), true, cv::Scalar(0));
            cv::fillPoly(roi_mask,  *dataBase->get_detect_vector(), cv::Scalar(0), 8, 0);
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
    dataBase->del_all_detect_vector();
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
    cv::Mat imgsrc = imgCenter->imgSrc.clone();
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

    std::vector<cv::Mat> rgb_channels(3);
    cv::split(imgsrc, rgb_channels);


//    std::vector<cv::Mat> threshold_vector;
    double channel_value = ui->lineEdit_rgb_value->text().toDouble();
    if(channel_value == -1){
        cv::Scalar rgb_mean;
        rgb_mean = cv::mean(imgsrc, roi_mask);
        rgb_mean = (rgb_mean * 58/100);
        channel_value = (int)(rgb_mean[2]);
        QString text = QStringLiteral("%1 ").arg(channel_value, 0, 'f', 1);
        ui->lineEdit_rgb_value->setText(text);
    }


    cv::Mat blue;
    cv::threshold(rgb_channels[2], blue, (double) channel_value, 255, cv::THRESH_BINARY_INV);    // 0:THRESH_BINARY, 1:THRESH_BINARY_INV
//    threshold_vector.push_back(blue);
//    blue.release();

    rgb_channels.clear();
    std::vector<cv::Mat>().swap(rgb_channels);


    cv::cvtColor(imgsrc, imgsrc, cv::COLOR_HSV2RGB);
    /*
     * if threshold_length <= 2 -> both of 2 selection need true;
     * else avg need avg > 255 * 0.7
     */
    double thresh_value = 254;

    blue.convertTo(blue, CV_8UC1);
    cv::Mat detect_threshold;
    cv::threshold(blue, detect_threshold, (double)thresh_value, 253, cv::THRESH_BINARY);
    detect_threshold = detect_threshold + 2;
    blue.release();
    detect_threshold.convertTo(detect_threshold, CV_8UC1);

    /*
     * if src1 == src2 ---> 255
     * else            ---> 0
     */
    cv::Mat mask_thrshold;
    cv::compare(detect_threshold, roi_mask, mask_thrshold, cv::CMP_EQ);
//    cv::bitwise_not(mask_thrshold, mask_thrshold);
    /*
     * Not need---> 255
     * need ---> 0
     */
    detect_threshold.release();

    std::vector<std::vector<cv::Point>> detect_contours;
    cv::findContours(mask_thrshold, detect_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    detect_contours.erase(std::remove_if(detect_contours.begin(), detect_contours.end(), [](std::vector<cv::Point>& contour) {
           return contour.size() < 5; }), detect_contours.end());


    cal_contours(&detect_contours);
    dataBase->del_contours();
    dataBase->set_detect_contours(detect_contours);
    set_flag->flag_contours = true;

    mask_thrshold.release();
    detect_contours.clear();
    std::vector<std::vector<cv::Point>>().swap(detect_contours);
    imgsrc.release();

    draw_contours_img();
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
    double surface_average = 0;
    double pixel_sacle = ui->lineEdit_pixel_scale_value->text().toDouble();
    countout_idx = 0;
    double scaled = std::pow(pixel_sacle, 2);
    dataBase->del_area();

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
            surface_average += surface;
            dataBase->set_data1_area((float)surface);
            dataBase->set_data1_diameter((float)diameter);
            countout_idx = countout_idx + 1;
            dataBase->set_particle(countout_idx, surface, diameter);

        }else
        {
            contours->erase(contours->begin() + i);
        }
    }

    surface_average = surface_average/countout_idx;
    dataBase->sort_area();
    dataBase->sort_diameter();
    data1_statis.avg = surface_average;
    data1_statis.cont = countout_idx;
    statistics(&data1_statis);
    if(ui->checkBox_outlier->isChecked()){
        statistics_without_outlier();
    }
    update_label();
    set_flag->flag_data1 = true;
}

void AnalysisCenter::createBar1_area()
{
    if(set_flag->flag_data1 == false)
    {
        std::cout << "please, detect contours first" << std::endl;
        return;
    }

//    if(!dataBase->get_hist_img().isNull()){
//        imgCenter->set_his_img();
//        return;
//    }

    const std::vector<float>* contours_area = dataBase->get_data1_area();

    float mean, sd;
    if(ui->checkBox_outlier->isChecked())
    {
        mean = data1_noutliter_statis.avg;
        sd = data1_noutliter_statis.sd;
    }else
    {
        mean = data1_statis.avg;
        sd = data1_statis.sd;
    }

    const float max_area = std::min(mean + 2 * sd, contours_area->back());
    const float min_area = std::max(mean - 2 * sd,  contours_area->front());
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
    for (auto& a : *contours_area)
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
    int max_range = (int)(((static_cast<float>(max_count)/contours_area->size())*100.0)/20 + 2) * 20;
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
    int count = nums_bins - D_70 - 1;
    for(QGraphicsRectItem * rect : rect_items){

        if(count > 0){
            count --;
        }else{
            QColor label_color = QColor("#FFC0CB");
            rect->setBrush(label_color);
        }

    }

    QPixmap p = chartView.grab();
    //   ui->label_image->setPixmap(p);
    QImage hist_image = p.toImage();
//    hist_image.save("/Users/haoyulin/Desktop/new_qt/hist.jpg", "JPG", 100);
    dataBase->set_hist_qimage(hist_image);
    imgCenter->set_his_img();
    delete p_chart;
}

void AnalysisCenter::createBar1_diameter()
{
    if(set_flag->flag_data1 == false)
    {
        std::cout << "please, detect contours first" << std::endl;
        return;
    }

//    if(!dataBase->get_hist_img().isNull()){
//        imgCenter->set_his_img();
//        return;
//    }

    const std::vector<float>* contours_diameter = dataBase->get_data1_diameter();

    float mean, sd;
    if(ui->checkBox_outlier->isChecked())
    {
        mean = data1_noutliter_statis.avg;
        sd = data1_noutliter_statis.sd;
    }else
    {
        mean = data1_statis.avg;
        sd = data1_statis.sd;
    }

    mean = std::sqrt(mean/CV_PI) * 2;
    sd = std::sqrt(sd/CV_PI) * 2;
    std::cout << "ok1" << std::endl;
    const float max_diameter = std::min(mean + 2 * sd, contours_diameter->back());
    const float min_diameter = std::max(mean - 2 * sd,  contours_diameter->front());
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
    std::cout << "ok2" << std::endl;
    std::vector<int> counters(nums_bins);
    int acc_count = 0;
    for (auto& a : *contours_diameter)
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
     std::cout << "ok3" << std::endl;
    p_line_series->setPointLabelsVisible(true);
    p_line_series->setPointLabelsFormat("@yPoint");
    float accmulate_percent = 0;
    int D_70 = 0;
    for (int i = 0; i < nums_bins; i++) {
        float bin_start = min_diameter + bin_size * i;
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
    p_axisX->setTitleText("Particle Diameter [ mm ]");
    p_axisX->setLabelsFont(QFont("Arial", 60));

    QValueAxis *p_axisYL = new QValueAxis();
    int max_range = (int)(((static_cast<float>(max_count)/contours_diameter->size())*100.0)/20 + 2) * 20;
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
    int count = nums_bins - D_70 - 1;
    for(QGraphicsRectItem * rect : rect_items){

        if(count > 0){
            count --;
        }else{
            QColor label_color = QColor("#FFC0CB");
            rect->setBrush(label_color);
        }

    }

    QPixmap p = chartView.grab();
    QImage hist_image = p.toImage();
    dataBase->set_hist_qimage(hist_image);
    imgCenter->set_his_img();
    delete p_chart;
}

void AnalysisCenter::createBar2_area()
{
    if(set_flag->flag_data2 == false)
    {
        std::cout << "please, detect data2 first" << std::endl;
        return;
    }

    if(!dataBase->get_hist2_img().isNull()){
        imgCenter->set_his2_img();
        return;
    }

    float mean = data2_statis.avg;
    float sd = data2_statis.sd;


    const float max_area = std::min(mean + 2 * sd, data2_area.back());
    const float min_area = std::max(mean - 2 * sd,  data2_area.front());
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
    for (auto& a : data2_area)
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
    int max_range = (int)(((static_cast<float>(max_count)/data2_area.size())*100.0)/20 + 2) * 20;
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
    int count = nums_bins - D_70 - 1;
    for(QGraphicsRectItem * rect : rect_items){

        if(count > 0){
            count --;
        }else{
            QColor label_color = QColor("#FFC0CB");
            rect->setBrush(label_color);
        }

    }

    QPixmap p = chartView.grab();
    //   ui->label_image->setPixmap(p);
    QImage hist_image = p.toImage();
//    hist_image.save("/Users/haoyulin/Desktop/new_qt/hist.jpg", "JPG", 100);
    dataBase->set_hist2_qimage(hist_image);
    imgCenter->set_his2_img();
    delete p_chart;
}

void AnalysisCenter::reproducehist()
{
    dataBase->del_hist_qimg();
    if(ui->comboBox_histstate->currentText() == "Surface"){
        createBar1_area();
    }else{
        createBar1_diameter();
    }

}

void AnalysisCenter::reproducehist2()
{
    dataBase->del_hist2_qimg();
    createBar2_area();
}

void AnalysisCenter::statistics(statis_t* statis_data)
{
    std::map<float, int> counter;
    const std::vector<float>* contours_area = dataBase->get_data1_area();
    int contours_area_len = contours_area->size();
    int d20_index = static_cast<int>(contours_area_len * 0.2);
    statis_data->d20 = contours_area->at(d20_index);
    float d50 = 0.0;
    if (contours_area_len % 2 == 0)
    {
        d50 = (contours_area->at(contours_area_len/2 - 1) + contours_area->at(contours_area_len / 2)) / 2.0;
    } else {
        d50 = contours_area->at(contours_area_len / 2);
    }
    statis_data->d50 = d50;

    int d70_index = static_cast<int>(contours_area_len * 0.7);
    statis_data->d70 = contours_area->at(d70_index);
    int max_count = 0;
    int max = (int)(contours_area->back() * 100 + 1);
    int min = (int)(contours_area->front() * 100 - 1);
    float mode = 0;
    float sd = 0.0;
    float mean = statis_data->avg;

    for(int i = min; i <= max; i += 1)
    {
        float tmp = i/100.0;
        counter[tmp] = 0;
    }

    for(const auto &a : *contours_area)
    {
        sd += std::pow(a - mean, 2);
        double tmp_area = std::round(a*100.0)/100.0;
        counter[tmp_area] ++;

        if(counter[tmp_area] > max_count)
        {
            max_count = counter[tmp_area];
            mode = tmp_area;
        }
    }
    sd = std::sqrt(sd/contours_area_len);
    statis_data->mode = mode;
    statis_data->sd = sd;
    counter.clear();
    std::map<float, int>().swap(counter);
}

void AnalysisCenter::statistics_without_outlier(){
    const std::vector<float>* contours_area = dataBase->get_data1_area();
    // cal standard_deviation
    float mean = data1_statis.avg;
    float sd = 0.0;
    for(const auto &a : *contours_area)
    {
        sd += std::pow(a - mean, 2);
    }
    sd = std::sqrt(sd/contours_area->size());

    // delete outlier 2 standard_deviation
    float sum = 0.0;
    std::vector<float> filtered_area;
    for(const auto& a : *contours_area)
    {
        if(std::abs(a-mean) < 2.0 * sd){
            filtered_area.push_back(a);
            sum += a;
        }
    }
    int filtered_area_len = filtered_area.size();

    // cal avg
    float means = sum / static_cast<float>(filtered_area_len);
    data1_noutliter_statis.avg = means;

    // cal standard_deviation
    sd = 0.0;
    for(const auto &a : filtered_area)
    {
        sd += std::pow(a - means, 2);
    }
    sd = std::sqrt(sd/filtered_area_len);
    data1_noutliter_statis.sd = sd;

    // cal d20 d50 d70
    int d20_index = static_cast<int>(filtered_area_len * 0.2) - 1;
    data1_noutliter_statis.d20 = filtered_area[d20_index];

    std::sort(filtered_area.begin(), filtered_area.end());
    float d50;
    if (filtered_area_len % 2 == 0)
    {
        d50 = (filtered_area[(filtered_area_len) / 2 - 1] + filtered_area[filtered_area_len / 2]) / 2.0;
    } else {
        d50 = filtered_area[filtered_area_len / 2];
    }
    data1_noutliter_statis.d50 = d50;

    int d70_index = static_cast<int>(filtered_area_len * 0.7) - 1;
    data1_noutliter_statis.d70 = filtered_area[d70_index];
    data1_noutliter_statis.cont = filtered_area_len;
    filtered_area.clear();
    std::vector<float>().swap(filtered_area);
}

void AnalysisCenter::update_label()
{
    if(!set_flag->flag_data1) return;
    if(ui->checkBox_outlier->isChecked())
    {
        ui->label_avgsurface->setText(QString("Average Surface : %1 -> %2").arg(QString::number(data1_statis.avg, 'f', 4),
                                      QString::number(data1_noutliter_statis.avg, 'f', 4)));
        ui->label_deviation->setText(QString("Standard Deviation : %1 -> %2").arg(QString::number(data1_statis.sd, 'f', 4), QString::number(data1_noutliter_statis.sd, 'f', 4)));
        ui->label_mode->setText(QString("Mode Surface : %1").arg(QString::number(data1_statis.mode, 'f', 2)));
        ui->label_nums->setText(QString("Nums Coffee Particle : %1 -> %2").arg(data1_statis.cont).arg(data1_noutliter_statis.cont));
        ui->label_d20->setText(QString("D20 : %1 -> %2").arg(QString::number(data1_statis.d20, 'f', 4), QString::number(data1_noutliter_statis.d20, 'f', 4)));
        ui->label_d50->setText(QString("D50 : %1 -> %2").arg(QString::number(data1_statis.d50, 'f', 4), QString::number(data1_noutliter_statis.d50, 'f', 4)));
        ui->label_d70->setText(QString("D70 : %1 -> %2").arg(QString::number(data1_statis.d70, 'f', 4), QString::number(data1_noutliter_statis.d70, 'f', 4)));
    }else if(set_flag->flag_data2 == true)
    {
        ui->label_avgsurface->setText(QString("Average Surface : (1) %1, (2) %2").arg(QString::number(data1_statis.avg, 'f', 4),
                                      QString::number(data2_statis.avg, 'f', 4)));
        ui->label_deviation->setText(QString("Standard Deviation : (1) %1, (2) %2").arg(QString::number(data1_statis.sd, 'f', 4), QString::number(data2_statis.sd, 'f', 4)));
        ui->label_mode->setText(QString("Mode Surface : (1) %1, (2) %2").arg(QString::number(data1_statis.mode, 'f', 2), QString::number(data2_statis.mode, 'f', 2)));
        ui->label_nums->setText(QString("Nums Coffee Particle : (1) %1, (2) %2").arg(data1_statis.cont).arg(data2_statis.cont));
        ui->label_d20->setText(QString("D20 : (1) %1, (2) %2").arg(QString::number(data1_statis.d20, 'f', 4), QString::number(data2_statis.d20, 'f', 4)));
        ui->label_d50->setText(QString("D50 : (1) %1, (2) %2").arg(QString::number(data1_statis.d50, 'f', 4), QString::number(data2_statis.d50, 'f', 4)));
        ui->label_d70->setText(QString("D70 : (1) %1, (2) %2").arg(QString::number(data1_statis.d70, 'f', 4), QString::number(data2_statis.d70, 'f', 4)));
    }else
    {
        ui->label_avgsurface->setText(QString("Average Surface : %1").arg(QString::number(data1_statis.avg, 'f', 4)));
        ui->label_deviation->setText(QString("Standard Deviation : %1").arg(QString::number(data1_statis.sd, 'f', 4)));
        ui->label_mode->setText(QString("Mode Surface : %1").arg(QString::number(data1_statis.mode, 'f', 2)));
        ui->label_nums->setText(QString("Nums Coffee Particle : %1").arg(data1_statis.cont));
        ui->label_d20->setText(QString("D20 : %1").arg(QString::number(data1_statis.d20, 'f', 4)));
        ui->label_d50->setText(QString("D50 : %1").arg(QString::number(data1_statis.d50, 'f', 4)));
        ui->label_d70->setText(QString("D70 : %1").arg(QString::number(data1_statis.d70, 'f', 4)));
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
        dataBase->set_data1_area(tmp_area);
        dataBase->set_data1_diameter(tmp_diameter);
        surface_average += tmp_area;
        countout_idx ++;
    }
    surface_average = surface_average/countout_idx;
    data1_statis.avg = surface_average;
    data1_statis.cont = countout_idx;
    dataBase->sort_area();
    dataBase->sort_diameter();
    statistics(&data1_statis);
    if(ui->checkBox_outlier->isChecked()){
        statistics_without_outlier();
    }
    update_label();
    datafile.close();
    set_flag->flag_data1 = true;
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

    while(!in.atEnd())
    {
        QString line = in.readLine();
        if(firstline){
            firstline = false;
            continue;
        }

        QStringList values = line.split(",");
        float area = values[1].toFloat();
//        dataBase->set_contours_area(area);
        float tmp_diameter = values[2].toFloat();
        data2_area.push_back(area);
        data2_diameter.push_back(tmp_diameter);
        surface_average += area;
        countout_idx ++;
    }
    surface_average = surface_average/countout_idx;

    data2_statis.avg = surface_average;
    data2_statis.cont = countout_idx;
    std::sort(data2_diameter.begin(), data2_diameter.end());
    std::sort(data2_area.begin(), data2_area.end());
    statistics(&data2_statis);
    if(ui->checkBox_outlier->isChecked()){
        statistics_without_outlier();
    }
    set_flag->flag_data2 = true;
    update_label();
    datafile.close();

}




























