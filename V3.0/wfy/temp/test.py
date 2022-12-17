#python
# -*- encoding: utf-8 -*-
'''
@File    :   test.py
@Time    :   2022/12/17 09:41:03
@Author  :   Snowman 
@Version :   1.0
@Desc    :   None
'''

'''def str_to_string(string):
        buffer = "" #临时字符串储存位置
        temp = 0    #遇到 ' 则加一，为1时置零，并把字符串添加到数组里
        new_list = []   #空数组
        for i in range(len(string)):
            if string[i] == "'":
                if temp and string[i+1]=="," or string[i+1]=="]":
                    temp = 0 
                    new_list.append(buffer[0:len(buffer)-1])
                    buffer = ""
                else:
                    temp = 1
            
            if temp :
                buffer += string[i+1]
        return new_list'''

def str_to_string(string):
        buffer = "" #临时字符串储存位置
        temp = 0    #遇到 ' 则加一，为1时置零，并把字符串添加到数组里
        new_list = []   #空数组
        i = - len(string)
        while True:
            i+=1
            if string[i] == "'":
                if temp and string[i+1]=="," or string[i+1]=="]":
                    temp = 0 
                    new_list.append(buffer[0:len(buffer)-1])
                    buffer = ""
                else:
                    temp = 1
            
            elif string[i] == '[':
                temp_list,k  = str_to_string(string[i:])
                i = k
                new_list.append(temp_list)

            elif string[i] == ']':
                return new_list,i            
            if temp :
                buffer += string[i+1]
            

abc = "['IDLead', 'IDas', [['IDNode1', '2022-12-17 10:33:33.915551', '473811'], ['IDNode2', '2022-12-17 10:33:43.681607', '151954']], '0xd9a8b28e5e5d517e8c561207c583b68237fccdd3e0aa41866b07c481016d061e', 'b435118ad11b7cd8934abc720d88a56b8c39f6924cc0f874a9ed9d3fed1ca6b5']"
print("这里是",abc)
end,i = str_to_string(abc)
print("结果是",end)

