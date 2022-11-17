#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File    :   client.py
@Time    :   2022/11/08 20:38:10
@Author  :   snowman
@Version :   1.0
@Contact :   
@License :   
@Desc    :   None
'''

import socket
import base64

from agreement_class import Massage






def recv_massage():
    data1 = s.recv(1024)
    #对信息进行编码转换并进行base64加密
    dataStr = base64.b64decode(data1).decode('utf-8')  #先解码
    massage.ju_massage(dataStr)  #对MD5判断 判断的同时  更新MAC存储随机数

def send_massage():
    massage2 = massage.get_list()
    temp = temp = base64.b64encode(str(massage2).encode('utf-8'))
    s.send(temp)


IDas = "IDas"
IDLead = "IDLead"
k="this_is_key"



s=socket.socket()
host = socket.gethostname()
port = 12345
s.connect((host,port))

#t = threading.Thread(target=getInfo)
#t.start()



massage = Massage(IDas,IDLead,K=k)

#第一次握手
send_massage()

#第二次握手
recv_massage()

#第三次握手
send_massage()
print(massage.massage_con,massage.r1,massage.r2,massage.r3)

s.close()


'''while True:
    inp = input("请输入要发送的信息").strip()
    if not inp:
        continue
    
    s.sendall(inp.encode())
    if inp==exit:
        print("结束通信")
        break
    server_reply = s.recv(1024).decode()
    print(server_reply)

s.close()
'''




