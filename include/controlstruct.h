#ifndef CONTROLLER_FLAG_H
#define CONTROLLER_FLAG_H

#define num_off 0
#define num_refer_obj 1
#define num_select_roi 2
#define num_hist 3
#define num_del_roi 4

typedef struct setting_flag{
    bool flag_image = false;
    bool flag_hist_img = false;
    bool flag_contours = false;
    bool flag_data1 = false;
    bool flag_data2 = false;
    int flag_num = num_off;

}setting_t;

typedef struct particle{
    int idx;
    float surface;
    float diameter;
}particle_t;

typedef struct statis{
    float avg;
    float sd;
    float mode;
    float cont;
    float d20;
    float d50;
    float d70;
}statis_t;


#endif // CONTROLLER_FLAG_H
