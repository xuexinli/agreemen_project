#python
# -*- encoding: utf-8 -*-
'''
@File    :   Leader.py
@Time    :   2022/11/08 20:38:10
@Author  :   snowman
@Version :   3.0
@Contact :   
@License :   
@Desc    :   None
'''

import socket
import base64
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
K = {"Leader_AS":"Leader_AS","Leader_Node":"Leader_Node"}

connet_key = {}
SM4_temp = SM4()
receve_massage = {}
send_massage = []


class MyServer(socketserver.BaseRequestHandler):
    """
    必须继承socketserver.BaseRequestHandler类
    """
    def handle(self):
        global send_massage,receve_massage
        conn = self.request    
        while True:     
            data = conn.recv(1024).decode()
            temp = base64.b64decode(data).decode('utf-8')
            #abc = [IDNode,IDLeader,T,r,MAC]
            if data=="":
                print("data是空的")
                break
            abc = str_to_string(temp)
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
            while True:
                sleep(1)
                if receve_massage[IDNode] != "":
                    temp = receve_massage[IDNode]
                    receve_massage[IDNode] = ""
                    conn.send(base64.b64encode(str(temp).encode('utf-8')))
                else:
                    continue

        
        




def server_threat(ip):   
   
    #启动多线程服务器
    print("这里是server_threat")
    server = socketserver.ThreadingTCPServer(ip, MyServer)
    print("启动Leader服务器！")
    # 启动服务器，服务器将一直保持运行状态
    server.serve_forever()

#对AS发送聚合后的消息
def main_send(sk):
    global send_massage,receve_massage
    print("这里是main_send线程")
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
            temp_key = connet_key[IDas]
            MAC_Leader = hash_msg(temp_key[1]+str(massage_AS))
            massage_AS.append(MAC_Leader)
            en_data = SM4_temp.encryptSM4(temp_key[0],str(massage_AS)+temp_key[1])#
            sk.send(en_data.encode('utf-8'))
            print("成功发送",massage_AS)



            




#接收AS的聚合认证回应
def main_rece(sk):
    while True:
        sk.settimeout(50000)
        raw_data = sk.recv(1024).decode()
        if raw_data=="":
            continue
        else: 
            data = SM4_temp.decryptSM4(connet_key[IDas][0],raw_data)
            data = data[:-32]
            print("收到的消息：",data)
            list_new= str_to_string_toolong(data)
            list_new = list_new[0]
            temp = [IDLead,IDas,list_new[2]]
            MAC_test = hash_msg(connet_key[IDas][1]+str(temp))
            if MAC_test == list_new[3]:
                print("MAC_L校验通过")
            for i in list_new[2]:
                receve_massage[i[0]] = i
            
            





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
    connet_key[a1] = [a2[:32],a2[32:]]
    print("协商秘钥为：",connet_key)
    
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
    threat_main_send = threading.Thread(target=main_send,args=(sk,))
    threat_main_rece = threading.Thread(target=main_rece,args=(sk,))
    threat_server.start()
    threat_main_send.start()
    threat_main_rece.start()


if __name__ == '__main__':
    main()







