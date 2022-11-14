'''
Author: snowman
Date: 2022-11-09 15:25:11
Description: 使用进行池和线程池进行异步操作用到的对象
'''

#示例1
'''import time
from concurrent.futures import Future
from concurrent.futures.thread import ThreadPoolExecutor
from concurrent.futures.process import ProcessPoolExecutor


def func(value):
    time.sleep(1)
    print(value)
    return 123

#创建线程池
pool = ThreadPoolExecutor(max_workers=5)

#创建进程池
#或ProcessPoolExecutor（max_workers=5）

for i in range(10):
    fut = pool.submit(func,i)
    print(fut)
'''
#示例2
#可能会存在异步交叉使用

import time
import asyncio
import concurrent.futures

def func1():
    #某个耗时操作
    time.sleep(2)
    return '执行完毕'


async def main():
    loop = asyncio.get_running_loop()

    '''
    1.Run in the default loop's executor(ThreadPoolExecutor)
    第一步: 内部会先调用ThreadPoolExecutor的submit方法去线程池中
            申请一个线程去执行func1函数，并返回一个concurrent.futures.Futhre对象
    第二步：调用asyncio.wrap_future将concurrent.futures.Futhre
            对象包装为asycio.Future对象，因为concurrent.futures.Futhre
            对象不支持await语法，所以要包装为asycio.Futhre才能使用
    '''
    fut = loop.run_in_executor(None,func1)
    result = await fut
    print('default thread pool',result)

    '''
    2.Run in a custom thread pool:
     with concurrent.futures.ThreadPoolExecutor() as pool:
        result = await loop.run_in_executor(
                pool,func1)
        print('custom thread pool',result)
    3.Run in a custom process pool:
     with concurrent.futures.ProvessPoolExecutor() as pool:
        result = await loop.run_in_executor(
                pool,func1)
        print('custom process pool',result)
    
    
    ''' 


asyncio.run(main())



