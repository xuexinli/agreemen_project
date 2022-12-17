#python
# -*- encoding: utf-8 -*-
'''
@File    :   agreement_Node.py
@Time    :   2022/12/17 18:01:36
@Author  :   Snowman 
@Version :   3.0
@Desc    :   None
'''

import random
import datetime
#from hashlib import sha256
from pysmx.SM3 import hash_msg
import socket
import base64


class Message_Node_Leader():
    r = ""
    massage_con = []
    def __init__(self, Me,s:socket="",Send="",T="",r=-1,K=""):
        if T =="":
            T =  str(datetime.datetime.now())
        if r == -1:
            r = str(int(random.random()*1000000))
        self.K = K
        self.massage_con.append(Me)
        self.massage_con.append(Send)
        self.massage_con.append(T)
        self.massage_con.append(r)
        self.massage_con.append(hash_msg(str(self.massage_con)+str(self.K))) 
        self.s = s
    

    
    def main_Node(self):
        self.s.send(base64.b64encode(str(self.massage_con).encode('utf-8')))
        print(self.massage_con[0],"发送了一条消息")
        self.s.recv(1024).decode()
    def MAC_return(self):
        return self.massage_con[4]



