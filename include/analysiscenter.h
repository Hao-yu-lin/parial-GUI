#ifndef ANALYSIS_H
#define ANALYSIS_H
#include "imagecenter.h"
#include "calculate.h"
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
    explicit AnalysisCenter(Ui::MainWindow *input_ui, ImageCenter *input_imgCenter);
    ~AnalysisCenter(){
        ui = nullptr;
        imgCenter = nullptr;
        dataBase = nullptr;
    };
    void test();
    void set_pts_vector(const QPointF &img_pos);
    void del_pts_vector();

    /*  ----------- Reference Object -----------  */
    void reset_refer();
    void cal_refer_obj();

    /*  ----------- Particle -----------  */
    void chose_detect_obj();
    void reset_detect();
    void detect_particle();

    /*  ----------- calculate area -----------  */
    void cal_contours(std::vector<std::vector<cv::Point>> *contours);

    /*  ----------- Update Image -----------  */
    void draw_img(const std::vector<cv::Point2i>& vector_pts, cv::Scalar color);
    void draw_img();
    void update_pts_img(cv::Mat &imgsrc, const std::vector<cv::Point2i> &vector_pts, const cv::Scalar color);
    void draw_contours_img();

    void createBarChart();
    void reproducehist();
    void statistics();
    void statistics_without_outlier();
    void update_label();

protected:
    struct statis{
        float avg;
        float sd;
        float mode;
        float cont;
        float d20;
        float d50;
        float d70;
    };

private:
    Ui::MainWindow *ui;
    ImageCenter *imgCenter;
    DataBase *dataBase;
    cv::Mat roi_mask;
//    Bool_State bool_state;
    int countout_idx;
    std::map<float, int> counter;
    /*
     * avg、cont、d50
     */
    statis statistics_area;
//    std::map<std::string, float> statistics_area;
    statis statistics_wo_outliter;
};



#endif // ANALYSIS_CENTER_H
