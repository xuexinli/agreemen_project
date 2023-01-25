# python
# -*- encoding: utf-8 -*-
'''
@File    :   agrement_AS.py
@Time    :   2022/12/17 17:54:36
@Author  :   Snowman 
@Version :   4.0
@Desc    :   None
'''

import random
import datetime
from pysmx.SM3 import hash_msg
import socket
from decodeandencode import SM4


SM4 = SM4()
# 该函数解决如下过长并有数组嵌套的情况
# ['IDLead', 'IDas', [['IDNode1', '2022-12-17 10:33:33.915551', '473811'], ['IDNode2', '2022-12-17 10:33:43.681607', '151954']], '0xd9a8b28e5e5d517e8c561207c583b68237fccdd3e0aa41866b07c481016d061e', 'b435118ad11b7cd8934abc720d88a56b8c39f6924cc0f874a9ed9d3fed1ca6b5']
def str_to_string_toolong(string):
    buffer = ""  # 临时字符串储存位置
    temp = 0  # 遇到 ' 则加一，为1时置零，并把字符串添加到数组里
    new_list = []  # 空数组
    i = - len(string)
    while True:
        i += 1
        if string[i] == "'":
            if temp and string[i+1] == "," or string[i+1] == "]":
                temp = 0
                new_list.append(buffer[0:len(buffer)-1])
                buffer = ""
            else:
                temp = 1

        elif string[i] == '[':
            temp_list, k = str_to_string_toolong(string[i:])
            i = k
            new_list.append(temp_list)

        elif string[i] == ']':
            return new_list, i
        if temp:
            buffer += string[i+1]


def str_to_string(string):
    buffer = ""  # 临时字符串储存位置
    temp = 0  # 遇到 ' 则加一，为1时置零，并把字符串添加到数组里
    new_list = []  # 空数组
    for i in range(len(string)):
        if string[i] == "'":
            if temp and string[i+1] == "," or string[i+1] == "]":
                temp = 0
                new_list.append(buffer[0:len(buffer)-1])
                buffer = ""
            else:
                temp = 1

        if temp:
            buffer += string[i+1]
    return new_list


class Massage_AS_Leader:
    K = ""
    massage_con = []
    r1 = -1
    r2 = -1
    r3 = -1

    def __init__(self, Me, conn, Other="", T="", r=-1, MAC="", K=""):
        if T == "":
            T = str(datetime.datetime.now())
        if r == -1:
            r = str(int(random.random()*1000000))
        self.K = K
        self.massage_con.append(Me)
        self.massage_con.append(Other)
        self.massage_con.append(T)
        self.massage_con.append(r)
        self.massage_con.append(MAC)
        self.conn = conn

    @staticmethod
    def random_test(self, num: int):
        if self.r1 == -1:
            self.r1 = num
        else:
            if self.r2 == -1:
                self.r2 = num
            else:
                if self.r3 == -1:
                    self.r3 = num

    '''
    算得MAC摘要在temp中储存顺序为：[IDas,IDLea,K,随机数]
    K为共享秘钥
    其中，随机数顺序应为时间顺序
    在调用时输入K，随机数1，随机数2等
    包含此次随机数与上一次及上上一次随机数
    '''
    @staticmethod
    def MAC_create(self, K):
        temp = [K]
        if self.r1 != -1:
            temp.append(str(self.r1))
            if self.r2 != -1:
                temp.append(str(self.r2))
                if self.r3 != -1:
                    temp.append(str(self.r3))
        '''hash_sha256 = sha256()
        hash_sha256 = hash_sha256.copy()
        hash_sha256.update(str(temp).encode('utf-8'))'''
        MAC = hash_msg(str(temp))
        return MAC

    # 验证消息的准确性
    def ju_massage(self, string: str):
        list_new = str_to_string(string)
        # 检验发送人员是否有误
        if ~(self.massage_con[1] == "") or (list_new[1] == self.massage_con[0] and list_new[0] == self.massage_con[1]):
            new_string = ""  # 字符串中含有毫秒串，datatime不能识别，所以把毫秒部分删去
            for i in list_new[2]:
                if i == ".":
                    break
                new_string += i
            date1 = datetime.datetime.now()  # 获取当前时间进行比对
            date2 = datetime.datetime.strptime(
                new_string, "%Y-%m-%d %H:%M:%S")  # 把字符串类型的时间转换为datatime类型
            # 检验消息是否已过30秒
            if (date1-date2).seconds > 30:
                print("消息已过期")
                exit()
            # 如果是服务端第一次接收消息则加入用户ID
            if self.massage_con[1] == "":
                self.massage_con[1] = list_new[0]
            # 存储随机数
            self.random_test(self, int(list_new[3]))
            # 如果有MAC，则进行验证
            if list_new[4]:
                MAC_test = self.MAC_create(self, self.K)
                if MAC_test != list_new[4]:
                    exit()
            self.massage_con[2] = str(datetime.datetime.now())
            self.massage_con[3] = str(int(random.random()*10000))
            return 1

        else:
            print("消息不是发给我的")
            return 0

    def get_list(self):
        self.random_test(self, int(self.massage_con[3]))
        MAC = self.MAC_create(self, self.K)
        self.massage_con[4] = MAC
        return self.massage_con

    def new_key(self):
        if self.r1 != -1 and self.r2 != -1 and self.r3 != -1:
            temp = [self.r2, self.r3, self.r1, self.K]
            key = hash_msg(str(temp))
            return self.massage_con[1], key
        else:
            print("没有完成协议协商")

    def massage_AS(self):
        conn = self.conn
        data1 = conn.recv(1024).decode()
        print("第一次握手接收消息",data1)
        a = self.ju_massage(data1)
        if a is False:
            exit()
        # 第二次握手(发送)
        massage2 = self.get_list()
        print("第二次握手发送消息：",massage2)
        conn.send(str(massage2).encode('utf-8'))
        # 第三次握手(接收)
        data3 = conn.recv(1024)
        temp =data3.decode('utf-8')
        print("第三次握手发送消息：",temp)
        a = self.ju_massage(temp)
        if a is False:
            exit()
        # 发送确认信息，并生成秘钥，并把该ID和对应秘钥放到字典里
        massage_sure = "OK"
        conn.send(str(massage_sure).encode('utf-8'))




def AS_Lead_First(IDas,conn,K:dict):
    #第一次握手(接收)
    massage = Massage_AS_Leader(IDas,conn,K=K["Leader_AS"])
    massage.massage_AS()
    ID , key = massage.new_key()
    temp = [key[:32],key[32:]]
    return 1,temp


#AS与Node的协议协商
def AS_Node_connect(IDas,IDLead,conn:socket,connet_key,K,raw_data):
    #后续需要加密
    s = socket.socket()
    s = conn
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
        temp_key = connet_key
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
                MAC_L = hash_msg(connet_key[1]+str(temp))
                if MAC_L == list_new[4]:
                    print("校验完成")
                    result,rec_ID =  AS_Node_return(IDas,IDLead,list_new,connet_key,K)
                    send_def(conn,result,connet_key)
                    return rec_ID
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
def AS_Node_return(IDas,IDLead,list_new:list,connet_key,K):
    result = [IDLead,IDas]
    temp_list =[] 
    rec_ID = {}
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
    MAC_AS_L_N = hash_msg(connet_key[1] + str(result))
    result.append(MAC_AS_L_N)
    return result,rec_ID
        
def send_def(sk:socket,massage,connet_key):
    temp_key = connet_key
    data = SM4.encryptSM4(temp_key[0],str(massage)+temp_key[1])
    sk.send(("AS_Node_back+"+data).encode())


def basic_handle(data):
    agreement = ""
    a = 0
    for i in range(len(data)):
        if data[i] !="+":
            agreement+= data[i]
        else :
            a = i
            break
    data = data[a +1:]
    return data,agreement