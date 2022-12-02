#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File    :   client.py
@Time    :   2022/11/08 20:38:10
@Author  :   snowman
@Version :   2.0
@Contact :   
@License :   
@Desc    :   None
'''

import socket
import base64
import socketserver
from agreement_class import Massage
from decodeandencode import SM4
import threading

#基本信息
IDas = "IDas"
IDLead = "IDLead"
k="this_is_key"
connet_key = ""
SM4 = SM4()


'''while True:
    inp = input('你:')#.strip()
    if not inp:
        continue
    print(inp)
    inp_send = SM4.encrypt(connet_key, inp)
    sk.sendall(inp_send.encode())

    if inp == 'exit':
        print("谢谢使用，再见！")
        break
    raw_data = sk.recv(1024).decode()
    print("raw_data:",raw_data)
    data = SM4.decrypt(connet_key,raw_data)
    print('服务器:', data)
sk.close()'''

class MyServer(socketserver.BaseRequestHandler):
    """
    必须继承socketserver.BaseRequestHandler类
    """
    def handle(self):
        conn = self.request         # request里封装了所有请求的数据
        #把所有的请求封装到all字典里面



        #conn.sendall('欢迎访问socketserver服务器！'.encode())
        a,ID = self.agreement_test(conn)
        if not a:
            exit()
        print("完成秘钥协商：秘钥为:",connet_key)    
        while True:
            #后续需要加密
            raw_data = conn.recv(1024).decode()
            data =SM4.decrypt(connet_key,raw_data)
            if data == "exit":
                print("断开与%s的连接！" % (self.client_address,))
                break
            print("来自%s的客户端向你发来信息：%s" % (self.client_address, data))
            en_data = SM4.encrypt(connet_key,data)
            conn.sendall(en_data.encode())



def server_threat():   
   
    #启动多线程服务器
    server = socketserver.ThreadingTCPServer(ip, MyServer)
    print("启动Leader服务器！")
    # 启动服务器，服务器将一直保持运行状态
    server.serve_forever()

#继承与先前链接的对象
def main_threat(sk:socket):
    pass


if __name__ == '__main__':
    ip_port = ('127.0.0.1', 9999)
    sk = socket.socket()
    sk.connect(ip_port)
    sk.settimeout(5)


    #第一次握手发送消息
    massage = Massage(IDLead,IDas,K=k)
    massage1 = massage.get_list()
    sk.send(base64.b64encode(str(massage1).encode('utf-8')))
    #第二次接收
    data2 = sk.recv(1024)
    temp = base64.b64decode(data2).decode('utf-8')
    a = massage.ju_massage(temp)
    if a is False :
        exit()
    #第三次发送
    massage3 = massage.get_list()
    sk.send(base64.b64encode(str(massage3).encode('utf-8')))
    #收到确认信息
    data_OK = sk.recv(1024)
    temp = base64.b64decode(data_OK).decode('utf-8')
    if temp == "OK":
        ID,connet_key = massage.new_key()
        print(connet_key)
    #sk为 与AS的链接（主）
    print("链接成功")

    #启动多线程服务器之前先广播出自己的ID和IP端口
    ip = sk.getsockname()
    s_temp =socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s_temp.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

    PORT_broadcast = 6666
    network = '<broadcast>'
    s_temp.sendto(str(ip).encode('utf-8'), (network,PORT_broadcast))
    s_temp.close()


    #接下来与AS的链接放到主线程里运行
    threat_server = threading.Thread(target=server_threat)
    #逻辑处理线程
    threat_main = threading.Thread(target=main_threat,args=sk)
    threat_main.start()
    threat_server.start()
    







