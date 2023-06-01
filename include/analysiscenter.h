#ifndef ANALYSIS_H
#define ANALYSIS_H
#include "imagecenter.h"
#include "calculate.h"
#include "controlstruct.h"

#include <QMouseEvent>
#include <QPointF>
#include <QtCharts>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QSplineSeries>
#include <QPainter>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <math.h>
#include <cfloat>
#include <cmath>
#include <vector>
#include <map>
#include <fstream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE



class AnalysisCenter:public QObject
{
    Q_OBJECT
public:
    explicit AnalysisCenter(Ui::MainWindow *input_ui, ImageCenter *input_imgCenter, setting_t *input_flag);
    ~AnalysisCenter(){
        ui = nullptr;
        imgCenter = nullptr;
        dataBase = nullptr;
        set_flag = nullptr;
    };
    void set_pts_vector(const QPointF &img_pos);
    void del_pts_vector();

    /*  ----------- Reference Object -----------  */
    void reset_refer();
    void cal_refer_obj();

    /*  ----------- Particle -----------  */
    void chose_detect_obj();
    void reset_detect();
    void detect_particle();
    void white_balance();
    void find_contours();

    /*  ----------- calculate area -----------  */
    void cal_contours(const std::vector<std::vector<cv::Point>> *contours);

    /*  ----------- Update Image -----------  */
    void draw_img(const std::vector<cv::Point2i>& vector_pts, cv::Scalar color);
    void draw_img();
    void update_pts_img(cv::Mat &imgsrc, const std::vector<cv::Point2i> &vector_pts, const cv::Scalar color);
    void draw_contours_img();

    void createbar_area(const statis_t& data);
    void createbar_diameter(const statis_t& data);

    void producehist1();
    void producehist2();
    void reproducehist1();
    void reproducehist2();

    void statistics(const statis_t& data);
//    void statistics_without_outlier();
    void update_label();
    void load_data1(const QString& fileName);
    void load_data2(const QString& fileName);



private:
    Ui::MainWindow *ui;
    ImageCenter *imgCenter;
    setting_t *set_flag;
    DataBase *dataBase;
    cv::Mat roi_mask;
    cv::Mat binary_map;
};



#endif // ANALYSIS_CENTER_H
