#python
# -*- encoding: utf-8 -*-
'''
@File    :   decodeandencode.py
@Time    :   2022/11/29 14:20:35
@Author  :   snowman
@Version :   1.0
@License :   
@Desc    :   None
'''

import binascii

from gmssl import sm4


class SM4:
    """
    国产加密 sm4加解密
    """

    def __init__(self):
        self.crypt_sm4 = sm4.CryptSM4()  # 实例化

    def str_to_hexStr(self, hex_str):
        """
        字符串转hex
        :param hex_str: 字符串
        :return: hex
        """
        hex_data = hex_str.encode('utf-8')
        str_bin = binascii.unhexlify(hex_data)
        return str_bin.decode('utf-8')

    def encryptSM4(self, encrypt_key, value):
        """
        国密sm4加密
        :param encrypt_key: sm4加密key
        :param value: 待加密的字符串
        :return: sm4加密后的十六进制值
        """
        crypt_sm4 = self.crypt_sm4
        crypt_sm4.set_key(encrypt_key.encode(), sm4.SM4_ENCRYPT)  # 设置密钥
        date_str = str(value)
        encrypt_value = crypt_sm4.crypt_ecb(date_str.encode())  # 开始加密。bytes类型
        return encrypt_value.hex()  # 返回十六进制值

    def decryptSM4(self, decrypt_key, encrypt_value):
        """
        国密sm4解密
        :param decrypt_key:sm4加密key
        :param encrypt_value: 待解密的十六进制值
        :return: 原字符串
        """
        crypt_sm4 = self.crypt_sm4
        crypt_sm4.set_key(decrypt_key.encode(), sm4.SM4_DECRYPT)  # 设置密钥
        decrypt_value = crypt_sm4.crypt_ecb(bytes.fromhex(encrypt_value))  # 开始解密。十六进制类型
        return decrypt_value.decode()
        # return self.str_to_hexStr(decrypt_value.hex())