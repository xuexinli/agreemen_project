#python
# -*- encoding: utf-8 -*-
'''
@File    :   Leader.py
@Time    :   2022/11/08 20:38:10
@Author  :   snowman
@Version :   4.0
@Contact :   
@License :   
@Desc    :   None
'''

import socket
import socketserver
from agreement_Leader import *
from decodeandencode import SM4
import threading
import datetime
from time import sleep
from pysmx.SM3 import hash_msg

#基本信息
IDas = "IDas"
IDLead = "IDLead"
K = {"Node_Leader":"Node_Leader","Leader_AS":"Leader_AS","Leader_Node":"Leader_Node"}

rec_ID = {}
SM4_temp = SM4()
receve_massage = {}
send_massage = []


class MyServer(socketserver.BaseRequestHandler):
    """
    必须继承socketserver.BaseRequestHandler类
    """
    def handle(self):
        self.IDNode = ""
        global send_massage,receve_massage
        conn = self.request    
        self.thread = threading.Thread(target=self.recieveData)
        self.thread.start()
        while True:
            sleep(1)
            IDNode = self.IDNode
            if IDNode in receve_massage :
                temp = receve_massage[IDNode]
                receve_massage[IDNode] = ""
                conn.send(("AS_Node+"+str(temp)).encode())
            else:
                continue
    #接收Node消息的线程，可以实现同时收发信息        
    def recieveData(self):
        while True:
            conn = self.request
            conn.settimeout(99999)
            raw_data = conn.recv(1024).decode()
            if raw_data == "":
                continue
            data,agreement = basic_handle(raw_data)
            #协议处理
            if agreement == "AS_Node":
                #与Node的链接
                self.IDNode = self.AS_Node_connect(data)
            elif agreement == "AS_Node_back":
                handle(data)
            elif agreement == "Node_Leader":
                self.Node_Leader(data)
            elif agreement == "MASSAGE_PRINT":
                print("IDLead向你发来明文消息",data)
            elif agreement == "MASSAGE_DECODE":
                print("这是加密消息")
            elif data =="":
                print("协议有误")
            else:
                print("消息中的协议有误，错误消息为",data)

    def Node_Leader(data,self):
        a,temp = AS_Lead_First(IDLead,self.request,K)
        if not a:
            print("协商有误")
        rec_ID[IDLead] = temp
        print("完成秘钥协商：秘钥为:",rec_ID[IDLead]) 
    #对于AS_Node的处理
    def AS_Node_connect(self,data):
        abc = str_to_string(data)
        IDNode = abc[0]
        new_string = ""#字符串中含有毫秒串，datatime不能识别，所以把毫秒部分删去
        for i in abc[2]:
            if i ==".":
                break
            new_string +=i
        date1 = datetime.datetime.now()#获取当前时间进行比对
        date2 = datetime.datetime.strptime(new_string,"%Y-%m-%d %H:%M:%S")#把字符串类型的时间转换为datatime类型
        #检验消息是否已过30秒
        if (date1-date2 ).seconds > 30:
            print("消息已过期")
            exit()
        send_massage.append(abc)
        receve_massage[IDNode] = ""
        abc = []
        return IDNode
        
        




def server_threat(ip):   
   
    #启动多线程服务器
    print("这里是server_threat")
    server = socketserver.ThreadingTCPServer(ip, MyServer)
    print("启动Leader服务器！")
    # 启动服务器，服务器将一直保持运行状态
    server.serve_forever()

#对AS发送聚合后的消息
def check_send(sk):
    global send_massage,receve_massage
    print("这里是检测发送check_send线程")
    while True:
        i = 0
        while i < 5 :
            sleep(1)
            i +=1
            if len(send_massage) > 4:
                break
            else:
                continue
        if send_massage == []:
            continue
        else:
            #开始聚合操作
            #在这一刻把数组里面的数据取出，然后清空
            #abc = [IDNode,IDLeader,T,r,MAC]
            temp = send_massage
            send_massage = []
            temp_2 = []
            xor_MAC = 0
            for i in temp:
                print(i[0]+":"+i[4])
                xor_MAC ^=  int(i[4],16)
                a = []
                a.append(i[0])
                a.append(i[2])
                a.append(i[3])
                temp_2.append(a)
            massage_AS= [IDLead,IDas,temp_2,hex(xor_MAC)]
            temp_key = rec_ID[IDas]
            MAC_Leader = hash_msg(temp_key[1]+str(massage_AS))
            massage_AS.append(MAC_Leader)
            en_data = SM4_temp.encryptSM4(temp_key[0],str(massage_AS)+temp_key[1])#
            sk.send(("AS_Node+"+en_data).encode('utf-8'))
            print("成功发送",massage_AS)



            




#接收AS的聚合认证回应
def handle(data):
    data = SM4_temp.decryptSM4(rec_ID[IDas][0],data)
    data = data[:-32]
    print("收到的消息：",data)
    list_new= str_to_string_toolong(data)
    list_new = list_new[0]
    temp = [IDLead,IDas,list_new[2]]
    MAC_test = hash_msg(rec_ID[IDas][1]+str(temp))
    if MAC_test == list_new[3]:
        print("MAC_L校验通过")
    for i in list_new[2]:
        receve_massage[i[0]] = i
            
            




def AS_check_recive(sk):
    massage_handle(sk)


def massage_handle(conn):
    while True:
        conn.settimeout(99999)
        raw_data = conn.recv(1024).decode()
        if raw_data == "":
            continue
        data,agreement = basic_handle(raw_data)
        #协议处理
        if agreement == "AS_Node_back":
            handle(data)
        elif agreement == "MASSAGE_PRINT":
            print("IDLead向你发来明文消息",data)
        elif agreement == "MASSAGE_DECODE":
            print("这是加密消息")
        elif data =="":
            print("协议有误")
        else:
            print("消息中的协议有误，错误消息为",data)



def main():
    global send_massage,receve_massage
    ip_port = ('127.0.0.1', 9999)
    sk = socket.socket()
    sk.connect(ip_port)
    sk.settimeout(5)

    #协议一链接（AS与Leader）
    massage = Massage_AS_Leader(IDLead,sk,Other=IDas,K=K["Leader_AS"])
    a1,a2 = massage.massage_Lead()
    #[CK,IK]
    rec_ID[a1] = [a2[:32],a2[32:]]
    print("协商秘钥为：",rec_ID)
    
    #sk为 与AS的链接（主）
    print("链接成功")

    #启动多线程服务器之前先广播出自己的ID和IP端口
    #ip = sk.getsockname()
    ip = ('127.0.0.1', 8888)
    s_temp =socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s_temp.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

    '''PORT_broadcast = 6666
    network = '<broadcast>'
    s_temp.sendto(str(ip).encode('utf-8'), (network,PORT_broadcast))
    s_temp.close()'''


    #接下来与Node的链接放到主线程里运行
    threat_server = threading.Thread(target=server_threat,args=(ip,))
    #逻辑处理线程
    threat_main_send = threading.Thread(target=check_send,args=(sk,))
    threat_AS_check_recive = threading.Thread(target=AS_check_recive,args=(sk,))
    threat_server.start()
    threat_main_send.start()
    threat_AS_check_recive.start()


if __name__ == '__main__':
    main()







