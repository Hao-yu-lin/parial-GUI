#include "../include/mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->scrollArea->setWidgetResizable(false);
    ui->label_image->installEventFilter(this);
    imgCenter = new ImageCenter(ui, &set_flag);
    analysisCenter = new AnalysisCenter(ui, imgCenter, &set_flag);
    ui->label_image->setText("No Image Loaded");
}

MainWindow::~MainWindow()
{
    delete imgCenter;
    delete ui;
}

void MainWindow::on_btn_open_img_clicked()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Images (*.png *.jpg *.jpeg *JPG)"));
    if(set_flag.flag_image == true)
    {
        analysisCenter->~AnalysisCenter();
        set_flag.flag_image = false;
    }
    imgCenter->open_img(fileName);
    analysisCenter = new AnalysisCenter(ui, imgCenter, &set_flag);
}

void MainWindow::on_slider_zoom_sliderReleased()
{
    if(set_flag.flag_image || set_flag.flag_hist_img)
    {
        int value = ui->slider_zoom->value();
        imgCenter->slider_zoom(value);
    }else
    {
        std::cout <<"please open new image!" << std::endl;
    }
}

void MainWindow::on_btn_zoom_in_clicked()
{
     if(set_flag.flag_image || set_flag.flag_hist_img)
     {
         imgCenter->zoom_in();
     }else
     {
         std::cout <<"please open new image!" << std::endl;
     }
}

void MainWindow::on_btn_zoom_out_clicked()
{
    if(set_flag.flag_image || set_flag.flag_hist_img)
    {
        imgCenter->zoom_out();
    }else
    {
        std::cout <<"please open new image!" << std::endl;
    }
}

void MainWindow::on_btn_reset_view_clicked()
{
    if(set_flag.flag_image)
    {
        imgCenter->rest_view();
        set_flag.flag_num = num_off;
        set_flag.img_show = num_show_img;
    }else
    {
        std::cout <<"please open new image!" << std::endl;
    }
}

void MainWindow::on_btn_shadow_removal_clicked()
{
    if(set_flag.flag_image)
    {
        imgCenter->shadow_removal();
    }else
    {
        std::cout <<"please open new image!" << std::endl;
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->label_image)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if(set_flag.flag_num == num_refer_obj)
            {
                set_flag.img_show = num_show_contours;
                if(mouseEvent->button() == Qt::LeftButton)
                {
                    QPointF img_pos = mouseEvent->pos();
                    analysisCenter->set_pts_vector(img_pos);
                    return true;
                }else if(mouseEvent->button() == Qt::RightButton)
                {
                    analysisCenter->del_pts_vector();
                    return true;
                }else
                {
                    return false;
                }
            }else if(set_flag.flag_num == num_select_roi){
                set_flag.img_show = num_show_contours;
                if(mouseEvent->button() == Qt::LeftButton)
                {
                    QPointF img_pos = mouseEvent->pos();
                    analysisCenter->set_pts_vector(img_pos);
                    return true;
                }else if(mouseEvent->button() == Qt::RightButton)
                {
                    analysisCenter->del_pts_vector();
                    return true;
                }else
                {
                    return false;
                }

            }else if(set_flag.flag_num == num_del_roi){
                if(mouseEvent->button() == Qt::LeftButton)
                {
                    QPointF img_pos = mouseEvent->pos();
                    analysisCenter->set_pts_vector(img_pos);
                    return true;
                }
            }else
            {
                return false;
            }
        }else
        {
            return false;
        }
    }else
    {
        return QWidget::eventFilter(obj, event);
    }
    return QWidget::eventFilter(obj, event);
}

void MainWindow::on_btn_refer_obj_clicked()
{
    if(set_flag.flag_image)
    {
        if(set_flag.flag_num != num_refer_obj){
            set_flag.flag_num = num_refer_obj;
            set_flag.img_show = num_show_contours;
        }else
        {
            set_flag.flag_num = num_off;
        }
    }else
    {
        std::cout <<"please open new image!" << std::endl;
    }
}

void MainWindow::on_btn_refer_obj_calculate_clicked()
{
    set_flag.flag_num = num_off;
    analysisCenter->cal_refer_obj();
}

void MainWindow::on_btn_refer_obj_rest_clicked()
{
   set_flag.flag_num = num_off;
   analysisCenter->reset_refer();
   ui->lineEdit_pixel_scale_value->clear();
   ui->lineEdit_object_length->clear();
}

void MainWindow::on_btn_roi_select_clicked()
{
    if(set_flag.flag_image)
    {
        if(set_flag.flag_num != num_select_roi)
        {
            set_flag.flag_num = num_select_roi;
        }else
        {
            set_flag.flag_num = num_off;
        }
    }else
    {
        std::cout <<"please open new image!" << std::endl;
    }
}

void MainWindow::on_btn_roi_choose_clicked()
{
    set_flag.flag_num = num_off;
    analysisCenter->chose_detect_obj();
}

void MainWindow::on_btn_particle_reset_clicked()
{
    set_flag.flag_num = num_off;
    analysisCenter->reset_detect();
}

void MainWindow::on_btn_find_contours_clicked()
{
     set_flag.flag_num = num_off;
     analysisCenter->find_contours();
}

void MainWindow::on_btn_detect_particle_clicked()
{
    set_flag.flag_num = num_off;
    analysisCenter->detect_particle();
}

void MainWindow::on_btn_erase_clusters_clicked()
{
    set_flag.flag_num = num_del_roi;
}


void MainWindow::on_btn_draw_hist_clicked()
{
    set_flag.flag_num = num_hist;
    int state_index = ui->comboBox_histstate->currentIndex(); // 0: surface, 1:diameter

    if(set_flag.flag_data1 == false)
    {
        std::cout << "please, detect data1" << std::endl;
        return;
    }
//    if(last_hist_state == state_data1_area || last_hist_state  == state_data1_diameter)
//    {
//        if(ui->lineEdit_nums_bins->isModified())
//        {
//            analysisCenter->reproducehist1();

//        }else if(state_index == 0 && last_hist_state == state_data1_area)
//        {
//             analysisCenter->producehist1();
//        }else
//        {
//            analysisCenter->reproducehist1();
//        }
//    }else{
//        analysisCenter->reproducehist1();
//    }
//    if(state_index == 0)
//    {
//        last_hist_state = state_data1_area;
//    }else
//    {
//        last_hist_state = state_data1_diameter;
//    }


     analysisCenter->reproducehist1();

}

void MainWindow::on_btn_draw_hist2_clicked()
{
    set_flag.flag_num = num_hist;
    int state_index = ui->comboBox_histstate->currentIndex(); // 0: surface, 1:diameter

    if(set_flag.flag_data2 == false)
    {
        std::cout << "please, detect data2" << std::endl;
        return;
    }
    if(last_hist_state == state_data2_area || last_hist_state  == state_data2_diameter)
    {
        if(ui->lineEdit_nums_bins->isModified())
        {
            analysisCenter->reproducehist2();

        }else if(state_index == 0 && last_hist_state == state_data2_area)
        {
             analysisCenter->producehist2();
        }else
        {
            analysisCenter->reproducehist2();
        }
    }else
    {
        analysisCenter->reproducehist2();
    }
    if(state_index == 0)
    {
        last_hist_state = state_data2_area;
    }else
    {
        last_hist_state = state_data2_diameter;
    }


}

void MainWindow::on_btn_show_image_clicked()
{
    if(set_flag.flag_image == false) return;
    set_flag.flag_num = num_off;
    set_flag.img_show = num_show_img;
    imgCenter->set_img();

}

void MainWindow::on_btn_show_contours_img_clicked()
{
    if(set_flag.flag_image == false) return;
    set_flag.flag_num = num_off;
    set_flag.img_show = num_show_contours;
    imgCenter->set_img();
}


void MainWindow::on_btn_save_contours_clicked()
{
    QString write_name = QFileDialog::getSaveFileName(this, tr("Save File"), nullptr, QStringLiteral("Text files (*.txt)"));
    imgCenter->save_data(write_name);
}

void MainWindow::on_btn_load_contours_clicked()
{
    QString dataName = QFileDialog::getOpenFileName(this, tr("Open Area Data"), "", tr("Text files (*.txt)"));
    analysisCenter->load_data1(dataName);
}

void MainWindow::on_btn_load_contours2_clicked()
{
    QString dataName2 = QFileDialog::getOpenFileName(this, tr("Open Area Data"), "", tr("Text files (*.txt)"));
    analysisCenter->load_data2(dataName2);
}

void MainWindow::on_btn_save_image_clicked()
{
    QString write_name = QFileDialog::getSaveFileName(this, tr("Save File"), nullptr, QStringLiteral("Text files (*.png)"));
    imgCenter->save_img(write_name);
}



