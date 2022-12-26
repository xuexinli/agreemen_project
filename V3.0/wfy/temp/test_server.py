#! python
# -*- encoding: utf-8 -*-
'''
@File    :   test_server.py
@Time    :   2022/12/02 14:40:23
@Author  :   snowman
@Version :   1.0
@License :   
@Desc    :   None
'''

import socketserver
import threading
from time import sleep

all = {}

class MyServer(socketserver.BaseRequestHandler):
    """
    必须继承socketserver.BaseRequestHandler类
    """
    def handle(self):
        conn = self.request         # request里封装了所有请求的数据
        #conn.sendall('欢迎访问socketserver服务器！'.encode())
        
        while True:
            #后续需要加密
            data = conn.recv(1024).decode()
            if data == "exit":
                print("断开与%s的连接！" % (self.client_address,))
                break
            all["data"] = conn
            print("来自%s的客户端向你发来信息：%s" % (self.client_address, data))
            conn.sendall(data.encode())
            print(all)




def jujce_threat():
    print("这里是主线程")
    sleep(30)

def server():
    # 创建一个多线程TCP服务器
    server = socketserver.ThreadingTCPServer(('127.0.0.1', 1234), MyServer)
    print("启动socketserver服务器！")
    # 启动服务器，服务器将一直保持运行状态
    server.serve_forever()






if __name__ == '__main__':
    num =1
    main_threat = threading.Thread(target=jujce_threat)
    server_threat = threading.Thread(target=server)
    main_threat.start()
    server_threat.start()
    
    