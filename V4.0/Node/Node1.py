#python
# -*- encoding: utf-8 -*-
'''
@File    :   Node1.py
@Time    :   2022/12/02 12:28:54
@Author  :   snowman
@Version :   4.0
@License :   
@Desc    :   None
'''


import socket
from agreement_Node import *
from time import sleep
import threading
#密钥
rec_ID={}#格式为ID：raw_key
#基本信息
IDas = "IDas"
IDLead2 = "IDLead"
IDNode = "Node1"
IDNode2 = "Node2"
IDNode3 = "Node3"

#共享秘钥
K = {"Node_Leader":"Node_Leader","AS_Node1":"AS_Node1","AS_Node2":"AS_Node2","AS_Node3":"AS_Node3"}


'''#接收Leader广播的地址信息
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
#设置6666为广播地址
PORT = 6666
s.bind(('', PORT))
print('Listening for broadcast at ', s.getsockname())'''


class Node:
    def __init__(self) :
        self.key = ""
        ip_port = ('127.0.0.1', 8888)
        sk = socket.socket()
        sk.connect(ip_port)
        sk.settimeout(1000)
        self.request = sk
        self.thread = threading.Thread(target=self.recieveData)
        self.thread.start()
        self.massage = Message_Node_AS(IDNode,sk,Send=IDLead2,K=K["AS_Node1"])
        self.massage.main_Node()
        while True:
            sleep(1)
            if self.key!= "":
                key = self.key
                rec_ID[IDas] =  [key[:32],key[32:]]
                break
        print("finish")
        #测试Lead_与Node的协议
        massage = Massage_Node_Leader(IDNode,self.request,Other=IDLead2,K=K["Node_Leader"])
        a1,a2 = massage.massage_Lead()
        #[CK,IK]
        rec_ID[a1] = [a2[:32],a2[32:]]
        print("协商秘钥为：",rec_ID)

    def recieveData(self):
        while True:
            conn = self.request
            conn.settimeout(99999)
            raw_data = conn.recv(1024).decode()
            if raw_data == "":
                continue
            data,agreement = basic_handle(raw_data)
            #协议处理
            if agreement == "AS_Node":
                #与Node的链接
                self.key = self.massage.Node_AS(data)
            elif agreement == "MASSAGE_PRINT":
                print("IDLead向你发来明文消息",data)
            elif agreement == "MASSAGE_DECODE":
                print("这是加密消息")
            elif data =="":
                print("协议有误")
            else:
                print("消息中的协议有误，错误消息为",data)




if __name__ == '__main__':
    Nodee = Node() 

