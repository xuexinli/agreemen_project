#include <gtk/gtk.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <openssl/bn.h>
#include <time.h>
#include <openssl/ebcdic.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/crypto.h>
#include <time.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/objects.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/hmac.h>
#include <mysql/mysql.h>
#define SERVER_PORT 8080





struct data{
  GtkWidget *IP_addr;
  GtkWidget *PORT;
  GtkTextBuffer *buffer;
};
void Print_to_screen(gpointer entry, char *msg)
{
	GtkTextIter iter; int num;
    num = gtk_text_buffer_get_char_count((GtkTextBuffer *)(((struct data *)entry)->buffer)); 
    gtk_text_buffer_get_iter_at_offset((GtkTextBuffer *)(((struct data *)entry)->buffer), &iter, num);
    gtk_text_buffer_insert((GtkTextBuffer *)(((struct data *)entry)->buffer), &iter, msg, -1);
}
void Key_generator(EC_KEY *key, const EC_GROUP *group)
{
	EC_KEY_set_group(key,group);
	EC_KEY_generate_key(key);
}
void xX_generator(const BIGNUM **x, const EC_POINT **X, const EC_GROUP *group,BN_CTX *ctx)
{
	BIGNUM *temp = BN_new();
	EC_POINT *temp_d;
	temp_d = EC_POINT_new(group);
	BN_rand(temp, 160, -1, 1);
    	*x = temp;
	EC_POINT_mul(group, temp_d, *x, NULL, NULL, ctx);
	*X = temp_d;

}
void w_generator(BIGNUM *ID, const EC_POINT *X, const EC_GROUP *group, char w[1024])
{
    char *str;
    str = BN_bn2hex(ID);
    strcat(w, str);
    time_t t;
    time(&t);
    sprintf(str, "%ld", t);
    strcat(w, str);
    str = EC_POINT_point2hex(group, X, 2,NULL);
    strcat(w, str);
}
void pP_generator(const BIGNUM **p, const EC_POINT **P, BIGNUM **h,char w[1024] , const EC_GROUP *group, BN_CTX *ctx, const BIGNUM *private_key_c)
{
	
	char w_P[1024], h_str[64] = {0}, h_str_1[128] = {0},pp[2], *str;
	BIGNUM *n, *r;
	BIGNUM *temp = BN_new();
	int h_size = 0;
	n = BN_new();
	//DA选择随机值r
	r = BN_new();
	BN_rand(r, 160, -1, 1);
	//DA计算P=r * G
	EC_POINT *temp_d;
	temp_d = EC_POINT_new(group);
	EC_POINT_mul(group, temp_d, r, NULL, NULL, ctx);
	*P = temp_d;


	//DA计算h = H(w||P),使用SM3哈希算法
	strcpy(w_P, w);
	str = EC_POINT_point2hex(group, *P, 2,NULL);
	strcat(w_P, str);		
	EVP_MD_CTX *md_ctx;
	const EVP_MD *md;
    md = EVP_sm3();
    md_ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(md_ctx, md, NULL);
	EVP_DigestUpdate(md_ctx, w_P, strlen((char *)w_P));
    EVP_DigestFinal_ex(md_ctx, h_str, &h_size);
    EVP_MD_CTX_free(md_ctx);
	for (int i = 0; i < h_size; i++) 
	{
		sprintf(pp, "%hhx", h_str[i]);
		strcat(h_str_1, pp);
	}
	BN_hex2bn(h, h_str_1);
	//DA计算p = r + h * c(mod n)
	EC_GROUP_get_order(group, n, ctx);	
	BN_mod_mul(temp, *h, private_key_c, n, ctx); 
	BN_mod_add(temp, r, temp, n, ctx);
	*p = temp;
	BN_free(r);
	
}
_Bool ProveEqual(const BIGNUM *p, const EC_POINT *P, BIGNUM *h, const EC_POINT *public_key_C, const EC_GROUP *group, BN_CTX *ctx)
{
	EC_POINT *zuo, *you;
	zuo = EC_POINT_new(group);
	you = EC_POINT_new(group);
	EC_POINT_mul(group, you, NULL, public_key_C, h, ctx);	
	EC_POINT_add(group, you, P, you, ctx);
		
	EC_POINT_mul(group, zuo, p, NULL, NULL, ctx);
	
	if(!EC_POINT_cmp(group, zuo, you, ctx)) return 1;
	else return 0;
}
void Save_WPn(char w[1024], const EC_POINT *P, BIGNUM *n, const EC_GROUP *group, char *filename)
{
	char *str;
	FILE *LOCAL;
	LOCAL = fopen (filename,"w");
	str = EC_POINT_point2hex(group, P, 2,NULL);
	fprintf(LOCAL, "w = %s\nP = %s\n",w, str);
	str = BN_bn2hex(n);
	fprintf(LOCAL, "n = %s\n\n",str);
	fclose (LOCAL);	
		
}	
void Pre_keyVal(char K[1024], const BIGNUM *p, const EC_POINT *P, char w[1024], const EC_POINT *public_key_C, const BIGNUM *x, const EC_POINT *X, const EC_GROUP *group, BN_CTX *ctx)
{
	
	char w_P[1024] = {0}, h_str[64] = {0}, h_str_1[128] = {0},pp[2] = {0}, *str;
	int h_size = 0;
	EC_POINT *temp_d;
	temp_d = EC_POINT_new(group);
	BIGNUM *h = BN_new();
	//h = H(w||P),使用SM3哈希算法
	strcpy(w_P, w);
	str = EC_POINT_point2hex(group, P, 2,NULL);
	strcat(w_P, str);		
	EVP_MD_CTX *md_ctx;
	const EVP_MD *md;
    md = EVP_sm3();
    md_ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(md_ctx, md, NULL);
	EVP_DigestUpdate(md_ctx, w_P, strlen((char *)w_P));
    EVP_DigestFinal_ex(md_ctx, h_str, &h_size);
    EVP_MD_CTX_free(md_ctx);
	for (int i = 0; i < h_size; i++) 
	{
		sprintf(pp, "%hhx", h_str[i]);
		strcat(h_str_1, pp);
	}
	BN_hex2bn(&h, h_str_1);
	
	//计算K_AB的第一部分 
	EC_POINT_mul(group, temp_d, NULL, public_key_C, h, ctx);
	EC_POINT_add(group, temp_d, P, temp_d, ctx);
	EC_POINT_mul(group, temp_d, NULL, temp_d, p, ctx);
	str = EC_POINT_point2hex(group, temp_d, 2,NULL);
	strcat(K, str);	
	//计算K_AB的第二部分 	
	EC_POINT_mul(group, temp_d, NULL, X, x, ctx);	
	str = EC_POINT_point2hex(group, temp_d, 2,NULL);
	strcat(K, str);		
	

	
	
	//EC_POINT *temp_d;
	//temp_d = EC_POINT_new(group);
	
}
void KDF(char K[1024], char KDF_K[50])
{
	char p[2], temp[25] = {0};	
	PKCS5_PBKDF2_HMAC_SHA1(K, strlen(K), "salt", 4, 1005, 20, temp);
	for (int i = 0; i < 20; i++) 
	{
		sprintf(p, "%hhx", temp[i]);
		strcat(KDF_K, p);
	}
}
void Save_Local_Key_Tab(char KDF_K_AB[50], char *filename)
{
	FILE *LOCAL;
	LOCAL = fopen (filename,"w");
	fprintf(LOCAL, "sk = %s\n", KDF_K_AB);
	fclose(LOCAL);	
}
void Auth_Message_generator(char Auth_Message[128], char sk[50], char w_1[1024], const EC_POINT *P_1, char w_2[1024], const EC_POINT *P_2, BIGNUM *n_1, BIGNUM *n_2, const EC_GROUP *group) 
{
	char str[2500] = {0}, *temp, temp1[128] = {0};
	strcpy(str, w_1);
	temp = EC_POINT_point2hex(group, P_1, 2,NULL);
	strcat(str, temp);	
	strcat(str, w_2);
	temp = EC_POINT_point2hex(group, P_2, 2,NULL);
	strcat(str, temp);
	temp = BN_bn2hex(n_1);
	strcat(str, temp);
	temp = BN_bn2hex(n_2);
	strcat(str, temp);
	int size = 0;
	const EVP_MD *md;
    md = EVP_sm3();
	HMAC(md, sk, strlen(sk), str, strlen(str), temp1, &size);
	char p[2];
	for (int i = 0; i < size; i++) 
	{
		sprintf(p, "%hhx", temp1[i]);
		strcat(Auth_Message, p);
	}
	
}
void LK_generator(BIGNUM *LK, char K[1024], BIGNUM *n_1, BIGNUM *n_2)
{
	char str[1500] = {0}, *temp;
	char KDF_K[50] = {0};
	strcpy(str, K);
	temp = BN_bn2hex(n_1);
	strcat(str, temp);
	temp = BN_bn2hex(n_2);
	strcat(str, temp);	
	KDF(str,KDF_K);
	BN_hex2bn(&LK, KDF_K);
}
void show_key(gpointer entry, const BIGNUM *x_A, const EC_POINT *X_A, const EC_GROUP *group)
{
	char *str1111;
	str1111 = BN_bn2hex(x_A);
	Print_to_screen(entry, "CA's private key is ");
    Print_to_screen(entry, str1111);
	Print_to_screen(entry, ".\n");

	str1111 = EC_POINT_point2hex(group, X_A, 2,NULL);
	Print_to_screen(entry, "CA's public key is ");
    Print_to_screen(entry, str1111);
	Print_to_screen(entry, ".\n");

}
/* Predeployment Phase */ 
struct sockaddr_in udp_PrePhase(gpointer entry, int sockfd, char w_A[1024], const BIGNUM **p_A, const EC_POINT **P_A, const BIGNUM *private_key_c, const EC_POINT *public_key_C, const EC_GROUP *group1, BN_CTX *ctx)
{
	struct sockaddr_in addr_A;
	int addrlen_A = sizeof(addr_A);
	/*step1: CA接收由A发来的w_A，并验证时间的有效性*/
	recvfrom(sockfd, w_A, 1024, 0, (struct sockaddr *)&addr_A, &addrlen_A);
	char t_Ag[10] = {0};
	for(int i = 0; i < 10; i++) t_Ag[i] = w_A[i + 2];
    time_t t, t_A;
    time(&t);
    t_A = atol(t_Ag);
    if(t - t_A > 2)
    {
		Print_to_screen(entry, "Time out of sync\n");
        exit(0);
    }
    Print_to_screen(entry, "CA has received Identity String = ");
    Print_to_screen(entry, w_A);
    Print_to_screen(entry, ".\n");

	
	/*step2:DA为设备A生成第二部分的私钥p_A与公钥P_A*/
	BIGNUM *h_A;
	h_A = BN_new();
	pP_generator(p_A, P_A, &h_A, w_A, group1, ctx,private_key_c);
    Print_to_screen(entry, "CA has generated Second part of Public key and Private key.\n");
	char *str11; str11 = BN_bn2hex(*p_A);
	Print_to_screen(entry, "Second part of Private key = ");
    Print_to_screen(entry, str11);
    Print_to_screen(entry, ".\n");
	str11 = EC_POINT_point2hex(group1, *P_A, 2,NULL);
	Print_to_screen(entry, "Second part of Public key = ");
    Print_to_screen(entry, str11);
    Print_to_screen(entry, ".\n");

	/*step3: DA将p_A,P_A,public_key_C发给设备A */
	char *p_str_A; p_str_A = BN_bn2hex(*p_A);
	char *P_str_A; P_str_A = EC_POINT_point2hex(group1, *P_A, 2,NULL);
	char *public_key_C_str;	public_key_C_str = EC_POINT_point2hex(group1, public_key_C, 2,NULL);
	sendto(sockfd, p_str_A, strlen(p_str_A), 0, (struct sockaddr*)&addr_A, sizeof(addr_A));
	sendto(sockfd, P_str_A, strlen(P_str_A), 0, (struct sockaddr*)&addr_A, sizeof(addr_A));
	sendto(sockfd, public_key_C_str, strlen(public_key_C_str), 0, (struct sockaddr*)&addr_A, sizeof(addr_A));	
	Print_to_screen(entry, "CA sends the Second part of Public key and Private key.\n");
	return addr_A;
	
	
}
void Get_Repeat(MYSQL mysql, char w_A[1024], char w_B[1024], const BIGNUM **p_A, const EC_POINT **P_A, const BIGNUM **p_B, const EC_POINT **P_B, const BIGNUM **private_key_c, const EC_POINT **public_key_C, const EC_GROUP *group, BN_CTX *ctx)
{
	char *query_str = NULL;
	MYSQL_RES  *res = NULL;
	MYSQL_ROW  row;	
	char temp[1024] = {0};
	BIGNUM *temp_BN = BN_new();
	EC_POINT *temp_EC = EC_POINT_new(group);
	
	query_str = "SELECT val FROM CA_key WHERE name = \"w_A\";";
    mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	sprintf(temp, "%s", row[0]);
	strcpy(w_A, temp);

	query_str = "SELECT val FROM CA_key WHERE name = \"w_B\";";
    mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	sprintf(temp, "%s", row[0]);
	strcpy(w_B, temp);
	
	query_str = "SELECT val FROM CA_key WHERE name = \"p_A\";";
    mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	sprintf(temp, "%s", row[0]);		
	BN_hex2bn(&temp_BN, temp);
	*p_A = temp_BN;

	query_str = "SELECT val FROM CA_key WHERE name = \"p_B\";";
    mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	sprintf(temp, "%s", row[0]);	
	BN_hex2bn(&temp_BN, temp);
	*p_B = temp_BN;

	query_str = "SELECT val FROM CA_key WHERE name = \"private_key_c\";";
    mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	sprintf(temp, "%s", row[0]);	
	BN_hex2bn(&temp_BN, temp);	
	*private_key_c = temp_BN;
	
	query_str = "SELECT val FROM CA_key WHERE name = \"P_A\";";
    mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	sprintf(temp, "%s", row[0]);	
	EC_POINT_hex2point(group, temp, temp_EC, ctx);
	*P_A = temp_EC;

	query_str = "SELECT val FROM CA_key WHERE name = \"P_B\";";
    mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	sprintf(temp, "%s", row[0]);	
	EC_POINT_hex2point(group, temp, temp_EC, ctx);
	*P_B = temp_EC;

	query_str = "SELECT val FROM CA_key WHERE name = \"public_key_C\";";
    mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	sprintf(temp, "%s", row[0]);	
	EC_POINT_hex2point(group, temp, temp_EC, ctx);	
	*public_key_C = temp_EC;
}
void Save_Repeat(MYSQL mysql, char w_A[1024], char w_B[1024], const BIGNUM *p_A, const EC_POINT *P_A, const BIGNUM *p_B, const EC_POINT *P_B, const BIGNUM *private_key_c, const EC_POINT *public_key_C, const EC_GROUP *group, BN_CTX *ctx)
{
	char query_str[1024] = {0};
	MYSQL_RES  *res = NULL;
	int rc;
	char *temp;
	
	strcpy(query_str, "CREATE TABLE CA_key(name VARCHAR(20), val VARCHAR(1024));");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	
	memset(query_str, 0, 1024);
	strcpy(query_str, "INSERT INTO CA_key VALUES ('w_A','");	
	strcat(query_str, w_A); strcat(query_str, "');");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);

	memset(query_str, 0, 1024);	
	strcpy(query_str, "INSERT INTO CA_key VALUES ('w_B','");
	strcat(query_str, w_B); strcat(query_str, "');");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);

	temp = BN_bn2hex(p_A);
	memset(query_str, 0, 1024);
	strcpy(query_str, "INSERT INTO CA_key VALUES ('p_A','");		
	strcat(query_str, temp); strcat(query_str, "');");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);	

	temp = BN_bn2hex(p_B);
	memset(query_str, 0, 1024);
	strcpy(query_str, "INSERT INTO CA_key VALUES ('p_B','");
	strcat(query_str, temp); strcat(query_str, "');");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);	

	temp = BN_bn2hex(private_key_c);
	memset(query_str, 0, 1024);
	strcpy(query_str, "INSERT INTO CA_key VALUES ('private_key_c','");
	strcat(query_str, temp); strcat(query_str, "');");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);		
	
	temp = EC_POINT_point2hex(group, P_A, 2,NULL);
	memset(query_str, 0, 1024);
	strcpy(query_str, "INSERT INTO CA_key VALUES ('P_A','");
	strcat(query_str, temp); strcat(query_str, "');");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);

	temp = EC_POINT_point2hex(group, P_B, 2,NULL);
	memset(query_str, 0, 1024);
	strcpy(query_str, "INSERT INTO CA_key VALUES ('P_B','");	
	strcat(query_str, temp); strcat(query_str, "');");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	
	temp = EC_POINT_point2hex(group, public_key_C, 2,NULL);
	memset(query_str, 0, 1024);
	strcpy(query_str, "INSERT INTO CA_key VALUES ('public_key_C','");	
	strcat(query_str, temp); strcat(query_str, "');");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
}




void start_fuc(GtkWidget *widget, gpointer entry)
{
  
  const char *addr_str = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)(((struct data *)entry)->IP_addr)));//获得addr
  const char *port_str = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)(((struct data *)entry)->PORT)));//获得port
  
  /*创建一个 group1*/
  EC_KEY *key1 = EC_KEY_new();
  int crv_len = EC_get_builtin_curves(NULL, 0);
  EC_builtin_curve *curves = (EC_builtin_curve *)malloc(sizeof(EC_builtin_curve) * crv_len);
  EC_get_builtin_curves(curves, crv_len);
  const EC_GROUP *group1 = EC_GROUP_new_by_curve_name(curves[25].nid);
  Key_generator(key1, group1);


  /*创建一个UDP数据报类型的套接字*/	
  Print_to_screen(entry, "CA initialize Socket.\n");
  int sockfd;
  struct sockaddr_in addr; //CA地址 
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0){fprintf(stderr, "Socket error");exit(1);} 
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  inet_aton(addr_str,&addr.sin_addr);
  addr.sin_port = htons(SERVER_PORT);
  if(bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {fprintf(stderr, "Bind error");exit(1);}
  printf("12121\n");

  /*各种参数初始化*/
  BN_CTX *ctx = BN_CTX_new(); char w_A[1024] = {0}; 	const BIGNUM *p_A; const EC_POINT *P_A;	
  char w_B[1024] = {0}; 	const BIGNUM *p_B; const EC_POINT *P_B;	
  const BIGNUM *private_key_c;
  const EC_POINT *public_key_C;
  struct sockaddr_in addr_A, addr_B;
  struct sockaddr_in addr_t;
  int addrlen_t = sizeof(addr_t);

  

  /*初始化mysql*/
  MYSQL mysql;
  char *query_str = NULL;
  MYSQL_RES  *res = NULL;
  int rc;
  mysql_init(&mysql);
  if(mysql_real_connect(&mysql, NULL, "root", "123456", NULL, 0, NULL, 0))
	  Print_to_screen(entry, "CA Connected MySQL successful! \n");
  query_str = "use CA";
  mysql_real_query(&mysql, query_str, strlen(query_str));
  res = mysql_store_result(&mysql);
  query_str = "select * from CA_key;";
  rc = mysql_real_query(&mysql, query_str, strlen(query_str));
  res = mysql_store_result(&mysql);
  if(rc == 0) //如果是第二次启动
  {
	  Print_to_screen(entry, "CA starts for the second time.\n");
	  Print_to_screen(entry, "CA checks MySQL , and obtains previously saved data.\n");
	  Print_to_screen(entry, "CA provide address to A or B.\n");
      char addr_A_str[1024] = {0}, addr_B_str[1024] = {0};
      Get_Repeat(mysql, w_A, w_B, &p_A, &P_A, &p_B, &P_B, &private_key_c, &public_key_C, group1, ctx);
      /*接收从A发送过来的地址*/
      recvfrom(sockfd, addr_A_str, 1024, 0, (struct sockaddr *)&addr_t, &addrlen_t);
      memcpy(&addr_A,addr_A_str,sizeof(addr_A) + 1);
      /*接收从B发送过来的地址*/
      recvfrom(sockfd, addr_B_str, 1024, 0, (struct sockaddr *)&addr_t, &addrlen_t);
      memcpy(&addr_B,addr_B_str,sizeof(addr_B) + 1);
      /*CA给B发送A的地址*/
      sendto(sockfd, (char*)&addr_A, sizeof(addr_A) + 1, 0, (struct sockaddr*)&addr_B, sizeof(addr_B));
      /*关闭套接字*/
	  close(sockfd); 
      return;
	}
  Print_to_screen(entry, "CA starts for the First time.\n");
  /*获得DA(Domain Authority)的公钥C与私钥c*/ 
  private_key_c = EC_KEY_get0_private_key(key1); 
  public_key_C = EC_KEY_get0_public_key(key1);
  Print_to_screen(entry, "CA has generated its Public key and Private key.\n");
  show_key(entry, private_key_c, public_key_C, group1);

  Print_to_screen(entry, "********************************************CA communicates with A ********************************************\n");
  /*调用通信函数与A进行通信，需要得到w_A,p_A,P_A，但是需要输入sockfd,group1,ctx,private_key_c,public_key_C*/
  addr_A = udp_PrePhase(entry, sockfd, w_A, &p_A, &P_A, private_key_c, public_key_C, group1, ctx);
  Print_to_screen(entry, "CA saves A's parameters\n");
  Print_to_screen(entry, "********************************************CA communicates with A ********************************************\n");

  Print_to_screen(entry, "********************************************CA communicates with B ********************************************\n");
  /*调用通信函数与B进行通信，需要得到w_B,p_B,P_B，但是需要输入sockfd,group1,ctx,private_key_c,public_key_C*/
  addr_B = udp_PrePhase(entry, sockfd, w_B, &p_B, &P_B, private_key_c, public_key_C, group1, ctx);
  Print_to_screen(entry, "CA saves B's parameters\n");
  Print_to_screen(entry, "********************************************CA communicates with B ********************************************\n");

	
  /*服务器CA给B发送A的地址，以便B与A通信*/
  sendto(sockfd, (char*)&addr_A, sizeof(addr_A) + 1, 0, (struct sockaddr*)&addr_B, sizeof(addr_B));
  
  /*第一次运行保存所有内容*/
  Save_Repeat(mysql, w_A, w_B, p_A, P_A, p_B, P_B, private_key_c, public_key_C, group1, ctx);
	
  /*关闭套接字*/
  close(sockfd); 
 
  return;
}
static void activate (GtkApplication* app, gpointer user_data)
{
  //窗口初始化
  GtkWidget *window;
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "CA");
  gtk_window_set_default_size (GTK_WINDOW (window), 1000, 1000);

   
  //建立布局放入窗口中
  GtkWidget *grid = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(window), grid);

  //建立输出展示窗口且滚动
  GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_kinetic_scrolling((GtkScrolledWindow *)scrolledWindow, 1);
  GtkWidget *show = gtk_text_view_new();
  gtk_text_view_set_editable ((GtkTextView*)show,0);
  gtk_container_add(GTK_CONTAINER(scrolledWindow), show);
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (show)); 

  //设置展示窗口字体
  GtkCssProvider *provider = gtk_css_provider_new (); gtk_css_provider_load_from_data (provider,"textview {font: 20px serif;}",-1,NULL);
  GtkStyleContext *context = gtk_widget_get_style_context (show);
  gtk_style_context_add_provider (context,GTK_STYLE_PROVIDER (provider),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    
  //设置展示窗口初始文字
  gtk_text_buffer_set_text(buffer,"CA已打开\n",-1);
  gtk_box_pack_start (GTK_BOX (grid), scrolledWindow, TRUE, TRUE, 0);
  
  GtkWidget *IPaddr_Input = gtk_entry_new();//建立IP输入窗口
  GtkWidget *IP = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);//建立横向box
  GtkWidget *label_ip = gtk_label_new("请输入CA的IP地址：");//设置输入IP字符
  gtk_box_pack_start (GTK_BOX (IP), label_ip, FALSE, FALSE, 0); //将IP字符放入横向box
  gtk_box_pack_start (GTK_BOX (IP), IPaddr_Input, TRUE, TRUE, 0);//将IP输入窗口放入横向box
  gtk_box_pack_start(GTK_BOX (grid), IP, FALSE, FALSE, 0); //将横向box放入grid中
  GtkWidget *Port_Input = gtk_entry_new();//建立PORT输入窗口
  
  GtkWidget *Port = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);//建立横向box
  GtkWidget *label_port = gtk_label_new("请输入CAPort地址：");//设置输入Port字符
  gtk_box_pack_start (GTK_BOX (Port), label_port, FALSE, FALSE, 0); //将Port字符放入横向box
  gtk_box_pack_start (GTK_BOX (Port), Port_Input, TRUE, TRUE, 0);//将Port输入窗口放入横向box
  gtk_box_pack_start(GTK_BOX (grid), Port, FALSE, FALSE, 0); //将横向box放入grid中
  
  GtkWidget *button = gtk_button_new_with_label("START"); //建立开始按钮
  gtk_box_pack_start(GTK_BOX (grid), button, FALSE, FALSE, 0);//将开始按钮放入grid中
  
  struct data *DATA = (struct data *)malloc(10000);
  DATA->IP_addr = IPaddr_Input;
  DATA->PORT = Port_Input;
  DATA->buffer = buffer;
  
  g_signal_connect(button, "clicked", G_CALLBACK(start_fuc), (void *)DATA);//点击时开始工作
  
  
  gtk_widget_show_all(window);

}

int main (int argc, char **argv)
{


  GtkApplication *app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  int status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref(app);


  return status;
}
