'''
Author: snowman
Date: 2022-11-07 18:38:35
Description: 
'''


import asyncio


async def set_after(fut):
    await asyncio.sleep(2)
    fut.set_result('666')


async def main():
    #获取当前事件循环
    loop = asyncio.get_running_loop()

    #创建一个任务（future对象）
    fut = loop.create_future()

    #创建一个task对象，绑定了set_after函数，函数内部两秒以后，对fut赋值
    #即手动设置future任务的最终结果，那么fut就可以结束了
    await loop.create_task( set_after(fut) )

    #等待Future对象获取最终结果，否则一直等下去
    data = await fut
    print(data)



asyncio.run( main() )