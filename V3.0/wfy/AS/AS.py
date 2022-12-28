#!python
# -*- encoding: utf-8 -*-
'''
@File    :   AS.py
@Time    :   2022/11/08 20:38:29
@Author  :   snowman
@Version :   3.0
@Contact :   
@License :   
@Desc    :   None
'''

import socketserver
from agrement_AS import *
from decodeandencode import SM4
from pysmx.SM3 import hash_msg
import datetime
import socket

K = {"Leader_AS":"Leader_AS","AS_Node1":"AS_Node1","AS_Node2":"AS_Node2","AS_Node3":"AS_Node3"}

#Global Variables
IDLead = "IDLead"
IDas = "IDas"
rec_ID={}#格式为ID：[IK:CK]
SM4 = SM4()




def AS_Node_connect(conn:socket,connet_key):
    global rec_ID
    while True:
            #后续需要加密
            s = socket.socket()
            s = conn
            raw_data = s.recv(1024).decode('utf-8')
            if raw_data=="":
                continue
            data =SM4.decryptSM4(connet_key[0],raw_data)
            data = data[:-32]
            #开始检验并生成秘钥
            #检验
            list_new,a = str_to_string_toolong(data)
            print(list_new)
            #开始验证信息正确性
            #1.验证消息发送方是否正确
            if list_new[1] == IDas:
                #2.验证MACL
                temp = []
                temp.append(list_new[0])
                temp.append(list_new[1])
                temp.append(list_new[2])
                temp.append(list_new[3])
                temp_key = rec_ID[IDLead]
                MAC_Leader = hash_msg(temp_key[1]+str(temp))
                print("MAC_Leader",MAC_Leader)
                if MAC_Leader == list_new[4]:
                    #3.验证Node的聚合MAC
                    MAC_xor = 0
                    for i in list_new[2]:
                        print(i)
                        #massage = Message_Node_Leader(i[0],Send=IDLead,T=i[1],r=i[2],K=K["AS_"+i[0]])
                        #MAC_temp = massage.MAC_return()
                        temp = [i[0],IDLead,i[1],i[2]]
                        MAC_temp = hash_msg(str(temp) + K["AS_"+temp[0]] )
                        print(MAC_temp,":"+i[0]+"的MAC值")
                        MAC_xor ^= int(MAC_temp,16)
                    if str(hex(MAC_xor)) ==list_new[3]:
                        #4.检验MAC_L
                        temp = [IDLead,IDas,list_new[2],hex(MAC_xor)]
                        temp_key = rec_ID[IDLead]
                        MAC_L = hash_msg(temp_key[1]+str(temp))
                        if MAC_L == list_new[4]:
                            print("校验完成")
                            return AS_Node_return(list_new)
                        else:
                            print("MAC_L有误")
                    else:
                        print("Node聚合的MAC错误")
                else:
                    print("MACL错误")
            else:
                print("这不是发给我的")



'''['IDLead', 'IDas', 
        [
            ['Node2', '2022-12-25 11:48:14.674870', '771257'], 
            ['Node3', '2022-12-25 11:48:14.681870', '300103'], 
            ['Node1', '2022-12-25 11:48:15.164528', '383510']], 
        '0xe1d83200609561269fb28a0ad9b5f1417e1bb8d50f8c4dd5a9d972be60b4be31', 
        '28b9a1fdc66907492ff5bf34d7f6545d8aad1c02a5e358b869c7238733cd2a3f']'''
def AS_Node_return(list_new:list):
    result = [IDLead,IDas]
    temp_list =[] 
    for i in list_new[2]:
        temp = [i[0]]
        T = str(datetime.datetime.now())
        R = str(int(random.random()*1000000))
        temp.append(T)
        temp.append(R)
        MAC_AS_Node = hash_msg(K["AS_"+temp[0]] + str(temp) + i[2])
        temp.append(MAC_AS_Node)
        temp_list.append(temp)
        temp = []
        key = hash_msg(K["AS_"+i[0]] + R + i[2])
        rec_ID[i[0]] = [key[:32],key[32:]]
    result.append(temp_list)
    MAC_AS_L_N = hash_msg(rec_ID[IDLead][1] + str(result))
    result.append(MAC_AS_L_N)
    return result
        
def send_def(sk:socket,massage):
    temp_key = rec_ID[IDLead]
    data = SM4.encryptSM4(temp_key[0],str(massage)+temp_key[1])
    sk.send(data.encode())


class MyServer(socketserver.BaseRequestHandler):
    """
    必须继承socketserver.BaseRequestHandler类
    """
    def handle(self):
        conn = self.request         # request里封装了所有请求的数据
        a,ID = self.agreement_test(conn)
        if not a:
            exit()
        connet_key = rec_ID[ID]
        print("完成秘钥协商：秘钥为:",connet_key)    
        #与Node的链接
        result =  AS_Node_connect(conn,connet_key)
        print(result)
        send_def(conn,result)
        print(rec_ID)
        



        '''if data == "exit":
            print("断开与%s的连接！" % (self.client_address,))
            break
        print("来自%s的客户端向你发来信息：%s" % (self.client_address, data))
        en_data = SM4.encrypt(connet_key[0],data+connet_key[1])
        conn.sendall(en_data.encode())'''
            #对Node-Leader-AS认证消息的处理
            


    
    def agreement_test(self,conn):
        #第一次握手(接收)
        massage = Massage_AS_Leader(IDLead,conn,K=K["Leader_AS"])
        massage.massage_AS()
        ID , key = massage.new_key()
        for i in rec_ID:
            if str(i) == ID:
                print("已有此ID和通信秘钥")
        rec_ID[ID] = [key[:32],key[32:]]
        return 1,ID






if __name__ == '__main__':
    # 创建一个多线程TCP服务器
    server = socketserver.ThreadingTCPServer(('127.0.0.1', 9999), MyServer)
    print("启动socketserver服务器！")
    # 启动服务器，服务器将一直保持运行状态
    server.serve_forever()


