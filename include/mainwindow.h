#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QScrollBar>
#include <QPointF>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>

#include "database.h"
#include "calculate.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_open_img_clicked();

    void on_slider_zoom_valueChanged(int value);

    void on_btn_zoom_in_clicked();

    void on_btn_zoom_out_clicked();

    void on_btn_reset_view_clicked();

private:
    Ui::MainWindow *ui;
    QString fileName;
    QImage qimg_img;
    QImage orig_qimg;
    cv::Mat imgSrc;
    Database *imgdata;

    void set_img();
    QPointF zoomevent(double new_rate);

};

#endif // MAINWINDOW_H
