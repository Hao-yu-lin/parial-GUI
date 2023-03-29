#define PY_SSIZE_T_CLEAN

#include "../include/cplus2py.h"


CallPy::CallPy(QObject *parent) : QObject(parent){

}


cv::Mat CallPy::start_python() {
    std::cout << "init python" << std::endl;
    // 初始化 python 接口
    Py_Initialize();
    if (!Py_IsInitialized()) {
        std::cout << "python init fail" << std::endl;
        std::exit(-1);
    }else{
        std::cout <<"python init sucess" << std::endl;
    }

    // 初始化使用的變量
    PyObject* pModule = NULL;
    PyObject* pFunc = NULL;
    PyObject* pName = NULL;

    // 初始化 python 系統文件路徑，保證路徑可以訪問到 .py 文件
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('../parial-GUI/src/py_file')");
    

    // 調用 python 文件名，不用寫後綴

    pModule = PyImport_ImportModule("train");
    if ( pModule == NULL ) {
        std::cout << "module not found" << std::endl;
        std::exit(-1);
    }
    // 調用函數
    pFunc = PyObject_GetAttrString(pModule, "main");
    if ( !pFunc || !PyCallable_Check(pFunc) ) {
        std::cout << "not found function main" << std::endl;
        std::exit(-1);
    }else{
        std::cout <<"found function main" << std::endl;
    }

    if(addr.empty()){
        std::cout << "address can't found" << std::endl;
        std::exit(-1);
    }else{
        std::cout <<"address found" << std::endl;
    }
    std::cout << "start python" << std::endl;
    // // 給 python 傳參數
    PyObject* pArgs = PyTuple_New(1);   
    // // 0:第一個參數，傳入 string 類型，傳遞圖片位址
    PyTuple_SetItem(pArgs, 0, Py_BuildValue("s", addr.c_str()));

    // // 0:第一個參數，傳入 int 類型的值 2
    // PyTuple_SetItem(pArgs, 1, Py_BuildValue("i", 4));

    PyObject* pReturn = PyObject_CallObject(pFunc, pArgs);
    std::cout << "Python Finsih" << std::endl;
    // // 接收 python 計算好的返回值
//    PyObject nResult;
//    PyArg_Parse(pReturn, "i", &nResult);
//        std::cout << "return result is " << nResult << std::endl;
    std::cout << "Start change np to mat" << std::endl;
    cv::Mat new_image = pbcvt::fromNDArrayToMat(pReturn);
    std::cout << "np to mat finish" << std::endl;


    PyObject_CallObject(pFunc, NULL);
    // 結束 python 接口初始化
    Py_Finalize();

    return new_image;

}

void CallPy::set_addr(std::string file_addr){
    addr = file_addr;
}
