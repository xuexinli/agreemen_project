#!python
# -*- encoding: utf-8 -*-
'''
@File    :   AS.py
@Time    :   2022/11/08 20:38:29
@Author  :   snowman
@Version :   4.0
@Contact :   
@License :   
@Desc    :   None
'''

import socketserver
from agrement_AS import *


K = {"Leader_AS":"Leader_AS","AS_Node1":"AS_Node1","AS_Node2":"AS_Node2","AS_Node3":"AS_Node3"}

#Global Variables
IDLead = "IDLead"
IDas = "IDas"
rec_ID={}#格式为ID：[IK:CK]


#协议格式
#XXXXXX+数据
#AS_Leader+数据
#AS_Node+数据W


def AS_Lead(conn):
    a,temp = AS_Lead_First(IDas,conn,K)
    if not a:
        print("协商有误")
    rec_ID[IDLead] = temp
    print("完成秘钥协商：秘钥为:",rec_ID[IDLead]) 


class MyServer(socketserver.BaseRequestHandler):
    """
    必须继承socketserver.BaseRequestHandler类
    """
    def handle(self):
        conn = self.request         # request里封装了所有请求的数据
        while True:
            raw_data = conn.recv(1024).decode()
            if raw_data == "":
                continue
            data,agreement = basic_handle(raw_data)
            if agreement == "AS_Lead":
                AS_Lead(conn)
            elif agreement == "AS_Node":
                #与Node的链接
                temp = AS_Node_connect(IDas,IDLead,conn,rec_ID[IDLead],K,data)
                for i in temp:
                    rec_ID[i] = temp[i]
            elif agreement == "MASSAGE":
                print("IDLead向你发来明文消息",data)
            elif agreement == "MASSAGE_DECODE":
                print("这是加密消息")
            else:
                print("消息中的协议有误，错误消息为",data)

        
        


if __name__ == '__main__':
    # 创建一个多线程TCP服务器
    server = socketserver.ThreadingTCPServer(('127.0.0.1', 9999), MyServer)
    print("启动socketserver服务器！")
    # 启动服务器，服务器将一直保持运行状态
    server.serve_forever()


