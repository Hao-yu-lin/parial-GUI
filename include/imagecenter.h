#ifndef IMAGECENTER_H
#define IMAGECENTER_H
//#include "cplus2py.h"
#include "database.h"
#include "calculate.h"
#include "controlstruct.h"

#include <QObject>
#include <QMainWindow>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
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



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


int rate_to_value(const double &ratio);
double value_to_rate(const int &value);


class ImageCenter:public QObject
{
    Q_OBJECT
public:
    explicit ImageCenter(Ui::MainWindow *input_ui, setting_t *input_flag);
    ~ImageCenter(){
        dataBase->~DataBase();
        ui = nullptr;
        dataBase = nullptr;
        delete dataBase;
    };

    void open_img(const QString &fileName);
    // zoom & slider
    void slider_zoom(const int value);
    void zoom_in();
    void zoom_out();
    void rest_view();

    // set img
    void set_img();
    void set_hist_img();
    void set_sroll_area();

    // img preprocessing
    void shadow_removal();
    void white_balance();

    // save
    void save_data(const QString &fileName);
    void save_img(const QString &fileName);

    QPointF zoomevent(const double &new_rate);
    friend class AnalysisCenter;

private:
    Ui::MainWindow *ui;
    DataBase *dataBase;
    setting_t *set_flag;
    QImage qimg_img;
    QString imgaddr;
    cv::Mat imgSrc;

    double label_image_width;
    double label_image_height;

    // for python

//    CallPy *m_callpy;
    QThread *m_child_thread;
    std::string img_path;

};

#endif // IMAGECENTER_H
