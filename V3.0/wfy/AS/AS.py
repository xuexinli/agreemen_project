#!python
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

import socketserver
from agrement_AS import Massage_AS_Leader,Message_Node_Leader,str_to_string_toolong
from decodeandencode import SM4
from pysmx.SM3 import hash_msg
import datetime
import threading

K = {"Leader_AS":"Leader_AS","AS_Node1":"AS_Node1","AS_Node2":"AS_Node2","AS_Node3":"AS_Node3"}

#Global Variables
IDLead = "IDas"
IDas = "IDas"
rec_ID={}#格式为ID：raw_key
SM4 = SM4()

#该函数解决如下过长并有数组嵌套的情况
# ['IDLead', 'IDas', [['IDNode1', '2022-12-17 10:33:33.915551', '473811'], ['IDNode2', '2022-12-17 10:33:43.681607', '151954']], '0xd9a8b28e5e5d517e8c561207c583b68237fccdd3e0aa41866b07c481016d061e', 'b435118ad11b7cd8934abc720d88a56b8c39f6924cc0f874a9ed9d3fed1ca6b5']


def AS_Node_connect(conn,connet_key):
    global rec_ID
    while True:
            #后续需要加密
            raw_data = conn.recv(1024).decode()
            if raw_data=="":
                continue
            data =SM4.decrypt(connet_key[0],raw_data)
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
                        massage = Message_Node_Leader(i[0],Send=IDLead,T=i[1],r=i[2],K=K["AS_"+i[0]])
                        MAC_temp = massage.MAC_return()
                        print(MAC_temp,"的MAC值")
                        MAC_xor ^= int(MAC_temp,16)
                    if MAC_xor ==list_new[3]:
                        #4.验证时间是否在正确范围内
                        new_string = ""#字符串中含有毫秒串，datatime不能识别，所以把毫秒部分删去
                        for i in i[1]:
                            if i ==".":
                                break
                            new_string +=i
                        date1 = datetime.datetime.now()#获取当前时间进行比对
                        date2 = datetime.datetime.strptime(new_string,"%Y-%m-%d %H:%M:%S")#把字符串类型的时间转换为datatime类型
                        #检验消息是否已过30秒
                        if (date1-date2 ).seconds > 30:
                            print("消息已过期")
                            exit()
                        #校验完成，生成返回信息
                        
                    else:
                        print("Node聚合的MAC错误")
                else:
                    print("MACL错误")
            else:
                print("这不是发给我的")


def AS_Node_return():
    pass



class MyServer(socketserver.BaseRequestHandler):
    """
    必须继承socketserver.BaseRequestHandler类
    """
    def handle(self):
        conn = self.request         # request里封装了所有请求的数据
        #conn.sendall('欢迎访问socketserver服务器！'.encode())
        a,ID = self.agreement_test(conn)
        if not a:
            exit()
        connet_key = rec_ID[ID]
        print("完成秘钥协商：秘钥为:",connet_key)    
        threat_AS_Node = threading.Thread(target=AS_Node_connect,args=(conn,connet_key,))
        threat_AS_Node.start()



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
        print(rec_ID)
        return 1,ID






if __name__ == '__main__':
    # 创建一个多线程TCP服务器
    server = socketserver.ThreadingTCPServer(('127.0.0.1', 9999), MyServer)
    print("启动socketserver服务器！")
    # 启动服务器，服务器将一直保持运行状态
    server.serve_forever()


