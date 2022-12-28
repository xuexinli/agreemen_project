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

def str_to_string(string):
        buffer = "" #临时字符串储存位置
        temp = 0    #遇到 ' 则加一，为1时置零，并把字符串添加到数组里
        new_list = []   #空数组
        for i in range(len(string)):
            if string[i] == "'":
                if temp and string[i+1]=="," or string[i+1]=="]":
                    temp = 0 
                    new_list.append(buffer[0:len(buffer)-1])
                    buffer = ""
                else:
                    temp = 1
            
            if temp :
                buffer += string[i+1]
        return new_list



class Message_Node_Leader():
    r = -1
    R = -1
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
        self.r = r
        self.massage_con.append(hash_msg(str(self.massage_con)+str(self.K))) 
        self.s = s
    

    
    def main_Node(self):
        self.s.send(base64.b64encode(str(self.massage_con).encode('utf-8')))
        print(self.massage_con[0],"发送了一条消息")
        data = self.s.recv(1024).decode()
        temp = base64.b64decode(data).decode('utf-8')
        list_new = str_to_string(temp)
        #1.校验时间
        new_string = ""#字符串中含有毫秒串，datatime不能识别，所以把毫秒部分删去
        for i in list_new[1]:
            if i ==".":
                break
            new_string +=i
        date1 = datetime.datetime.now()#获取当前时间进行比对
        date2 = datetime.datetime.strptime(new_string,"%Y-%m-%d %H:%M:%S")#把字符串类型的时间转换为datatime类型
        #检验消息是否已过30秒
        if (date1-date2 ).seconds > 30:
            print("消息已过期")
            exit()
        #2。校验MAC
        temp = [self.massage_con[0],list_new[1],list_new[2]]
        MAC_AS_Node = hash_msg(self.K +str(temp)+self.r)
        print(MAC_AS_Node)
        if MAC_AS_Node == list_new[3]:
            print("校验完成")
            return self.new_key(list_new[2])


    def new_key(self,R):
        key = hash_msg(self.K + R +self.r)
        return  key
