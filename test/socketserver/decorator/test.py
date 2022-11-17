'''
Author: snowman
Date: 2022-11-08 21:25:19
Description: 
'''

import time


def display_time(func):
    def wrapper(*args):
        t1 = time.time()
        result = func(*args)
        t2 = time.time()
        print("Total time : {:.4}".format(t2-t1))
        return result
    return wrapper

def  is_number(num):
    if num < 2:
        return False
    elif num ==2:
        return True
    else:
        for i in range(2,int(num/2)):
            if num % i ==0:
                return False
        return True

@display_time
def number(maxnum):
    count = 0
    for i in range(2,maxnum):
        if is_number(i):
            count = count +1
    return count


count = number(100000)
print(count)

