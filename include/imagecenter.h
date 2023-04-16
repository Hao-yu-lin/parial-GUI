#ifndef IMAGECENTER_H
#define IMAGECENTER_H

#include "database.h"
#include "calculate.h"


//#include "cplus2py.h"
#include <QObject>
#include <QMainWindow>
#include <QFileDialog>
#include <QScrollBar>
#include <QPointF>
#include <QMouseEvent>
#include <QWidget>
#include <string>
#include <opencv2/core/types.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cmath>

#define flag_off 0
#define flag_refer_obj 1
#define flag_select_roi 2
#define flag_hist 3

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


int rate_to_value(const double &ratio);
double value_to_rate(const int &value);


class ImageCenter:public QObject
{
    Q_OBJECT
public:
    explicit ImageCenter(Ui::MainWindow *input_ui);
    ~ImageCenter(){
        dataBase->~DataBase();
        ui = nullptr;
        dataBase = nullptr;
        delete dataBase;

    };

    void open_img(const QString &fileName, bool &flag_open_image);
    // zoom & slider
    void slider_zoom(const int value);
    void zoom_in();
    void zoom_out();
    void rest_view();
    void set_img();
    void set_sroll_area();
    QPointF zoomevent(const double &new_rate);

    friend class AnalysisCenter;


     int flag_num = flag_off;

private:
    Ui::MainWindow *ui;
    DataBase *dataBase;
    QString fileName;
    QImage qimg_img;
    cv::Mat imgSrc;

    double label_image_width;
    double label_image_height;

    // for python
//    CallPy *m_callpy;
    QThread *m_child_thread;



};

#endif // IMAGECENTER_H
