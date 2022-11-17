#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File    :   test_client2.py
@Time    :   2022/11/16 14:37:53
@Author  :   snowman
@Version :   2.0
@License :   
@Desc    :   None
'''

import socket
import base64
from agreement_class import Massage

#基本信息
IDas = "IDas"
IDLead = "IDLead"
k="this_is_key"


ip_port = ('127.0.0.1', 9999)
sk = socket.socket()
sk.connect(ip_port)
sk.settimeout(5)


#第一次握手发送消息
massage = Massage(IDas,IDLead,K=k)
massage1 = massage.get_list()
print(massage1)
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


print("链接成功")
while True:
    inp = input('你:').strip()
    if not inp:
        continue

    sk.sendall(inp.encode())

    if inp == 'exit':
        print("谢谢使用，再见！")
        break
    data = sk.recv(1024).decode()
    print('服务器:', data)
sk.close()

'''sk.settimeout(5)
data = sk.recv(1024).decode()
print('服务器:', data)
while True:
    inp = input('你:').strip()
    if not inp:
        continue

    sk.sendall(inp.encode())

    if inp == 'exit':
        print("谢谢使用，再见！")
        break
    data = sk.recv(1024).decode()
    print('服务器:', data)
sk.close()'''