from Message import Message
import socket
from time import time
from Toolkit import listHash
from random import random
from sm2encry import sm2Encry
#新鲜度阈值
fresh_threshold = 1

#等待响应的时间
waiting_time = 50

#预置密钥
key = 0

#创建tcp套接字
sockfd = socket.socket(socket.AF_INET,socket.SOCK_STREAM,)

#绑定地址
sockfd.bind(('0.0.0.0',8888))

#设置监听
sockfd.listen(10)

while True:
    #阻塞等待处理
    print("*****Waiting for connect...******")
    try:
        connfd,addr = sockfd.accept()
        print("Connect from:",addr)#打印连接的客户端地址
    except KeyboardInterrupt:
        print("Server exit")
        break
    except Exception as e:
        print("e")
        continue
    
    #AS发起认证请求
    rand_1 = random()
    time_1 = time()
    data = str(Message("server","server" , time_1, rand_1,key))#待修改
    #加密
    msg_encrypt = sm2Encry.encrypt(data)
    print("Server(加密):",msg_encrypt)
    connfd.send(msg_encrypt.encode())#转换为字节再发送
    #解密
    #msg_decrypt = sm2Encry.decrypt(msg_encrypt)
    #print("Server(解密):",msg_decrypt)
    
    
    #AS接收响应信息
    data = connfd.recv(1024).decode()
    if not data:
        continue
    #解密
    msg_decrypt = sm2Encry.decrypt(data)
    print("Client(解密):",msg_decrypt)#打印收到的消息 
    
    #判断T2新鲜性与MACl的正确性
    msg_2 = Message.strToMsg(msg_decrypt)
    if time() - msg_2.Time > fresh_threshold:
        continue
    HASH = listHash(key, msg_2.Time, msg_2.Rand, rand_1)
    if msg_2.Mac != HASH:
        continue
    
    
    #AS发起认证确认
    rand_3 = random()
    time_3 = time()
    data = str(Message("server", "server", time_3, rand_3, listHash(key, "server", "server", time_3, rand_3, msg_2.Rand)))
    #加密
    msg_encrypt = sm2Encry.encrypt(data)
    print("Server(加密):",msg_encrypt)
    connfd.send(msg_encrypt.encode())#转换为字节再发送
    #解密
    #msg_decrypt = sm2Encry.decrypt(msg_encrypt)
    #print("Server(解密):",msg_decrypt)
    
    #接收认证结果
    data = connfd.recv(1024).decode()
    #解密
    msg_decrypt = sm2Encry.decrypt(data)
    if msg_decrypt != "ok":
        continue
    print("Client(解密):",msg_decrypt)#打印收到的消息
    
    
    #计算密钥
    key = listHash("client", "server", rand_1, msg_2.Rand, rand_3, key)

    print("key is :", hex(key)[2:])
    connfd.close()
#关闭套接字
sockfd.close()