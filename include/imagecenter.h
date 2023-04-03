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
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <string>

class MainWindow;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ImageCenter:public QObject
{
    Q_OBJECT
public:
    explicit ImageCenter(Ui::MainWindow *input_ui);
    ~ImageCenter(){
        imgdata->~DataBase();
        ui = nullptr;
        imgdata = nullptr;
        delete imgdata;

    };

    void open_img(const QString &fileName, bool &flag_open_image);
    // zoom & slider
    void slider_zoom(const int value);
    void zoom_in();
    void zoom_out();
    void rest_view();
    void set_img();

    QPointF zoomevent(const double &new_rate);


private:
    Ui::MainWindow *ui;
    DataBase *imgdata;
    QString fileName;
    QImage qimg_img;
    QImage orig_qimg;
    cv::Mat imgSrc;

    double label_image_width;
    double label_image_height;

    // for python
//    CallPy *m_callpy;
    QThread *m_child_thread;



};

#endif // IMAGECENTER_H
