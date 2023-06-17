#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "imagecenter.h"
#include "database.h"
#include "calculate.h"
#include "analysiscenter.h"
#include "controlstruct.h"

#include <QMainWindow>
#include <QFileDialog>
#include <QScrollBar>
#include <QPointF>
#include <iostream>
#include <QMouseEvent>
#include <QObject>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define surface_percent 0
#define surface_number 1
#define diameter_percent 2
#define diameter_number 3
#define state_none  4

#define hist_data1 1
#define hist_data2 2
#define hist_data_mix 3
//#define state_datamix_area 5
//#define state_datamix_diameter 6



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void on_btn_open_img_clicked();

    void on_btn_zoom_in_clicked();

    void on_btn_zoom_out_clicked();

    void on_btn_reset_view_clicked();

    void on_btn_shadow_removal_clicked();

    void on_slider_zoom_sliderReleased();

    void on_btn_refer_obj_clicked();

    void on_btn_refer_obj_calculate_clicked();

    void on_btn_refer_obj_rest_clicked();

    void on_btn_roi_select_clicked();

    void on_btn_roi_choose_clicked();

    void on_btn_particle_reset_clicked();

    void on_btn_find_contours_clicked();

    void on_btn_detect_particle_clicked();

    void on_btn_draw_hist_clicked();

    void on_btn_draw_hist2_clicked();

    void on_btn_show_image_clicked();

    void on_btn_erase_clusters_clicked();

    void on_btn_save_contours_clicked();

    void on_btn_load_data1_clicked();

    void on_btn_load_data2_clicked();

    void on_btn_show_contours_img_clicked();

    void on_btn_save_image_clicked();

    void on_btn_draw_comparehist_clicked();

private:
    Ui::MainWindow *ui;
    ImageCenter *imgCenter;
    QString fileName;
    AnalysisCenter *analysisCenter;
    setting_t set_flag;
    int last_hist_state = state_none;
    int last_data = state_none;
};

#endif // MAINWINDOW_H
