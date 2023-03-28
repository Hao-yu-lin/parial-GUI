#ifndef CPLUS2PY_H
#define CPLUS2PY_H
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp> 
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "converter.h"

void call_python(std::string addr);

#endif // CPLUS2PY_H
