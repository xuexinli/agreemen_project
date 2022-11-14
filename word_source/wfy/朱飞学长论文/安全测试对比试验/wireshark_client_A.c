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
int aes_encrypt(char* in, char* key, char* out)
{
    if (!in || !key || !out)
    {
        return 0;
    }
 
    AES_KEY aes;
    if (AES_set_encrypt_key((unsigned char*)key, 128, &aes) < 0)
    {
        return 0;
    }
 
    int len = strlen(in), en_len = 0;
 
    //输入输出字符串够长。而且是AES_BLOCK_SIZE的整数倍，须要严格限制
    while (en_len < len)
    {
        AES_encrypt((unsigned char*)in, (unsigned char*)out, &aes);
        in	+= AES_BLOCK_SIZE;
        out += AES_BLOCK_SIZE;
        en_len += AES_BLOCK_SIZE;
    }
 
    return 1;
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
void device_X(char X_hex[],BIGNUM *x,EC_KEY *key,EC_GROUP *group)
{
    EC_POINT *X;
    X=EC_POINT_new(group);//没new

    if(EC_POINT_mul(group,X,x,NULL,NULL,NULL)==0)
    {
        printf("some error occurs!");
    }

    char *temp_hex=EC_POINT_point2hex(group,X,EC_KEY_get_conv_form(key),NULL);
    strcpy(X_hex,temp_hex);
}


int main(int argc, char* argv[])

{
 EC_KEY *key1; //定义私钥种子
    //X=xG 公钥是椭圆曲线上的点
    EC_GROUP *group1;//定义椭圆曲线群
    int ret,nid,size,i,sig_len;
    EC_builtin_curve  *curves;
    BN_CTX *ctx;

    int crv_len;
    int bits = 80;
    int top =0;
    int bottom = 0;
    //生成key1和key2
    key1=EC_KEY_new();
    ctx=BN_CTX_new();//类似于一个种子,可以索引一个椭圆曲线
    if(key1==NULL)
    {
        printf("EC_KEY_new err!\n");
        return -1;
    }
    /* 获取实现的椭圆曲线个数 */

    crv_len = EC_get_builtin_curves(NULL, 0);

    curves = (EC_builtin_curve *)malloc(sizeof(EC_builtin_curve) * crv_len);

    /* 获取椭圆曲线列表 */

    EC_get_builtin_curves(curves, crv_len);

    /*

    nid=curves[0].nid;会有错误，原因是密钥太短

    */

    /* 选取一种椭圆曲线 */ 
    //25指定了椭圆曲线的类型 可以作为参数进行协商
    

    nid=curves[25].nid;

    /* 根据选择的椭圆曲线生成密钥参数group */

    group1=EC_GROUP_new_by_curve_name(nid);

    if(group1==NULL)

    {

        printf("EC_GROUP_new_by_curve_name err!\n");

        return -1;

    }

 

    /* 设置密钥参数 */

    ret=EC_KEY_set_group(key1,group1);

    if(ret!=1)

    {

        printf("EC_KEY_set_group err.\n");

        return -1;

    }

    /* 生成密钥 */

    ret=EC_KEY_generate_key(key1);

    if(ret!=1)

    {

        printf("EC_KEY_generate_key err.\n");

        return -1;

    }
    
    //设备A随机生成私钥
    BIGNUM *x_A=BN_new();
    BN_rand(x_A,bits,top,bottom);
    //计算K_A=x_A *G
    char *x_A_hex=BN_bn2hex(x_A);
    char K_A_hex[2048]={0};
    device_X(K_A_hex,x_A,key1,group1);
   
    //首先AB进行密钥协商
        int sockfd;
	struct sockaddr_in addr; // CA的地址
	if(argc != 3){fprintf(stderr, "usage:client ipaddr port");exit(1);} //接收参数
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0){fprintf(stderr, "Socket error");exit(1);} 
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[2]));
	if(inet_aton(argv[1], &addr.sin_addr) < 0){fprintf(stderr, "Inet_aton error");exit(1);}
  
 
     struct sockaddr_in c_addr;
    socklen_t addr_len = sizeof(c_addr);
    //将K_A发送给B
    if (sendto(sockfd, K_A_hex, strlen(K_A_hex), 0, (struct sockaddr *)&addr, sizeof(addr)) <= 0) {
        perror("sendto error.");
        return -1;
    }
    
    char K_CA_hex[1024];
    int K_CA_len = recvfrom(sockfd, K_CA_hex, sizeof(K_CA_hex) - 1, 0, (struct sockaddr *) &c_addr, &addr_len);
    if (K_CA_len < 0) {
        perror("recvfrom error.");
        return -1;
    }
    K_CA_hex[K_CA_len] = '\0';
    //printf("接受到CA发来的K_CA:%s\n",K_B_hex);
    EC_POINT *K_CA= EC_POINT_new(group1);
    EC_POINT_hex2point(group1,K_CA_hex, K_CA, ctx);
    EC_POINT *K_A_CA= EC_POINT_new(group1);
    EC_POINT_mul(group1,K_A_CA,NULL,K_CA,x_A,ctx);
     char *KACA_hex=EC_POINT_point2hex(group1,K_A_CA,EC_KEY_get_conv_form(key1),NULL);
     printf("协商后的密钥为:%s\n",KACA_hex);
     char key[128];
     strcpy(key,KACA_hex);
     
     BIGNUM *px_A=BN_new();
    //BN_rand(x_B,bits,top,bottom);
    //计算K_B=x_B *G
    
    px_A=(BIGNUM*)EC_KEY_get0_private_key(key1);

    
      
    char  encrypt_buffer[100] = {0}, decrypt_buffer[100] = {0};
    int len;
    //A将自己的私钥发送给CA
    //char *data = BN_bn2hex(px_A);
    char data[1024]="我的邮箱密码是Aa12345,请查收！";
    printf("A的私钥为：%s\n", data);
    
 
   
    len = strlen(data);
    //printf("明文长度：%d\n", len);
 
    aes_encrypt(data, key, encrypt_buffer);
    
    printf("加密后的密文为:%s\n",encrypt_buffer);
    
    //将加密结果发送给CA
    if (sendto(sockfd, encrypt_buffer, strlen(encrypt_buffer), 0, (struct sockaddr *)&c_addr, sizeof(c_addr)) <= 0) {
        perror("sendto error.");
        return -1;
    }
     
    
    aes_decrypt(encrypt_buffer, key, decrypt_buffer);
    printf("解密结果：%s\n", decrypt_buffer);










}
