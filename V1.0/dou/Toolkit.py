def listHash(*arg):
    hashcode = 0
    for i in str(arg):
        hashcode = (hashcode * 31 + ord(i)) & ((1 << 128) - 1)
    return hashcode