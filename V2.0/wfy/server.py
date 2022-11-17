#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File    :   server.py
@Time    :   2022/11/08 20:38:29
@Author  :   snowman
@Version :   2.0
@Contact :   
@License :   
@Desc    :   None
'''

import base64
import socketserver
from agreement_class import Massage

#Global Variables
IDLead = "IDLead"
k="this_is_key"
rec_ID={}#格式为ID：秘钥



class MyServer(socketserver.BaseRequestHandler):
    """
    必须继承socketserver.BaseRequestHandler类
    """
    def handle(self):
        conn = self.request         # request里封装了所有请求的数据
        #conn.sendall('欢迎访问socketserver服务器！'.encode())
        while True:
            if not self.agreement_test(conn):
                exit()
            #后续需要加密
            data = conn.recv(1024).decode()
            if data == "exit":
                print("断开与%s的连接！" % (self.client_address,))
                break
            print("来自%s的客户端向你发来信息：%s" % (self.client_address, data))
            conn.sendall(('已收到你的消息<%s>' % data).encode())
    
    def agreement_test(self,conn):
        #第一次握手(接收)
        massage = Massage(IDLead,K=k)
        data1 = conn.recv(1024)
        temp = base64.b64decode(data1).decode('utf-8')
        a = massage.ju_massage(temp)
        if a is False :
            exit()
        #第二次握手(发送)
        massage2 = massage.get_list()
        conn.send(base64.b64encode(str(massage2).encode('utf-8')))
        #第三次握手(接收)
        data3 = conn.recv(1024)
        temp = base64.b64decode(data3).decode('utf-8')
        a = massage.ju_massage(temp)
        if a is False :
            exit()
        #发送确认信息，并生成秘钥，并把该ID和对应秘钥放到字典里
        ID , key = massage.new_key()
        for i,j in rec_ID:
            if str(i) == ID:
                print("已有此ID和通信秘钥")
        rec_ID[ID] = key
        print(rec_ID)
        return 1






if __name__ == '__main__':
    # 创建一个多线程TCP服务器
    server = socketserver.ThreadingTCPServer(('127.0.0.1', 9999), MyServer)
    print("启动socketserver服务器！")
    # 启动服务器，服务器将一直保持运行状态
    server.serve_forever()


