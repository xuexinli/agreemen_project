
from time import sleep

def str_to_string_toolong(string):
    buffer = ""  # 临时字符串储存位置
    temp = 0  # 遇到 ' 则加一，为1时置零，并把字符串添加到数组里
    new_list = []  # 空数组
    i = - len(string)
    while True:
        i += 1
        if string[i] == "'":
            if temp and string[i+1] == "," or string[i+1] == "]":
                temp = 0
                new_list.append(buffer[0:len(buffer)-1])
                buffer = ""
            else:
                temp = 1

        elif string[i] == '[':
            temp_list, k = str_to_string_toolong(string[i:])
            i = k
            new_list.append(temp_list)

        elif string[i] == ']':
            return new_list, i
        if temp:
            buffer += string[i+1]

list_test = "['IDLead', 'IDas', [['Node1', '2022-12-27 13:26:14.707524', '753473', '563e970357eaeda480066f4751cc201720f35a8230eee71c9a5cf5800d18d47b'], ['Node3', '2022-12-27 13:26:14.708524', '872045', '3cdeec6a26276512eacdd16dea259bf299a80efd66db602b7047fa20495f6bbc'], ['Node2', '2022-12-27 13:26:14.708524', '854828', '402f0edac303ef8b0b398443a4fb6da248d41d0fcfecb6372e95676de16bf046']], 'a54d79c6bd78d30f3f327082659891d0e687ba2a3abee2bc277d2b96f5d77a0c']"
turn = str_to_string_toolong(list_test)
print(turn)






