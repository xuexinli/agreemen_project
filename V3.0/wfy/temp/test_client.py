#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File    :   test_client.py
@Time    :   2022/12/02 14:44:11
@Author  :   snowman
@Version :   1.0
@License :   
@Desc    :   None
'''

import socket
from time import sleep
'''
class Test():
    def __init__(self,sk:socket):
        self.sk = sk

    def send(self):
        self.sk.send("这是一个测试".encode('utf-8'))


ip_port = ('127.0.0.1', 9999)
sk = socket.socket()
sk.connect(ip_port)
sk.settimeout(5)



test = Test(sk)
test.send()
'''
ip_port = ('127.0.0.1', 1234)
sk = socket.socket()
sk.connect(ip_port)
sk.settimeout(5)
sk.send("这是一个测试".encode('utf-8'))
print(sk)