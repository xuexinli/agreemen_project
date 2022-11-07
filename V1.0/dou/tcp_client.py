from Message import Message
import socket
from time import time
from Toolkit import listHash
from random import random
from sm2encry import sm2Encry
#新鲜度阈值
fresh_threshold = 1

#等待响应的时间
waiting_time = 5

#预置密钥
key = 0

#创建tcp套接字
sockfd = socket.socket()

#连接服务器
server_addr =  ('127.0.0.1',8888)#服务端地址
sockfd.connect(server_addr)

while True:
    #Leader收到认证请求 
    data =sockfd.recv(1024).decode()
    #解密
    msg_decrypt = sm2Encry.decrypt(data)
    print("Server(解密):",msg_decrypt)#打印接受内容
    
    
    #判断T1新鲜性
    msg_1 = Message.strToMsg(msg_decrypt)
    if time() - msg_1.Time > fresh_threshold:
        continue
    
    #Leader发送响应消息
    time_2 = time()
    rand_2 = random()
    HASH = listHash(key, time_2, rand_2, msg_1.Rand)
    data = str(Message("server", "server", time_2, rand_2, HASH))
    #加密
    msg_encrypt = sm2Encry.encrypt(data)
    print("Client(加密):",msg_encrypt)
    sockfd.send(msg_encrypt.encode())#转换为字节再发送
    #解密
    #msg_decrypt = sm2Encry.decrypt(msg_encrypt)
    #print("Client(解密):",msg_decrypt)
    
    #Leader收到认证确认 
    data =sockfd.recv(1024).decode()
    #解密
    msg_decrypt = sm2Encry.decrypt(data)
    print("Server(解密):",msg_decrypt)#打印接受内容
    
    #判断T3新鲜性与MACas的正确性
    msg_3 = Message.strToMsg(msg_decrypt)
    if time() - msg_3.Time > fresh_threshold:
        continue
    HASH = listHash(key, "server", "server", msg_3.Time, msg_3.Rand, rand_2)
    if msg_3.Mac != HASH:
        continue
    
    #返回认证结果
    data = "ok"
    if data == 'ok':
        #加密
        msg_encrypt = sm2Encry.encrypt(data)
        print("Client(加密):",msg_encrypt)
        sockfd.send(msg_encrypt.encode())#转换为字节再发送
        #解密
        #msg_decrypt = sm2Encry.decrypt(msg_encrypt)
        #print("Client:",msg_decrypt)
    
    #计算密钥
    key = listHash("client", "server", msg_1.Rand, rand_2, msg_3.Rand, key)#global
    print("key is :",hex(key)[2:])
    #关闭套接字
    sockfd.close()
    break

