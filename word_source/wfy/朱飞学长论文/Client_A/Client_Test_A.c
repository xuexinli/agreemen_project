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
  GtkWidget *page;
  GtkWidget *main_window;
};
struct data1{
  GtkWidget *page;
  GtkWidget *main_window;
  GtkWidget *subwindow;
  char w_A[1024];
  char sk_AB[50]; 
  char Auth_Message_A[128]; 
  char Auth_Message_B[128];
  const BIGNUM *LK_A; 
  int rc;
};
//用于修改颜色
void myCSS(void){
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;

    provider = gtk_css_provider_new ();
    display = gdk_display_get_default ();
    screen = gdk_display_get_default_screen (display);
    gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    const gchar *myCssFile = "/home/wu/桌面/gtk_add/end2/mystyle.css";
    GError *error = 0;

    gtk_css_provider_load_from_file(provider, g_file_new_for_path(myCssFile), &error);
    g_object_unref (provider);
}
void Print_to_screen(gpointer entry, char *msg)
{
	GtkTextIter iter; int num;
    num = gtk_text_buffer_get_char_count((GtkTextBuffer *)entry); 
    gtk_text_buffer_get_iter_at_offset((GtkTextBuffer *)entry, &iter, num);
    gtk_text_buffer_insert((GtkTextBuffer *)entry, &iter, msg, -1);
}
void show_hengxian(GtkWidget *box,GtkWidget *box1,GtkWidget *frame,GtkWidget *show,GtkTextBuffer *buffer,GtkCssProvider *provider, char *msg, char *path)
{
  //设置show为不可编辑
  gtk_text_view_set_editable ((GtkTextView*)show,0);
  //设置字体
  gtk_css_provider_load_from_data (provider,"textview {font: 20px serif;}",-1,NULL);
  GtkStyleContext *context = gtk_widget_get_style_context (show);
  gtk_style_context_add_provider (context,GTK_STYLE_PROVIDER (provider),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION); 
  //设置内容
  Print_to_screen(buffer, msg);
  //设置边框
  gtk_frame_set_shadow_type((GtkFrame *)frame,GTK_SHADOW_NONE);
  gtk_container_add(GTK_CONTAINER(frame), show);
  //放置图片
  GdkPixbuf *src = gdk_pixbuf_new_from_file(path, NULL);
  GdkPixbuf *dst = gdk_pixbuf_scale_simple(src,60,60,GDK_INTERP_BILINEAR);
  GtkWidget *pic= gtk_image_new_from_pixbuf(dst); 
  gtk_box_pack_start(GTK_BOX(box1),pic,0,0,0);
  gtk_box_pack_start(GTK_BOX(box1),frame,1,1,0);
  gtk_box_pack_start(GTK_BOX (box), box1, 0, 0, 0); 
  
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
void Save_WP(MYSQL mysql, char w[1024], const EC_POINT *P, const EC_GROUP *group)
{
	char query_str[1024] = {0};
	MYSQL_RES  *res = NULL;
	int rc;
	char *temp;
	strcpy(query_str, "CREATE TABLE Client_A_key(name VARCHAR(20), val VARCHAR(1024));");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	memset(query_str, 0, 1024);
	strcpy(query_str, "INSERT INTO Client_A_key VALUES ('w_B','");	
	strcat(query_str, w); strcat(query_str, "');");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	temp = EC_POINT_point2hex(group, P, 2,NULL);
	memset(query_str, 0, 1024);
	strcpy(query_str, "INSERT INTO Client_A_key VALUES ('P_B','");
	strcat(query_str, temp); strcat(query_str, "');");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
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
	
	printf("h_str_A = %s\n", h_str_1);

	
	
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
void Save_Local_Key_Tab(MYSQL mysql, char KDF_K_AB[50])
{
	char query_str[1024] = {0};
	MYSQL_RES  *res = NULL;
	int rc;
	
	strcpy(query_str, "INSERT INTO Client_A_key VALUES ('sk_AB','");	
	strcat(query_str, KDF_K_AB); strcat(query_str, "');");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	
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
void h_generator(char w[1024], char P_str[1024], BIGNUM **h)
{
	char w_P[1024], h_str[64] = {0}, h_str_1[128] = {0},pp[2], *str;
	BIGNUM *n, *r;
	int h_size = 0;

	//计算h = H(w||P),使用SM3哈希算法
	strcpy(w_P, w);
	strcat(w_P, P_str);		
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

}
void udp_PrePhase(int sockfd, struct sockaddr_in addr, const BIGNUM **p_A, const EC_POINT **P_A, const EC_POINT **public_key_C, char w_A[1024], const EC_GROUP *group1, BN_CTX *ctx)
{
	char command[2] = {0};
	/*step1:向CA发送w_A*/
	sendto(sockfd, w_A, strlen(w_A), 0, (struct sockaddr*)&addr, sizeof(addr));
	printf("A已将身份字符串发送给CA\n");
	
	/*step2:接收CA的p_A与P_A*/
	char p_str_A[1024] = {0}; char P_str_A[1024] = {0}; char public_key_C_str[1024] = {0};
	struct sockaddr_in addr_t;
    int addrlen_t = sizeof(addr_t);
	int addrlen = sizeof(addr);
	recvfrom(sockfd, p_str_A, 1024, 0, (struct sockaddr *)&addr_t, &addrlen_t);	
	recvfrom(sockfd, P_str_A, 1024, 0, (struct sockaddr *)&addr_t, &addrlen_t);
	recvfrom(sockfd, public_key_C_str, 1024, 0, (struct sockaddr *)&addr_t, &addrlen_t);
	BIGNUM *temp = BN_new(); BN_hex2bn(&temp, p_str_A);*p_A = temp;	
	EC_POINT *temp_d = EC_POINT_new(group1); EC_POINT_hex2point(group1, P_str_A, temp_d, ctx); *P_A = temp_d;	
	EC_POINT *temp_d_2 = EC_POINT_new(group1);EC_POINT_hex2point(group1, public_key_C_str, temp_d_2, ctx); *public_key_C = temp_d_2;
	printf("A已经收到CA发来的第二部分公私钥\n");


	/*step3:设备A验证p_A * G = P_A + h_A * C */
	BIGNUM *h_A	= BN_new(); 
	h_generator(w_A, P_str_A, &h_A);
	if(ProveEqual(*p_A, *P_A, h_A, *public_key_C, group1, ctx))
		printf("A验证第二部分公私钥的正确性：验证结果成功，预部署阶段完成\n");

}
struct sockaddr_in udp_Formal(char sk_AB[50], char Auth_Message_A[128], char Auth_Message_B[128], MYSQL mysql, BIGNUM **LK_A,const BIGNUM *x_A,const EC_POINT *X_A,char w_A[1024],const BIGNUM *p_A, const EC_POINT *P_A, int sockfd,const EC_POINT *public_key_C,const EC_GROUP *group1, BN_CTX *ctx)
{
	
	struct sockaddr_in addr_B;
	int addrlen_B = sizeof(addr_B);
	struct sockaddr_in addr_t;
    int addrlen_t = sizeof(addr_t);
	/*step1:设备A接收B发来的n_B,w_B,P_B并保存，并接受X_B*/ 
	char n_B_str[256] = {0}, P_str_B[512] = {0}, w_B[512] = {0}, X_str_B[512] = {0};
	recvfrom(sockfd, n_B_str, 256, 0, (struct sockaddr *)&addr_B, &addrlen_B);	
	recvfrom(sockfd, w_B, 512, 0, (struct sockaddr *)&addr_B, &addrlen_B);
	//char t_Bg[10] = {0};
    //for(int i = 0; i < 10; i++) t_Bg[i] = w_B[i + 4];
    //time_t t, t_B;
    //time(&t);
    //t_B = atol(t_Bg);
    //if(t - t_B > 2)
    //{
	//	Print_to_screen(entry, "Time out of sync\n");
    //    exit(0);
    //}
	recvfrom(sockfd, P_str_B, 512, 0, (struct sockaddr *)&addr_B, &addrlen_B);	
	recvfrom(sockfd, X_str_B, 512, 0, (struct sockaddr *)&addr_B, &addrlen_B);	
	BIGNUM *n_B = BN_new(); BN_hex2bn(&n_B, n_B_str);	
	EC_POINT *P_B = EC_POINT_new(group1); EC_POINT_hex2point(group1, P_str_B, P_B, ctx);
	EC_POINT *X_B = EC_POINT_new(group1); EC_POINT_hex2point(group1, X_str_B, X_B, ctx); 
	Save_WP(mysql, w_B, P_B, group1);
	printf("A已经收到了B发来的参数（随机数，身份字符串等）");
	//char *chan = " chan "; //sprintf(chan, "%ld\0",t-t_B);
    //Print_to_screen(entry, chan);
    printf("A保存这些参数\n");
	/*step2:设备A向B发送n_A,w_A,P_A, X_A*/
	BIGNUM *n_A = BN_new();
	BN_rand(n_A, 160, -1, 1);
	char *n_A_str = BN_bn2hex(n_A), *P_str_A = EC_POINT_point2hex(group1, P_A, 2,NULL),*X_str_A = EC_POINT_point2hex(group1, X_A, 2,NULL);	
	sendto(sockfd, n_A_str, strlen(n_A_str), 0, (struct sockaddr*)&addr_B, sizeof(addr_B));
	sendto(sockfd, w_A, strlen(w_A), 0, (struct sockaddr*)&addr_B, sizeof(addr_B));
	sendto(sockfd, P_str_A, strlen(P_str_A), 0, (struct sockaddr*)&addr_B, sizeof(addr_B));	
	sendto(sockfd, X_str_A, strlen(X_str_A), 0, (struct sockaddr*)&addr_B, sizeof(addr_B));
	printf("A已经向B发送他自己的身份字符串与随机数\n");
	
	/*step3:设备A检查时间一致性，并计算 preliminary key value K_AB;计算preliminary session key, sk = sk_AB = fai(K_AB);
			准备A的authentication message =  ψ[sk_AB, (w_A,P_A,w_B,P_B,n_A,n_B)]。*/ 
	char K_AB[1024] = {0};
	Pre_keyVal(K_AB, p_A, P_B, w_B, public_key_C, x_A, X_B, group1, ctx);
	KDF(K_AB, sk_AB);
	Save_Local_Key_Tab(mysql, sk_AB);
	Auth_Message_generator(Auth_Message_A, sk_AB, w_A, P_A, w_B, P_B, n_A, n_B, group1);
	printf("A计算键值与初步会话密钥，并保存初步会话密钥\nA的初步会话密钥 = %s\n",sk_AB);
	
	/*step4:设备A收到设备B的Auth_Message_B,设备A再自己算一下 Auth_Message_B_1，观察是否相等。*/ 
	recvfrom(sockfd, Auth_Message_B, 128, 0, (struct sockaddr *)&addr_t, &addrlen_t);
    printf("A已经收到B的认证字符串 = %s\n",Auth_Message_B);

	char Auth_Message_B_1[128] = {0};
	Auth_Message_generator(Auth_Message_B_1, sk_AB, w_B, P_B, w_A, P_A, n_B, n_A, group1);
	if(strcmp(Auth_Message_B_1, Auth_Message_B) == 0) printf("A认为B可信\n");
		else printf("A认为B不可信\n");
	
	/*step5:设备A将自己的authentication message发给B*/ 
	sendto(sockfd, Auth_Message_A, strlen(Auth_Message_A), 0, (struct sockaddr*)&addr_B, sizeof(addr_B));
    printf("A将自己的认证字符串发送给B，认证字符串 = %s\n",Auth_Message_A);

	/*step6:A生成会话密钥LK_A*/ 
	LK_generator(*LK_A, K_AB, n_A, n_B);	
	
	/*step7:A将K_AB保存*/
	char query_str[1024] = {0};
	MYSQL_RES  *res = NULL;
	int rc;
	memset(query_str, 0, 1024);	
	strcpy(query_str, "INSERT INTO Client_A_key VALUES ('K_AB','");
	strcat(query_str, K_AB); strcat(query_str, "');");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	char *str1212;str1212 = BN_bn2hex(*LK_A);
    printf("A生成会话密钥 = %s, A将会话密钥保存\n",str1212);
	
}
void Get_Repeat(MYSQL mysql, const BIGNUM **p_A, const BIGNUM **x_A, const EC_POINT **X_A,char w_A[1024], char w_B[1024], const EC_POINT **P_A, const EC_POINT **P_B, char K_AB[1024], char sk_AB[50], const EC_POINT **public_key_C, const EC_GROUP *group, BN_CTX *ctx)
{
	char *query_str = NULL;
	MYSQL_RES  *res = NULL;
	MYSQL_ROW  row;	
	char temp[1024] = {0};
	BIGNUM *temp_BN = BN_new();
	EC_POINT *temp_EC = EC_POINT_new(group);

	query_str = "SELECT val FROM Client_A_key WHERE name = \"p_A\";";
    mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	sprintf(temp, "%s", row[0]);	
	BN_hex2bn(&temp_BN, temp);
	*p_A = temp_BN;

	query_str = "SELECT val FROM Client_A_key WHERE name = \"x_A\";";
    mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	sprintf(temp, "%s", row[0]);	
	BN_hex2bn(&temp_BN, temp);
	*x_A = temp_BN;

	
	query_str = "SELECT val FROM Client_A_key WHERE name = \"X_A\";";
    mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	sprintf(temp, "%s", row[0]);	
	EC_POINT_hex2point(group, temp, temp_EC, ctx);
	*X_A = temp_EC;

	query_str = "SELECT val FROM Client_A_key WHERE name = \"w_A\";";
    mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	sprintf(temp, "%s", row[0]);
	strcpy(w_A, temp);

	query_str = "SELECT val FROM Client_A_key WHERE name = \"w_B\";";
    mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	sprintf(temp, "%s", row[0]);
	strcpy(w_B, temp);

	query_str = "SELECT val FROM Client_A_key WHERE name = \"P_A\";";
    mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	sprintf(temp, "%s", row[0]);	
	EC_POINT_hex2point(group, temp, temp_EC, ctx);
	*P_A = temp_EC;
	
	query_str = "SELECT val FROM Client_A_key WHERE name = \"P_B\";";
    mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	sprintf(temp, "%s", row[0]);	
	EC_POINT_hex2point(group, temp, temp_EC, ctx);
	*P_B = temp_EC;

	query_str = "SELECT val FROM Client_A_key WHERE name = \"K_AB\";";
    mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	sprintf(temp, "%s", row[0]);		
	strcpy(K_AB, temp);
	
	query_str = "SELECT val FROM Client_A_key WHERE name = \"sk_AB\";";
    mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	sprintf(temp, "%s", row[0]);		
	strcpy(sk_AB, temp);
	
	query_str = "SELECT val FROM Client_A_key WHERE name = \"public_key_C\";";
    mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	sprintf(temp, "%s", row[0]);	
	EC_POINT_hex2point(group, temp, temp_EC, ctx);
	*public_key_C = temp_EC;
	
}
_Bool udp_Formal_Repeat(int sockfd, BIGNUM **LK_A, char K_AB[1024], char w_A[1024], char w_B[1024], const EC_POINT *P_A, const EC_POINT *P_B, char sk_AB[50],const EC_GROUP *group, BN_CTX *ctx)
{
	struct sockaddr_in addr_B;
	int addrlen_B = sizeof(addr_B);
	struct sockaddr_in addr_t;
    int addrlen_t = sizeof(addr_t);

    /*step1:接收从B发来的n_B*/
    char n_B_str[256] = {0};
    recvfrom(sockfd, n_B_str, 256, 0, (struct sockaddr *)&addr_B, &addrlen_B);
	BIGNUM *n_B = BN_new(); BN_hex2bn(&n_B, n_B_str);
	printf("A已收到了B发来的随机数\n");
	
	
	/*step2:设备A向B发送n_A*/	
	BIGNUM *n_A = BN_new();
	BN_rand(n_A, 160, -1, 1);
	char *n_A_str = BN_bn2hex(n_A);
	sendto(sockfd, n_A_str, strlen(n_A_str), 0, (struct sockaddr*)&addr_B, sizeof(addr_B));	
    printf("A已给B发送新的随机数\n");

	/*step3:准备A的authentication message =  ψ[sk_AB, (w_A,P_A,w_B,P_B,n_A,n_B)]。*/
	char Auth_Message_A[128] = {0};
	Auth_Message_generator(Auth_Message_A, sk_AB, w_A, P_A, w_B, P_B, n_A, n_B, group);
	
	/*step4:设备A收到设备B的Auth_Message_B,设备A再自己算一下 Auth_Message_B_1，观察是否相等。*/
	printf("A收到B发来的验证消息，并验证其正确性\n");
	char Auth_Message_B[128] = {0};
	recvfrom(sockfd, Auth_Message_B, 128, 0, (struct sockaddr *)&addr_t, &addrlen_t);
	char Auth_Message_B_1[128] = {0};
	Auth_Message_generator(Auth_Message_B_1, sk_AB, w_B, P_B, w_A, P_A, n_B, n_A, group);
	if(strcmp(Auth_Message_B_1, Auth_Message_B) == 0) printf("A认为B可信\n");
		else { printf("A认为B不可信\n"); return 0;}

	
	/*step5:设备A将自己的authentication message发给B*/ 
	sendto(sockfd, Auth_Message_A, strlen(Auth_Message_A), 0, (struct sockaddr*)&addr_B, sizeof(addr_B));
	printf("A将自己的验证信息发给B\n");

	/*step6:A生成会话密钥LK_A*/
    LK_generator(*LK_A, K_AB, n_A, n_B);
    char *str1212;
    str1212 = BN_bn2hex(*LK_A);
    printf("新的会话密钥 = %s\n", str1212);
	return 1;

}
void Save_Repeat(MYSQL mysql, char w_A[1024], const BIGNUM *x_A, const EC_POINT *X_A, const BIGNUM *p_A, const EC_POINT *P_A, const EC_POINT *public_key_C,BIGNUM *LK_A, const EC_GROUP *group, BN_CTX *ctx)
{
	char query_str[1024] = {0};
	MYSQL_RES  *res = NULL;
	int rc;
	char *temp;
	
	memset(query_str, 0, 1024);
	strcpy(query_str, "INSERT INTO Client_A_key VALUES ('w_A','");	
	strcat(query_str, w_A); strcat(query_str, "');");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);

	temp = BN_bn2hex(x_A);
	memset(query_str, 0, 1024);
	strcpy(query_str, "INSERT INTO Client_A_key VALUES ('x_A','");		
	strcat(query_str, temp); strcat(query_str, "');");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);	

	temp = EC_POINT_point2hex(group, X_A, 2,NULL);
	memset(query_str, 0, 1024);
	strcpy(query_str, "INSERT INTO Client_A_key VALUES ('X_A','");
	strcat(query_str, temp); strcat(query_str, "');");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);

	temp = BN_bn2hex(p_A);
	memset(query_str, 0, 1024);
	strcpy(query_str, "INSERT INTO Client_A_key VALUES ('p_A','");		
	strcat(query_str, temp); strcat(query_str, "');");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);	

	temp = EC_POINT_point2hex(group, P_A, 2,NULL);
	memset(query_str, 0, 1024);
	strcpy(query_str, "INSERT INTO Client_A_key VALUES ('P_A','");
	strcat(query_str, temp); strcat(query_str, "');");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	
	temp = EC_POINT_point2hex(group, public_key_C, 2,NULL);
	memset(query_str, 0, 1024);
	strcpy(query_str, "INSERT INTO Client_A_key VALUES ('public_key_C','");	
	strcat(query_str, temp); strcat(query_str, "');");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
	
	temp = BN_bn2hex(LK_A);
	memset(query_str, 0, 1024);
	strcpy(query_str, "INSERT INTO Client_A_key VALUES ('LK_A','");		
	strcat(query_str, temp); strcat(query_str, "');");
	rc = mysql_real_query(&mysql, query_str, strlen(query_str));
	res = mysql_store_result(&mysql);
}
char *show_simplekey(char *key)
{
  char re[100] = {0};
  char *p = re;
  for(int i = 0; i < 3; i++) {re[i] = *(key + i);}
  for(int i = 3; i < 7; i++) {re[i] = '*';}
  for(int i = 0; i < 4; i++) {re[7 + i] = *(key + 6 + i);}
  re[11] = '\0';
  return p;
}

void Third2_page(GtkWidget *widget, gpointer entry)
{
  gtk_widget_destroy(GTK_WIDGET(((struct data1 *)entry)->subwindow));
  gtk_widget_show_all(((struct data1 *)entry)->subwindow);
  /*新建第3页*/
  GtkWidget *page3_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);//建立垂直布局page3_box
  //myCSS();gtk_widget_set_name(page3_box, "mypage");//设置page3颜色
  gtk_notebook_append_page (GTK_NOTEBOOK (((struct data1 *)entry)->page), page3_box, gtk_label_new("第3页"));
  gtk_widget_show_all(((struct data1 *)entry)->main_window);
  gtk_notebook_next_page(GTK_NOTEBOOK (((struct data1 *)entry)->page));
  /*设置密钥更新或密钥协商标题*/
  GtkWidget *title3_1=gtk_label_new("");
  GtkCssProvider *provider = gtk_css_provider_new();
  gtk_css_provider_load_from_data (provider,"textview {font: 20px serif;}",-1,NULL);
  /*获得rc*/
  int rc = (int )(((struct data1 *)entry)->rc);
  if(rc == 0)//如果是第二次启动
  {
      /*获得data1中的数据*/
      const BIGNUM *LK_A = (BIGNUM *)(((struct data1 *)entry)->LK_A);
      /*设置show3_box,表示密钥更新或密钥协商阶段过程*/
      GtkWidget *show3_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);gtk_widget_set_size_request(show3_box, 500, 220);//建立垂直布局show3_box 
      GtkWidget *show3_box_1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
      GdkPixbuf *src6 = gdk_pixbuf_new_from_file("/home/wu/桌面/gtk_add/6.2/arrow.png", NULL);
      GdkPixbuf *dst6 = gdk_pixbuf_scale_simple(src6,117,430.7,GDK_INTERP_BILINEAR);
      GtkWidget *arrow = gtk_image_new_from_pixbuf(dst6);
      GtkWidget *arrow_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
      gtk_box_pack_start(GTK_BOX (arrow_box), arrow, 0, 0, 0); 
      gtk_box_pack_start(GTK_BOX (show3_box_1), arrow_box, 0, 0, 0); 
      gtk_box_pack_start (GTK_BOX (show3_box_1), show3_box, 1, 1, 0); 
      GtkWidget *show3_box_2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);gtk_box_set_center_widget((GtkBox *)show3_box_2,show3_box_1);//将show3_box_1居中
      GtkWidget *show3 = gtk_text_view_new();gtk_text_view_set_editable ((GtkTextView*)show3,0);
      gtk_text_view_set_pixels_below_lines ((GtkTextView *)show3,40);
      GtkTextBuffer *buffer3 = gtk_text_view_get_buffer(GTK_TEXT_VIEW (show3)); 
      GtkStyleContext *context3 = gtk_widget_get_style_context (show3);
      gtk_style_context_add_provider (context3,GTK_STYLE_PROVIDER (provider),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
      GtkWidget *frame3 = gtk_frame_new (""); gtk_frame_set_shadow_type ((GtkFrame *)frame3,GTK_SHADOW_ETCHED_IN);
      //gtk_container_add(GTK_CONTAINER(frame3), show3);//加边框 
      gtk_box_pack_start (GTK_BOX (show3_box), show3, 1, 1, 0); 
      /*放入标题*/
      gtk_label_set_markup((GtkLabel *)title3_1,"<span font_desc=\"17.0\">密钥更新阶段</span>");//设置title3_1内容
      gtk_box_pack_start (GTK_BOX(page3_box), title3_1, 0, 0, 0);
      gtk_box_pack_start(GTK_BOX (page3_box), show3_box_2, 0, 0, 0);//将水平布局加入page3_box中
      
      /*显示show3,展示密钥更新阶段过程*/
      GtkTextIter iter; int num;
      gtk_text_buffer_create_tag(buffer3, "grey", "foreground", "grey",NULL);
      num = gtk_text_buffer_get_char_count(buffer3); 
      gtk_text_buffer_get_iter_at_offset(buffer3, &iter, num);
      gtk_text_buffer_insert_with_tags_by_name (buffer3, &iter, "A已给B发送新的随机数\n", -1, "grey",  NULL);
      num = gtk_text_buffer_get_char_count(buffer3); 
      gtk_text_buffer_get_iter_at_offset(buffer3, &iter, num);
      gtk_text_buffer_insert_with_tags_by_name (buffer3, &iter, "A已收到了B发来的随机数\n", -1, "grey",  NULL);
      num = gtk_text_buffer_get_char_count(buffer3); 
      gtk_text_buffer_get_iter_at_offset(buffer3, &iter, num);
      gtk_text_buffer_insert_with_tags_by_name (buffer3, &iter, "A收到B发来的验证消息，并验证其正确性\n", -1, "grey",  NULL);
      num = gtk_text_buffer_get_char_count(buffer3); 
      gtk_text_buffer_get_iter_at_offset(buffer3, &iter, num);
      gtk_text_buffer_insert_with_tags_by_name (buffer3, &iter, "A认为B可信\n", -1, "grey",  NULL);
      num = gtk_text_buffer_get_char_count(buffer3); 
      gtk_text_buffer_get_iter_at_offset(buffer3, &iter, num);
      gtk_text_buffer_insert_with_tags_by_name (buffer3, &iter, "A将自己的验证信息发给B\n", -1, "grey",  NULL);
      //Print_to_screen(buffer3, "A已给B发送新的随机数\n");
      //Print_to_screen(buffer3, "A已收到了B发来的随机数\n");
      //Print_to_screen(buffer3, "A收到B发来的验证消息，并验证其正确性\n");
      //Print_to_screen(buffer3, "A认为B可信\n");
      //Print_to_screen(buffer3, "A将自己的验证信息发给B\n");
      Print_to_screen(buffer3, "新的会话密钥 = ");
      num = gtk_text_buffer_get_char_count(buffer3); 
      gtk_text_buffer_get_iter_at_offset(buffer3, &iter, num);
      gtk_text_buffer_create_tag(buffer3, "bold", "weight", PANGO_WEIGHT_BOLD, NULL);
      gtk_text_buffer_insert_with_tags_by_name (buffer3, &iter, BN_bn2hex(LK_A), -1, "bold",  NULL);
      Print_to_screen(buffer3, "\n");
      gtk_widget_show_all(((struct data1 *)entry)->main_window);

      return;
  }
  /*获得data1中的数据*/
  char sk_AB[50];strcpy(sk_AB,((struct data1 *)entry)->sk_AB);
  char Auth_Message_A[128];strcpy(Auth_Message_A,((struct data1 *)entry)->Auth_Message_A);
  char Auth_Message_B[128];strcpy(Auth_Message_B,((struct data1 *)entry)->Auth_Message_B);
  const BIGNUM *LK_A = (BIGNUM *)(((struct data1 *)entry)->LK_A);
  /*代码：与Client_A的密钥协商阶段*/
  /*设置show3_box,表示密钥更新或密钥协商阶段过程*/
  GtkWidget *show3_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);gtk_widget_set_size_request(show3_box, 800, 290);//建立垂直布局show3_box 
  GtkWidget *show3_box_1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GdkPixbuf *src6 = gdk_pixbuf_new_from_file("/home/wu/桌面/gtk_add/6.2/arrow1.png", NULL);
  GdkPixbuf *dst6 = gdk_pixbuf_scale_simple(src6,127,587,GDK_INTERP_BILINEAR);
  GtkWidget *arrow = gtk_image_new_from_pixbuf(dst6);
  GtkWidget *arrow_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX (arrow_box), arrow, 0, 0, 0); 
  gtk_box_pack_start (GTK_BOX (show3_box_1), arrow_box, 1, 1, 0); 
  gtk_box_pack_start (GTK_BOX (show3_box_1), show3_box, 1, 1, 0); 
  GtkWidget *show3_box_2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);gtk_box_set_center_widget((GtkBox *)show3_box_2,show3_box_1);//将show3_box_1居中
  GtkWidget *show3 = gtk_text_view_new();gtk_text_view_set_editable ((GtkTextView*)show3,0);
  gtk_text_view_set_pixels_below_lines ((GtkTextView *)show3,40);
  GtkTextBuffer *buffer3 = gtk_text_view_get_buffer(GTK_TEXT_VIEW (show3)); 
  GtkStyleContext *context3 = gtk_widget_get_style_context (show3);
  gtk_style_context_add_provider (context3,GTK_STYLE_PROVIDER (provider),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  GtkWidget *frame3 = gtk_frame_new (""); gtk_frame_set_shadow_type ((GtkFrame *)frame3,GTK_SHADOW_ETCHED_IN);
  //gtk_container_add(GTK_CONTAINER(frame3), show3);//加边框 
  gtk_box_pack_start (GTK_BOX (show3_box), show3, 1, 1, 0); 
  /*放入标题*/
  gtk_label_set_markup((GtkLabel *)title3_1,"<span font_desc=\"17.0\">密钥协商阶段</span>");//设置title3_1内容
  gtk_box_pack_start (GTK_BOX(page3_box), title3_1, 0, 0, 0);
  gtk_box_pack_start(GTK_BOX (page3_box), show3_box_2, 0, 0, 0);//将垂直布局加入page3_box中

  //udp_Formal
  GtkTextIter iter; int num; 
  gtk_text_buffer_create_tag(buffer3, "grey", "foreground", "grey",NULL);
  num = gtk_text_buffer_get_char_count(buffer3); 
  gtk_text_buffer_get_iter_at_offset(buffer3, &iter, num);
  gtk_text_buffer_insert_with_tags_by_name (buffer3, &iter, "A已经收到了B发来的参数（随机数，身份字符串等）,A保存这些参数\n", -1, "grey",  NULL);
  num = gtk_text_buffer_get_char_count(buffer3); 
  gtk_text_buffer_get_iter_at_offset(buffer3, &iter, num);
  gtk_text_buffer_insert_with_tags_by_name (buffer3, &iter, "A已经向B发送他自己的身份字符串与随机数\n", -1, "grey",  NULL);
  num = gtk_text_buffer_get_char_count(buffer3); 
  gtk_text_buffer_get_iter_at_offset(buffer3, &iter, num);
  gtk_text_buffer_insert_with_tags_by_name (buffer3, &iter, "A计算键值与初步会话密钥，并保存初步会话密钥\nA的初步会话密钥 = ", -1, "grey",  NULL);
  num = gtk_text_buffer_get_char_count(buffer3); 
  gtk_text_buffer_get_iter_at_offset(buffer3, &iter, num);
  gtk_text_buffer_insert_with_tags_by_name (buffer3, &iter, sk_AB, -1, "grey",  NULL);
  num = gtk_text_buffer_get_char_count(buffer3); 
  gtk_text_buffer_get_iter_at_offset(buffer3, &iter, num);
  gtk_text_buffer_insert_with_tags_by_name (buffer3, &iter, ".\nA已经收到B的认证字符串 = ", -1, "grey",  NULL);
  num = gtk_text_buffer_get_char_count(buffer3); 
  gtk_text_buffer_get_iter_at_offset(buffer3, &iter, num);
  gtk_text_buffer_insert_with_tags_by_name (buffer3, &iter, Auth_Message_B, -1, "grey",  NULL);
  num = gtk_text_buffer_get_char_count(buffer3); 
  gtk_text_buffer_get_iter_at_offset(buffer3, &iter, num);
  gtk_text_buffer_insert_with_tags_by_name (buffer3, &iter, ".\nA检查正确性，最终A认为B可信\nA将认证字符串发送给B，字符串 = ", -1, "grey",  NULL); 
  num = gtk_text_buffer_get_char_count(buffer3); 
  gtk_text_buffer_get_iter_at_offset(buffer3, &iter, num);
  gtk_text_buffer_insert_with_tags_by_name (buffer3, &iter, Auth_Message_A, -1, "grey",  NULL);      
      
  //Print_to_screen(buffer3, "A已经收到了B发来的参数（随机数，身份字符串等）,");
  //Print_to_screen(buffer3, "A保存这些参数\n");
  //Print_to_screen(buffer3, "A已经向B发送他自己的身份字符串与随机数\n");
  //Print_to_screen(buffer3, "A计算键值与初步会话密钥，并保存初步会话密钥\nA的初步会话密钥 = ");
  //Print_to_screen(buffer3, sk_AB); Print_to_screen(buffer3, ".\n");
  //Print_to_screen(buffer3, "A已经收到B的认证字符串 = ");
  //Print_to_screen(buffer3, Auth_Message_B); Print_to_screen(buffer3, ".\nA检查正确性，最终");
  //Print_to_screen(buffer3, "A认为B可信\n");
  //Print_to_screen(buffer3, "A将认证字符串发送给B，字符串 = "); 
  //Print_to_screen(buffer3, Auth_Message_A); Print_to_screen(buffer3, " \n"); 
  Print_to_screen(buffer3, "\nA生成会话密钥 = ");
  num = gtk_text_buffer_get_char_count(buffer3); 
  gtk_text_buffer_get_iter_at_offset(buffer3, &iter, num);
  gtk_text_buffer_create_tag(buffer3, "bold", "weight", PANGO_WEIGHT_BOLD, NULL);
  gtk_text_buffer_insert_with_tags_by_name (buffer3, &iter, BN_bn2hex(LK_A), -1, "bold",  NULL);
  Print_to_screen(buffer3, "\n");
  gtk_widget_show_all(((struct data1 *)entry)->main_window);
  return;

}

void Third_page(GtkWidget *widget, gpointer entry)
{
  //赤裸裸的展示
  /*窗口：正在与Client_B进行密钥协商*/
  GtkWidget *subwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (subwindow), "密钥协商阶段");
  gtk_window_set_default_size (GTK_WINDOW (subwindow), 400, 200);
  GtkWidget *subbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);gtk_container_add(GTK_CONTAINER(subwindow), subbox);
  GtkWidget *sublabel = gtk_label_new(""); 
  gtk_label_set_markup((GtkLabel *)sublabel,"<span font_desc=\"14.0\">正在与Client_B协商密钥....</span>");//设置sublabel内容 
  gtk_box_pack_start(GTK_BOX(subbox),sublabel,1,1,0);
  GtkWidget *subbutton = gtk_button_new_with_label("确定"); gtk_box_pack_start(GTK_BOX(subbox),subbutton,0,0,0);
  ((struct data1 *)entry)->subwindow = subwindow;
  g_signal_connect(G_OBJECT(subbutton), "clicked",G_CALLBACK(Third2_page), entry);
  gtk_widget_show_all(subwindow); 
  
  
}
void Second_page(GtkWidget *widget, gpointer entry)
{
  /*获得IP与PORT*/
  const char *IP = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)(((struct data *)entry)->IP_addr)));
  const char *PORT = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)(((struct data *)entry)->PORT)));
  
  /*我们要把所有的事情全部办完*/
  /*预部署阶段*/
  /*创建一个 group1*/
  EC_KEY *key1 = EC_KEY_new(); int crv_len = EC_get_builtin_curves(NULL, 0);
  EC_builtin_curve *curves = (EC_builtin_curve *)malloc(sizeof(EC_builtin_curve) * crv_len);
  EC_get_builtin_curves(curves, crv_len); 
  const EC_GROUP *group1 = EC_GROUP_new_by_curve_name(curves[25].nid); Key_generator(key1, group1);
  /*创建一个UDP数据报类型的套接字*/
  int sockfd; struct sockaddr_in addr;  struct sockaddr_in addr_A; bzero(&addr_A, sizeof(addr_A));//A与CA的地址 
  sockfd = socket(AF_INET, SOCK_DGRAM, 0); if(sockfd < 0){fprintf(stderr, "Socket error");exit(1);} 
  bzero(&addr, sizeof(addr)); addr.sin_family = AF_INET; addr.sin_port = htons(atoi(PORT));
  if(inet_aton(IP, &addr.sin_addr) < 0){fprintf(stderr, "Inet_aton error");exit(1);}
  /*配置A的地址*/
  addr_A.sin_family = AF_INET; inet_aton("127.0.0.2",&addr_A.sin_addr); addr_A.sin_port = htons(8080);
  if(bind(sockfd, (struct sockaddr*)&addr_A, sizeof(addr_A)) < 0) {fprintf(stderr, "Bind error");exit(1);}
  /*各种参数初始化*/
  BN_CTX *ctx = BN_CTX_new(); const BIGNUM *x_A; const EC_POINT *X_A; char w_A[1024] = {0};	
  const BIGNUM *p_A; const EC_POINT *P_A; const EC_POINT *public_key_C;	
  BIGNUM *LK_A = BN_new(); struct sockaddr_in addr_B; int addrlen_B = sizeof(addr_B);
  char sk_AB[50] = {0}; char Auth_Message_A[128] = {0}; char Auth_Message_B[128] = {0};
  /*初始化mysql*/
  MYSQL mysql; char *query_str = NULL; MYSQL_RES  *res = NULL; int rc; mysql_init(&mysql);
  if(mysql_real_connect(&mysql, NULL, "root", "123456", NULL, 0, NULL, 0))
	  printf("A成功链接上Mysql\n");
  query_str = "use Client_A"; mysql_real_query(&mysql, query_str, strlen(query_str)); res = mysql_store_result(&mysql);
  query_str = "select * from Client_A_key;"; rc = mysql_real_query(&mysql, query_str, strlen(query_str)); res = mysql_store_result(&mysql);

  /*新建第2页*/
  GtkWidget *page2_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);//建立垂直布局page2_box
  myCSS();gtk_widget_set_name(page2_box, "mypage");//设置page2颜色
  gtk_notebook_append_page (GTK_NOTEBOOK (((struct data *)entry)->page), page2_box, gtk_label_new("第2页"));
  gtk_widget_show_all(((struct data *)entry)->main_window);
  gtk_notebook_next_page(GTK_NOTEBOOK (((struct data *)entry)->page));
  /*显示预部署标题*/
  GdkPixbuf *src4 = gdk_pixbuf_new_from_file("/home/wu/桌面/gtk_add/6.2/grey.png", NULL);
  GdkPixbuf *dst4 = gdk_pixbuf_scale_simple(src4,200,50,GDK_INTERP_BILINEAR);
  GtkWidget *blank = gtk_image_new_from_pixbuf(dst4);
  gtk_box_pack_start (GTK_BOX (page2_box), blank, 0, 0, 0);
  GtkWidget *title1_1=gtk_label_new("");
  gtk_label_set_markup((GtkLabel *)title1_1,"<span font_desc=\"19.0\">预部署阶段</span>");//设置title1_1内容
  gtk_box_pack_start (GTK_BOX (page2_box), title1_1, 0, 0, 0);
  /*显示show1_box,表示预部署阶段过程*/
  GtkWidget *show1_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);gtk_widget_set_size_request(show1_box, 800, 300);//建立垂直布局show1_box 
  GtkWidget *show1_box_1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);gtk_box_set_center_widget((GtkBox *)show1_box_1,show1_box);//将show1_box居中
  gtk_box_pack_start(GTK_BOX (page2_box), show1_box_1, 0, 0, 0);//将垂直布局加入page2_box中
  GtkCssProvider *provider = gtk_css_provider_new();
  /*设置title2_1*/
  GtkWidget *title2_1=gtk_label_new("");
  GtkWidget *title_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start(GTK_BOX (title_box), title2_1, 0, 0, 0);
  if(rc == 0)//如果是第二次启动
  {
      char addr_B_str[1024] = {0}, w_B[1024] = {0}, K_AB[1024] = {0};
      const EC_POINT *P_B;
      /*获得第一次启动时的各种参数*/
      Get_Repeat(mysql, &p_A, &x_A, &X_A, w_A, w_B, &P_A, &P_B, K_AB, sk_AB, &public_key_C, group1, ctx);
      /*设备A将自己的地址发送给CA*/
      sendto(sockfd, (char*)&addr_A, sizeof(addr_A) + 1, 0, (struct sockaddr*)&addr, sizeof(addr));
      /*A与B之间进行第二次通信*/
      if(udp_Formal_Repeat(sockfd, &LK_A, K_AB, w_A, w_B, P_A, P_B, sk_AB, group1, ctx) == 0)
      {
		  printf("Repeat failed\n");
    	  return;
	  } 

      /*事情已经全部办完了，我们要将其展示在图形化界面中*/
      GtkWidget *show1_2 = gtk_text_view_new();//建立show1_box中的第2个窗口show1_2
      GtkTextBuffer *buffer1_2 = gtk_text_view_get_buffer(GTK_TEXT_VIEW (show1_2));//将buffer1_2连接到show1_2上  
      GtkWidget *frame1_2 = gtk_frame_new ("");//初始化边框frame1_2
      GtkWidget *show1_2_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);//建立横向show1_2_box
      show_hengxian(show1_box,show1_2_box,frame1_2,show1_2,buffer1_2,provider, "A的预部署阶段\t\t\t\t\t\t\t\t\t\t\t\t\t已完成","/home/wu/桌面/gtk_add/6.2/finish.png");
      GtkWidget *show1_3 = gtk_text_view_new();//建立show1_box中的第3个窗口show1_3
      GtkTextBuffer *buffer1_3 = gtk_text_view_get_buffer(GTK_TEXT_VIEW (show1_3));//将buffer1_3连接到show1_3上  
      GtkWidget *frame1_3 = gtk_frame_new ("");//初始化边框frame1_3
      GtkWidget *show1_3_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);//建立横向show1_3_box
      show_hengxian(show1_box,show1_3_box,frame1_3,show1_3,buffer1_3,provider, "A的公钥\t\t\t\t\t\t\t\t\t\t\t\t\t\t","/home/wu/桌面/gtk_add/6.2/lock.png");    
      Print_to_screen(buffer1_3, show_simplekey(EC_POINT_point2hex(group1, X_A, 2,NULL)));
      GtkWidget *show1_4 = gtk_text_view_new();//建立show1_box中的第4个窗口show1_4
      GtkTextBuffer *buffer1_4 = gtk_text_view_get_buffer(GTK_TEXT_VIEW (show1_4));//将buffer1_4连接到show1_4上  
      GtkWidget *frame1_4 = gtk_frame_new ("");//初始化边框frame1_4
      GtkWidget *show1_4_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);//建立横向show1_4_box
      show_hengxian(show1_box,show1_4_box,frame1_4,show1_4,buffer1_4,provider, "A的私钥\t\t\t\t\t\t\t\t\t\t\t\t\t\t","/home/wu/桌面/gtk_add/6.2/key.png");    
      Print_to_screen(buffer1_4, show_simplekey(BN_bn2hex(x_A)));           	
      gtk_widget_show_all(((struct data *)entry)->main_window); //将show1显示出来
      /*显示show2,显示当前无人机列表*/
      gtk_label_set_markup((GtkLabel *)title2_1,"<span font_desc=\"14.0\">\t\t\t\t  已部署的无人机有</span>");//设置title1_1内容
      gtk_box_pack_start (GTK_BOX (page2_box), title_box, 0, 0, 0);
      GtkWidget *show2_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);gtk_widget_set_size_request(show2_box, 800, 300);//建立垂直布局show2_box 
      GtkWidget *show2_box_1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);gtk_box_set_center_widget((GtkBox *)show2_box_1,show2_box);//将show2_box居中
      gtk_box_pack_start(GTK_BOX (page2_box), show2_box_1, 0, 0, 0);//将垂直布局加入page2_box中
      GtkWidget *show2_1 = gtk_text_view_new();//建立show2_box中的第1个窗口show2_1
      GtkTextBuffer *buffer2_1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW (show2_1));//将buffer2_1连接到show2_1上  
      GtkWidget *frame2_1 = gtk_frame_new ("");//初始化边框frame2_1
      GtkWidget *show2_1_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);//建立横向show2_1_box
      show_hengxian(show2_box,show2_1_box,frame2_1,show2_1,buffer2_1,provider, "无人机A","/home/wu/桌面/gtk_add/6.2/UAV.png");
      GtkWidget *show2_2 = gtk_text_view_new();//建立show2_box中的第2个窗口show2_2
      GtkTextBuffer *buffer2_2 = gtk_text_view_get_buffer(GTK_TEXT_VIEW (show2_2));//将buffer2_2连接到show2_2上  
      GtkWidget *frame2_2 = gtk_frame_new ("");//初始化边框frame2_2
      GtkWidget *show2_2_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);//建立横向show2_2_box
      show_hengxian(show2_box,show2_2_box,frame2_2,show2_2,buffer2_2,provider, "无人机B","/home/wu/桌面/gtk_add/6.2/UAV.png");
      gtk_widget_show_all(((struct data *)entry)->main_window); //将show2显示出来
      /*构建data1*/
      struct data1 *DATA1 = (struct data1 *)malloc(15000);
      DATA1->page = ((struct data *)entry)->page; DATA1->main_window = ((struct data *)entry)->main_window;
      DATA1->rc = rc; DATA1->LK_A = LK_A;
      /*选择通信对方*/
      GtkWidget *choo_box=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);gtk_box_pack_start(GTK_BOX(page2_box),choo_box,0,0, 20);//建立横向choo_box并放入page2_box中
      GdkPixbuf *src5 = gdk_pixbuf_new_from_file("/home/wu/桌面/gtk_add/6.2/com_obj.png", NULL);
      GdkPixbuf *dst5 = gdk_pixbuf_scale_simple(src5,55,55,GDK_INTERP_BILINEAR);
      GtkWidget *com_obj = gtk_image_new_from_pixbuf(dst5);
      GtkWidget*choo_box_1=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
      gtk_box_pack_start(GTK_BOX(choo_box_1), com_obj,1,1,0);
      GtkWidget *choo_Input = gtk_entry_new();gtk_entry_set_placeholder_text(GTK_ENTRY (choo_Input), "请选择通信对象");
      gtk_box_pack_start(GTK_BOX(choo_box_1), choo_Input,1,1,0);//建立choo_Input窗口并放入choo_box_1中
      GtkWidget *choo_button=gtk_button_new_with_label("确定");gtk_box_pack_start(GTK_BOX(choo_box_1), choo_button,1,1,0);
      gtk_box_set_center_widget((GtkBox *)choo_box,choo_box_1);
      g_signal_connect(choo_button, "clicked", G_CALLBACK(Third_page), (void *)DATA1);//点击确定时，进入Third_page
      gtk_widget_show_all(((struct data *)entry)->main_window);
      /*关闭套接字*/
      close(sockfd); 
      return;

  }

  /*设备A取随机值x_A并将其作为私钥的第一部分，并计算相应的公钥X_A*/
  xX_generator(&x_A, &X_A, group1, ctx);
  /*设备A的ID_A = 0001(十进制)(ID在0-255之间),设备A计算w_A = (ID_A||X_A)*/
  BIGNUM *ID_A = BN_new(); BN_set_word(ID_A, 0001); w_generator(ID_A, X_A, group1, w_A);
  /*调用通信函数与CA进行通信,需要得到p_A,P_A,public_key_C,但是需要输入sockfd,w_A,group1,ctx*/
  udp_PrePhase(sockfd, addr, &p_A, &P_A, &public_key_C, w_A, group1, ctx);
  /*A与B相互认证，协商密钥*/ 
  addr_B = udp_Formal(sk_AB, Auth_Message_A, Auth_Message_B,mysql, &LK_A,x_A,X_A,w_A, p_A,P_A,sockfd,public_key_C,group1,ctx);
  /*第一次运行保存所有内容*/
  Save_Repeat(mysql, w_A, x_A, X_A, p_A, P_A, public_key_C, LK_A, group1, ctx);


  /*事情已经全部办完了，我们要将其展示在图形化界面中*/ 
  GtkWidget *show1_2 = gtk_text_view_new();//建立show1_box中的第2个窗口show1_2
  GtkTextBuffer *buffer1_2 = gtk_text_view_get_buffer(GTK_TEXT_VIEW (show1_2));//将buffer1_2连接到show1_2上  
  GtkWidget *frame1_2 = gtk_frame_new ("");//初始化边框frame1_2
  GtkWidget *show1_2_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);//建立横向show1_2_box
  show_hengxian(show1_box,show1_2_box,frame1_2,show1_2,buffer1_2,provider, "A的预部署阶段\t\t\t\t\t\t\t\t\t\t\t\t\t刚完成","/home/wu/桌面/gtk_add/6.2/finish.png");
  GtkWidget *show1_3 = gtk_text_view_new();//建立show1_box中的第3个窗口show1_3
  GtkTextBuffer *buffer1_3 = gtk_text_view_get_buffer(GTK_TEXT_VIEW (show1_3));//将buffer1_3连接到show1_3上  
  GtkWidget *frame1_3 = gtk_frame_new("");//初始化边框frame1_3
  GtkWidget *show1_3_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);//建立横向show1_3_box
  show_hengxian(show1_box,show1_3_box,frame1_3,show1_3,buffer1_3,provider, "A的公钥\t\t\t\t\t\t\t\t\t\t\t\t\t\t","/home/wu/桌面/gtk_add/6.2/lock.png");    
  Print_to_screen(buffer1_3, show_simplekey(EC_POINT_point2hex(group1, X_A, 2,NULL)));
  GtkWidget *show1_4 = gtk_text_view_new();//建立show1_box中的第4个窗口show1_4
  GtkTextBuffer *buffer1_4 = gtk_text_view_get_buffer(GTK_TEXT_VIEW (show1_4));//将buffer1_4连接到show1_4上  
  GtkWidget *frame1_4 = gtk_frame_new("");//初始化边框frame1_4
  GtkWidget *show1_4_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);//建立横向show1_4_box
  show_hengxian(show1_box,show1_4_box,frame1_4,show1_4,buffer1_4,provider, "A的私钥\t\t\t\t\t\t\t\t\t\t\t\t\t\t","/home/wu/桌面/gtk_add/6.2/key.png");    
  Print_to_screen(buffer1_4, show_simplekey(BN_bn2hex(x_A)));       
  gtk_widget_show_all(((struct data *)entry)->main_window);

  /*显示show2,显示当前无人机列表*/
  gtk_label_set_markup((GtkLabel *)title2_1,"<span font_desc=\"14.0\">\t\t\t\t  已部署的无人机有</span>");//设置title1_1内容
  gtk_box_pack_start (GTK_BOX (page2_box), title_box, 0, 0, 0);
  GtkWidget *show2_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);gtk_widget_set_size_request(show2_box, 800, 300);//建立垂直布局show2_box 
  GtkWidget *show2_box_1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);gtk_box_set_center_widget((GtkBox *)show2_box_1,show2_box);//将show2_box居中
  gtk_box_pack_start(GTK_BOX (page2_box), show2_box_1, 0, 0, 0);//将垂直布局加入page2_box中
  GtkWidget *show2_1 = gtk_text_view_new();//建立show2_box中的第1个窗口show2_1
  GtkTextBuffer *buffer2_1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW (show2_1));//将buffer2_1连接到show2_1上  
  GtkWidget *frame2_1 = gtk_frame_new ("");//初始化边框frame2_1
  GtkWidget *show2_1_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);//建立横向show2_1_box
  show_hengxian(show2_box,show2_1_box,frame2_1,show2_1,buffer2_1,provider, "无人机A","/home/wu/桌面/gtk_add/6.2/UAV.png");
  GtkWidget *show2_2 = gtk_text_view_new();//建立show2_box中的第2个窗口show2_2
  GtkTextBuffer *buffer2_2 = gtk_text_view_get_buffer(GTK_TEXT_VIEW (show2_2));//将buffer2_2连接到show2_2上  
  GtkWidget *frame2_2 = gtk_frame_new ("");//初始化边框frame2_2
  GtkWidget *show2_2_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);//建立横向show2_2_box
  show_hengxian(show2_box,show2_2_box,frame2_2,show2_2,buffer2_2,provider, "无人机B","/home/wu/桌面/gtk_add/6.2/UAV.png");
  gtk_widget_show_all(((struct data *)entry)->main_window); //将show2显示出来

  /*构建data1*/
  struct data1 *DATA1 = (struct data1 *)malloc(15000);
  DATA1->page = ((struct data *)entry)->page; DATA1->main_window = ((struct data *)entry)->main_window;
  strcpy(DATA1->sk_AB, sk_AB); strcpy(DATA1->Auth_Message_A, Auth_Message_A); 
  strcpy(DATA1->Auth_Message_B, Auth_Message_B);DATA1->rc = rc; DATA1->LK_A = LK_A;

  /*选择通信对方*/
  GtkWidget *choo_box=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);gtk_box_pack_start(GTK_BOX(page2_box),choo_box,0,0, 20);//建立横向choo_box并放入page2_box中
  GdkPixbuf *src5 = gdk_pixbuf_new_from_file("/home/wu/桌面/gtk_add/6.2/com_obj.png", NULL);
  GdkPixbuf *dst5 = gdk_pixbuf_scale_simple(src5,55,55,GDK_INTERP_BILINEAR);
  GtkWidget *com_obj = gtk_image_new_from_pixbuf(dst5);
  GtkWidget*choo_box_1=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
  gtk_box_pack_start(GTK_BOX(choo_box_1), com_obj,1,1,0);
  GtkWidget *choo_Input = gtk_entry_new();gtk_entry_set_placeholder_text(GTK_ENTRY (choo_Input), "请选择通信对象");
  gtk_box_pack_start(GTK_BOX(choo_box_1), choo_Input,1,1,0);//建立choo_Input窗口并放入choo_box_1中
  GtkWidget *choo_button=gtk_button_new_with_label("确定");gtk_box_pack_start(GTK_BOX(choo_box_1), choo_button,1,1,0);
  gtk_box_set_center_widget((GtkBox *)choo_box,choo_box_1);
  g_signal_connect(choo_button, "clicked", G_CALLBACK(Third_page), (void *)DATA1);//点击确定时，进入Third_page
  gtk_widget_show_all(((struct data *)entry)->main_window);
  /*关闭套接字*/
  close(sockfd); 
  return;
}


static void activate (GtkApplication* app, gpointer user_data)
{
  /*设置主窗口*/
  GtkWidget *main_window = gtk_application_window_new (app);//窗口初始化main_window
  gtk_window_set_title (GTK_WINDOW (main_window), "Client_A");//设置main_window名称为Client_A
  gtk_window_set_default_size (GTK_WINDOW (main_window), 1000, 1000);//设置main_window大小为1000*1000
  GtkWidget *page1 = gtk_notebook_new(); gtk_container_add(GTK_CONTAINER(main_window), page1);//建立page1页面并放入main_window中
  GtkWidget *box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);//建立垂直布局box
  gtk_container_add(GTK_CONTAINER(page1), box);//将box放入page1中
  GdkPixbuf *src3 = gdk_pixbuf_new_from_file("/home/wu/桌面/gtk_add/6.2/logo.png", NULL);
  GdkPixbuf *dst3 = gdk_pixbuf_scale_simple(src3,500,500,GDK_INTERP_BILINEAR);
  GtkWidget *title = gtk_image_new_from_pixbuf(dst3);
  gtk_box_pack_start (GTK_BOX (box), title, 1, 0, 0);//将title放入box中
  GtkWidget *IP_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);gtk_box_pack_start(GTK_BOX (box), IP_box, 0, 0, 10);//建立横向IP_box并放入box中
  GdkPixbuf *src1 = gdk_pixbuf_new_from_file("/home/wu/桌面/gtk_add/6.2/user.png", NULL);
  GdkPixbuf *dst1 = gdk_pixbuf_scale_simple(src1,55,55,GDK_INTERP_BILINEAR);
  GtkWidget *IP_Label = gtk_image_new_from_pixbuf(dst1);
  GtkWidget *IP_box_1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0); gtk_box_pack_start(GTK_BOX(IP_box_1), IP_Label,1,1,0);//设置IP字符并放入IP_box_1中
  GtkWidget *IP_Input = gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY (IP_Input), "请输入CA的IP地址");
  gtk_box_pack_start(GTK_BOX(IP_box_1), IP_Input,1,1,0);//建立IP输入窗口并放入IP_box_1中
  gtk_box_set_center_widget((GtkBox *)IP_box,IP_box_1);
  GtkWidget *PORT_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);gtk_box_pack_start(GTK_BOX (box), PORT_box,0,0,0);//建立横向PORT_box并放入box中
  GdkPixbuf *src2 = gdk_pixbuf_new_from_file("/home/wu/桌面/gtk_add/6.2/password.png", NULL);
  GdkPixbuf *dst2 = gdk_pixbuf_scale_simple(src2,55,55,GDK_INTERP_BILINEAR);
  GtkWidget *PORT_Label= gtk_image_new_from_pixbuf(dst2); 
  GtkWidget *PORT_box_1=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);gtk_box_pack_start(GTK_BOX(PORT_box_1),PORT_Label,1,1,0);//设置PORT字符并放入PORT_box_1中
  GtkWidget *PORT_Input = gtk_entry_new();gtk_entry_set_placeholder_text(GTK_ENTRY (PORT_Input), "请输入CA的端口地址");
  gtk_box_pack_start(GTK_BOX(PORT_box_1), PORT_Input,1,1,0);//建立PORT输入窗口并放入PORT_box_1中
  gtk_box_set_center_widget((GtkBox *)PORT_box,PORT_box_1);
  myCSS(); GtkWidget *START_button = gtk_button_new_with_label("开始");
  gtk_widget_set_name(START_button, "myButton_green");//设置按钮颜色
  GtkWidget *START_box=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);//建立开始按钮
  gtk_widget_set_size_request (START_button, 220, 50);
  gtk_box_set_center_widget((GtkBox *)START_box,START_button); gtk_box_pack_start(GTK_BOX(box), START_box, 0, 0, 30);//将开始按钮放入grid中
  GdkPixbuf *src4 = gdk_pixbuf_new_from_file("/home/wu/桌面/gtk_add/6.2/blank.png", NULL);
  GdkPixbuf *dst4 = gdk_pixbuf_scale_simple(src4,200,200,GDK_INTERP_BILINEAR);
  GtkWidget *blank = gtk_image_new_from_pixbuf(dst4);
  gtk_box_pack_start (GTK_BOX (box), blank, 0, 0, 0);//将title放入box中  


  /*点击START_button后，带着IP与PORT进入Second_page函数*/
  struct data *DATA = (struct data *)malloc(10000);
  DATA->IP_addr = IP_Input; DATA->PORT = PORT_Input;
  DATA->page = page1; DATA->main_window = main_window;
  g_signal_connect(START_button, "clicked", G_CALLBACK(Second_page), (void *)DATA);
  
  gtk_widget_show_all(main_window);
}

int main (int argc, char **argv)
{

  GtkApplication *app = gtk_application_new ("org.gtk.example.A", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  int status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref(app);

  return status;
}
