'''
Author: snowman
Date: 2022-11-07 18:38:35
Description: 
'''


import asyncio


async def set_after(fut):
    pass


async def main():
    #获取当前事件循环
    loop = asyncio.get_running_loop()

    #创建一个任务（future对象）
    fut = loop.create_future()

    #创建一个task对象，绑定了set_after函数，函数内部两秒以后，对fut赋值



asyncio.run( main() )