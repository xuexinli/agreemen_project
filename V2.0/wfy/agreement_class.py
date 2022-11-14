'''
Author: Snowman12137 80035057+Snowman12137@users.noreply.github.com
Date: 2022-11-07 18:38:35
LastEditors: Snowman12137 80035057+Snowman12137@users.noreply.github.com
LastEditTime: 2022-11-08 20:39:15
FilePath: \信安作品赛\V2.0\wfy\agreement_class.py
Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
'''
#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File    :   agreement_class.py
@Time    :   2022/11/08 20:36:27
@Author  :   snowman
@Version :   2.0
@Contact :   优化代码效率，更换国密算法，加入多线程框架
@License :   
@Desc    :   None
'''

import random
import datetime
from symbol import pass_stmt
from time import sleep
from hashlib import md5,sha256


class Massage:
    K = ""
    massage_con = []
    r1=-1
    r2=-1
    r3=-1
    def __init__(self,Me,Other,T="",r=-1,MAC="",K=""):
        if T =="":
            T =  str(datetime.datetime.now())
        if r == -1:
            r = str(int(random.random()*10000))
        self.K = K
        self.massage_con.append(Me)
        self.massage_con.append(Other)
        self.massage_con.append(T)
        self.massage_con.append(r)
        self.massage_con.append(MAC)

    #时刻更新系统时间和随机数
    #随机数为0-9999的随机数
    @staticmethod
    def resate_mass(self):
        self.massage_con[2] = str(datetime.datetime.now())
        self.massage_con[3] = str(int(random.random()*10000))

        #把字符串转换成数组类型
    @staticmethod
    def str_to_string(self,string):
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

    @staticmethod
    def MD5_cal(self,tar_list:list):
        a = str(tuple(tar_list))
        hash_md5 = md5()  # MD5 hash对象
        h_md5 = hash_md5.copy()  # 复制一个对象，避免频繁创建对象消耗性能
        h_md5.update(a.encode('utf-8'))  # 需要将字符串进行编码，编码成二进制数据
        md5_str = h_md5.hexdigest()  # 获取16进制的摘要
        return md5_str


    @staticmethod
    def random_test(self,num:int):
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
    def MAC_create(self,K):
        temp = [self.massage_con[0],self.massage_con[1],K]
        if self.r1 !=-1:
            temp.append(str(self.r1))
            if self.r2 != -1:
                temp.append(str(self.r2))
                if self.r3 !=-1:
                    temp.append(self.r3)
        hash_sha256 = sha256()
        hash_sha256 = hash_sha256.copy()
        hash_sha256.update(str(temp).encode('utf-8'))
        MAC  = hash_sha256.hexdigest()
        return MAC  


    #获取MD5摘要
    def MD5(self):
        self.resate_mass(self)
        self.random_test(self,self.massage_con[3])
        return self.MD5_cal(self,self.massage_con)
    
    #验证MD5摘要
    def ju_massage(self,string:str):
        turn_list = self.str_to_string(self,string)
        MD5_result = turn_list[5]
        list_new = turn_list[0:5]
        #检验MD5是否正确
        if MD5_result == self.MD5_cal(self,list_new):
            print("消息没有被篡改")
        else:
            print("消息有误")
        #检验发送人员是否有误
        if list_new[1] == self.massage_con[0] and  list_new[0] == self.massage_con[1]:
            #检验时间是否超时
            new_string = ""#字符串中含有毫秒串，datatime不能识别，所以把毫秒部分删去
            for i in list_new[2]:
                if i ==".":
                    break
                new_string +=i
            date1 = datetime.datetime.now()#获取当前时间进行比对
            date2 = datetime.datetime.strptime(new_string,"%Y-%m-%d %H:%M:%S")#把字符串类型的时间转换为datatime类型
            #检验消息是否已过30秒
            if (date1-date2 ).seconds > 30:
                print("消息已过期")
                exit()
            #通过所有检验则认为该消息是可信的则存储随机数
            self.random_test(self,int(list_new[3]))
            MAC = self.MAC_create(self,self.K)
            self.massage_con[4] = MAC

        else:
            print("消息不是发给我的")
        

        
        

    
 
    

   

