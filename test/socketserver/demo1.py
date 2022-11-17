import os
import multiprocessing

def foo(i):
    print("这里是",multiprocessing.current_process().name)
    print("模块名称：",__name__)
    print("父进程id:",os.getpid()) #获取父进程id