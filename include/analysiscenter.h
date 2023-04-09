#ifndef ANALYSIS_H
#define ANALYSIS_H
#include "imagecenter.h"
#include "calculate.h"
#include <QMouseEvent>
#include <QPointF>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <math.h>
#include <vector>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


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
    void set_pts_vector(const QPointF &img_pos, const int &flag_num);
    void del_pts_vector(const int &flag_num);

    /*  ----------- Reference Object -----------  */
    void reset_refer();
    void cal_refer_obj();

    /*  ----------- Particle -----------  */
    void chose_detect_obj();
    void reset_detect();
    void detect_particle();


    /*  ----------- Update Image -----------  */
    void draw_img(const std::vector<cv::Point2i>& vector_pts, cv::Scalar color);
    void draw_img(const std::vector<cv::Point2i>& vector_pts);
    void update_pts_img(cv::Mat &imgsrc, const std::vector<cv::Point2i> &vector_pts, const cv::Scalar color);


private:
    Ui::MainWindow *ui;
    ImageCenter *imgCenter;
    DataBase *dataBase;
    cv::Mat roi_mask;
//    Bool_State bool_state;

};

#endif // ANALYSIS_CENTER_H
