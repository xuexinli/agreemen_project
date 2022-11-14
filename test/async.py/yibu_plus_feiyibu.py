#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File    :   yibu_plus_feiyibu.py
@Time    :   2022/11/09 16:22:28
@Author  :   snowman
@Version :   1.0
@License :   
@Desc    :   None
'''

import asyncio
import requests


async def download_image(url):
    #发送网络请求
    print("开始下载",url)

    loop = asyncio.get_event_loop()
    #requests模块默认不支持一步操作，所以只能用线程池配合实现
    future = loop.run_in_executor(None,requests.get,url)

    response = await future
    print("下载完成")
    #图片保存到本地文件
    file_name = url.rsplit("_")[-1]
    with open(file_name,mode='wb') as file_object:
        file_object.write(response.content)


if __name__ == '__main__':
    usr_list = [
        'https://img1.baidu.com/it/u=96588794,3355262961&fm=253&fmt=auto&app=138&f=JPEG?w=500&h=720',
        'https://gimg2.baidu.com/image_search/src=http%3A%2F%2Fi0.hdslb.com%2Fbfs%2Farticle%2Fb2daa552253c00c76e25b85f77ec0a16073ae911.jpg&refer=http%3A%2F%2Fi0.hdslb.com&app=2002&size=f9999,10000&q=a80&n=0&g=0n&fmt=auto?sec=1670575041&t=2c3ad1fa39d016dfc011a320bfc614e4',
        'https://gimg2.baidu.com/image_search/src=http%3A%2F%2Fi0.hdslb.com%2Fbfs%2Farticle%2F929e76800997ec1a13b34cd6962998f8b39307d2.jpg&refer=http%3A%2F%2Fi0.hdslb.com&app=2002&size=f9999,10000&q=a80&n=0&g=0n&fmt=auto?sec=1670575042&t=8716cf1f2687539fbb8e80fd75ff225f'
    ]

    tasks = [download_image(url) for url in usr_list]
    asyncio.run(asyncio.wait(tasks))
    



