#define PY_SSIZE_T_CLEAN
#include <python3.10/Python.h>
#include "../include/cplus2py.h"
#include <iostream>

void call_python() {
    // 初始化 python 接口
    Py_Initialize();
    if (!Py_IsInitialized()) {
        std::cout << "python init fail" << std::endl;
        std::exit(-1);
    }

    // 初始化使用的變量
    PyObject* pModule = NULL;
    PyObject* pFunc = NULL;
    PyObject* pName = NULL;
    std::cout << "ok1" << std::endl;
    // 初始化 python 系統文件路徑，保證路徑可以訪問到 .py 文件
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('../parial-GUI/src/py_file')");
    PyRun_SimpleString("print()");

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
    }

    // // 給 python 傳參數
    // PyObject* pArgs = PyTuple_New(2);   
    // // 0:第一個參數，傳入 int 類型的值 2
    // PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", 2));

    // // 0:第一個參數，傳入 int 類型的值 2
    // PyTuple_SetItem(pArgs, 1, Py_BuildValue("i", 4));

    // PyObject* pReturn = PyEval_CallObject(pFunc, pArgs);

    // // 接收 python 計算好的返回值
    // int nResult;
    // PyArg_Parse(pReturn, "i", &nResult);
    //     std::cout << "return result is " << nResult << std::endl;

    PyObject_CallObject(pFunc, NULL);
    // 結束 python 接口初始化
    Py_Finalize();
}