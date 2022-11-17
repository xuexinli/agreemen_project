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

# here put the import lib

import socket
import base64
from agreement_class import Massage


IDLead = "IDLead"
k="this_is_key"
rec_ID={}#格式为ID：秘钥



def recv_massage():
    data1 = c.recv(1024)
    dataStr = base64.b64decode(data1).decode('utf-8')  #先解码
    massage.ju_massage(dataStr)  #对MD5判断 判断的同时  更新MAC存储随机数

def send_massage():
    massage2 = massage.get_list()
    temp =  base64.b64encode(str(massage2).encode('utf-8'))
    





s = socket.socket() #创建socket对象
host = socket.gethostname() #获取本地主机名
port = 12345 #设置端口
s.bind((host,port)) #绑定端口

s.listen(5) #等待客户端链接
c,addr = s.accept() #建立客户端链接

print("链接地址"+str(addr))


massage = Massage(IDLead,K=k)

#第一次握手
recv_massage()
print("第一次握手信息接收成功")

#第二次握手
send_massage()
print("第二次握手信息发送成功")

#第三次握手
recv_massage()
print("第三次握手成功")
print(massage.massage_con,massage.r1,massage.r2,massage.r3)


'''
massage1.append(massage.MD5())
temp = base64.b64encode(str(massage1).encode('utf-8'))
s.send(temp)
print("client发送成功")
'''

'''
while True:
    client_data = c.recv(1024).decode()
    if client_data =="exit":
        exit("通讯结束")
    print("来自%s客户端向你发来信息：%s" %(addr,client_data))
    c.send("服务器已接收到你的信息".encode())

'''

