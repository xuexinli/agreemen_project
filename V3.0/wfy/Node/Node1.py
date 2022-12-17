#python
# -*- encoding: utf-8 -*-
'''
@File    :   Node1.py
@Time    :   2022/12/02 12:28:54
@Author  :   snowman
@Version :   2.0
@License :   
@Desc    :   None
'''

# here put the import lib


import socket
from agreement_Node import Message_Node_Leader
from decodeandencode import SM4

#基本信息
IDas3 = "IDas"
IDLead2 = "IDLead"
IDNode1 = "Node1"
IDNode2 = "Node2"
IDNode3 = "Node3"

#共享秘钥
AS_Node1="AS_Node1"
AS_Node2="AS_Node2"
AS_Node3="AS_Node3"


'''#接收Leader广播的地址信息
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
#设置6666为广播地址
PORT = 6666
s.bind(('', PORT))
print('Listening for broadcast at ', s.getsockname())'''



ip_port = ('127.0.0.1', 8888)
sk = socket.socket()
sk.connect(ip_port)
sk.settimeout(100000)

massage1 = Message_Node_Leader(IDNode1,s=sk,Send=IDLead2,K=AS_Node1)
massage1.main_Node()




