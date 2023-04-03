#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "database.h"
#include "calculate.h"
#include "imagecenter.h"

//#include "cplus2py.h"

#include <QMainWindow>
#include <QFileDialog>
#include <QScrollBar>
#include <QPointF>
#include <iostream>

class ImageCenter;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool flag_open_img = false;

private slots:
    void on_btn_open_img_clicked();

//    void on_slider_zoom_valueChanged(int value);

    void on_btn_zoom_in_clicked();

    void on_btn_zoom_out_clicked();

    void on_btn_reset_view_clicked();

    void on_btn_shadow_removal_clicked();

    void on_slider_zoom_sliderReleased();



private:
    Ui::MainWindow *ui;
    ImageCenter *imgCenter;
    QString fileName;

    // flag



    // for python
//    CallPy *m_callpy;
    QThread *m_child_thread;

};

#endif // MAINWINDOW_H
