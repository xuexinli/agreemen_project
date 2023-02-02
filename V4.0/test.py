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


a = 0xc2fab6a0b4f723e9b42d8b9a864e1c24cbbce9cb9fd912321e552b88282c8723
b = 0xeffb6d955c57600428119ed3d5992c94b0c1daec2b0f8b2b5fa5e1f62a48e38e
c = 0x42f788e7aee707c4d3976a04e23ebc18fdf7a7657f62376281a701c1d2f8f207

result = a ^ b ^c
print(hex(result))
