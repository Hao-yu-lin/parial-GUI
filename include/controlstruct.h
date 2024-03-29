#ifndef CONTROLLER_FLAG_H
#define CONTROLLER_FLAG_H

#include <vector>

#define num_off 0
#define num_refer_obj 1
#define num_select_roi 2
#define num_hist 3
#define num_del_roi 4
#define num_show_img 5
#define num_show_contours 6


typedef struct setting_flag{
    bool flag_image = false;
    bool flag_hist_img = false;
    bool flag_contours = false;
    bool flag_data1 = false;
    bool flag_data2 = false;

    int flag_num = num_off;
    int img_show = num_show_img;
}setting_t;

typedef struct particle{
    int idx;
    float surface;
    float diameter;
}particle_t;

typedef struct statis{
    int cont;
    float avg;
    float sd;
    float mode;
    float d20;
    float d50;
    float d75;
    float d80;
    float d85;
    std::vector<float> value;
}statis_t;


#endif // CONTROLLER_FLAG_H
