import socket
import threading
from time import sleep



def send_def(conn):
    while True:
        test = input("这里是输入")
        conn.send(test.encode())
        
def receive_def(conn):
    while True:
        raw_data = conn.recv(1024).decode()
        if raw_data == "":
            continue
        print(raw_data)



ip_port = ('127.0.0.1', 9999)
sk = socket.socket()
sk.connect(ip_port)
sk.settimeout(5)
print("链接成功",sk)


raw_data = sk.recv(1024).decode()
print(raw_data)
threat_main_send = threading.Thread(target=send_def,args=(sk,))
threat_main_receive = threading.Thread(target=receive_def,args=(sk,))
threat_main_send.start()
threat_main_receive.start()




