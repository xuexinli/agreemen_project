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


ip_port = ('127.0.0.1', 9999)
sk = socket.socket()
sk.connect(ip_port)
sk.settimeout(5)

massage1 = input("输入：")
sk.send(massage1.encode('utf-8'))


sleep(30)

