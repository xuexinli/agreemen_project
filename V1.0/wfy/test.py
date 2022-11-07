import datetime
from re import L
from time import sleep
from typing import List
import datetime


string = "2022-11-04 15:32:24.005554"
new_string = ""
for i in string:
    if i ==".":
        break
    new_string +=i


date1 = datetime.datetime.now()
date2 = datetime.datetime.strptime(new_string,"%Y-%m-%d %H:%M:%S")
print((date1-date2 ).seconds)





