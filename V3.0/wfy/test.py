#python
# -*- encoding: utf-8 -*-
'''
@File    :   test.py
@Time    :   2022/12/25 11:04:29
@Author  :   Snowman 
@Version :   1.0
@Desc    :   None
'''


from gmssl.sm4 import CryptSM4, SM4_ENCRYPT, SM4_DECRYPT
import binascii



class SM4:
    """
    国密sm4加解密
    """

    def __init__(self):
        self.crypt_sm4 = CryptSM4()

    def str_to_hexStr(self, hex_str):
        """
        字符串转hex
        :param hex_str: 字符串
        :return: hex
        """
        hex_data = hex_str.encode('utf-8')
        str_bin = binascii.unhexlify(hex_data)
        temp = str_bin.decode('utf-8')
        return temp

    def encrypt(self, encrypt_key, value):
        """
        国密sm4加密
        :param encrypt_key: sm4加密key
        :param value: 待加密的字符串
        :return: sm4加密后的hex值
        """
        crypt_sm4 = self.crypt_sm4
        crypt_sm4.set_key(encrypt_key.encode(), SM4_ENCRYPT)
        encrypt_value = crypt_sm4.crypt_ecb(value.encode())  # bytes类型
        return encrypt_value.hex()

    def decrypt(self, decrypt_key, encrypt_value):
        """
        国密sm4解密
        :param decrypt_key:sm4加密key
        :param encrypt_value: 待解密的hex值
        :return: 原字符串
        """
        crypt_sm4 = self.crypt_sm4
        crypt_sm4.set_key(decrypt_key.encode(), SM4_DECRYPT)
        decrypt_value = crypt_sm4.crypt_ecb(bytes.fromhex(encrypt_value))  # bytes类型
        return self.str_to_hexStr(decrypt_value.hex())

'''str_data = "test_string"
key = "3l5butlj26hvv313"
SM4 = SM4()
print("待加密内容：", str_data)
encoding = SM4.encrypt(key, str_data)
print("国密sm4加密后的结果：", encoding)
print("国密sm4解密后的结果：", SM4.decrypt(key, encoding))'''


key = "325b273251a93aa2cd40978113cd40ef"
temp = 0x34f3526ad940f0f4993de0a3fb4e032ffc2bd037a894e8b22427ddf1b48ae1cedd2019ba064005ce3b990d88504601c106f8821f6b50fce89e470a57ad1818616183ec2ef2f4128dde9f3f24c489b074f85363dbecd6f8296715939740da920001565637d8526623ee3fd65042592adc92a6fe04657b3fce281df2ed622559b4046d1ee7edbff723a767ce8465c032bd04c1c600277198681ea5992244df8c36341405b6c4c258c7dadf5e5c08b35994d2f412042ffcfbe9705e02c971ffb60f9a19f762e2274cd651bdb906d81f1a8e9ceed7506960975e55b21da784b5cb6c6ec3b087ebe4887a392fea8d722fd9c9838d78e6940f2fc8639e22c682adc2620f5df1ff7a48f938a1232be64bed4834a9eb6422541e43b9b702396059e0470fefc8afaf459ae1c2304cb3022e968c374e28143e625ef1c79d75c27af268545d84efcbcb6cbfc4a9606d8094f3a5a3865c9eefbd9bab7b6982d34f535faf0357
SM4 = SM4()

print(SM4.decrypt(key,str(temp)))



