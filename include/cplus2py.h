#ifndef CPLUS2PY_H
#define CPLUS2PY_H

#include "converter.h"
#include <opencv2/opencv.hpp> 
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <QThread>
#include <QObject>


class CallPy : public QObject{
    Q_OBJECT
public:
    explicit CallPy(QObject *parent = nullptr);
    void set_addr(std::string file_addr);

    ~CallPy(){
        std::cout << "cplus2py delete" << std::endl;
    };

signals:
    void finished();
    void errorOccurred();

public slots:
    cv::Mat start_python();

private:
    std::string addr;

};



#endif // CPLUS2PY_H
