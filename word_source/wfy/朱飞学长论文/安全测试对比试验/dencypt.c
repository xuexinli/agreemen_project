#include<iostream>
#include<cstring>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/objects.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/err.h>
#include <memory.h>
#include <stdlib.h>
#include <openssl/aes.h>
using namespace std;
string hexStringToString(string hexStr)
{
	string ret;
	char asd[255];
	int n;
	string hexString = "0123456789abcdef";
	for(int i=0;i<hexStr.length()/2;i++){
        n=hexString.find(hexStr.at(2*i))*16+hexString.find(hexStr.at(2*i+1));
        ret.insert(i,1,(char)n);
	}
	return ret;
}
int aes_decrypt(char* in, char* key, char* out)
{
    if (!in || !key || !out)
    {
        return 0;
    }
 
    AES_KEY aes;
    if (AES_set_decrypt_key((unsigned char*)key, 128, &aes) < 0)
    {
        return 0;
    }
 
    int len = strlen(in), en_len = 0;
    while (en_len < len)
    {
        AES_decrypt((unsigned char*)in, (unsigned char*)out, &aes);
        in	+= AES_BLOCK_SIZE;
        out += AES_BLOCK_SIZE;
        en_len += AES_BLOCK_SIZE;
    }
 
    return 1;
}
int main()
{
    string miwen="665ed8c2cf3e650d6011f16e316656023bfe0dc41eae0f98f1dcce156ce48a9cf8df24d742e44745a567ed180be56926";
    
    string mi=hexStringToString(miwen);
    
    char msg[1024];
    strcpy(msg,mi.c_str());
    
    char   key[1024]="0401C0210ACA155517A795C22339A1CA705675CC13C7050A585FE7EEA5CCCF723AAC7A2BA1CBC259CA5489";

    
    char out[1024]="";
    
    aes_decrypt(msg,key,out);
    
    cout <<"对截获的密文进行解密:"<< out << endl;  
   
    
    
    
}
