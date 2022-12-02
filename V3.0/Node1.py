#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File    :   Node1.py
@Time    :   2022/12/02 12:28:54
@Author  :   snowman
@Version :   1.0
@License :   
@Desc    :   None
'''

# here put the import lib


import socket
import base64
from agreement_class import Massage
from decodeandencode import SM4

#基本信息
IDas3 = "IDas"
IDLead2 = "IDLead"
IDNode1 = "IDNode"
k="this_is_key"
connet_key = ""

#接收Leader广播的地址信息
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

PORT = 6666
s.bind(('', PORT))
print('Listening for broadcast at ', s.getsockname())



ip_port = ('127.0.0.1', 9999)
sk = socket.socket()
sk.connect(ip_port)
sk.settimeout(5)









