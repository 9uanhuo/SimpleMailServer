#include <winsock2.h>
#include <iostream>
#include <cstdio>
#include <windows.h>
#include <wsipv6ok.h>
#include <time.h>

//自定义头文件
#include "TextOperation.h"
#include "HeadRespond.h"
#include "WithSMTP.h"

//库引入
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"Mswsock.lib")
#pragma comment(lib,"AdvApi32.lib")

//宏定义
#define PORT 80
#define BufLength 2048

using namespace std;
//全局变量
char recvBuf[BufLength] = { 0 };
string gbk;

//日志文件相关全局变量
FILE* logfile;
char p[200] = { '0' };
time_t now;
char* txt;
char* tim;
char* tim2;

//浏览器传递的邮件信息
char* mailFrom = (char*)malloc(100);
char* mailTo = (char*)malloc(200);
char* mailTitle = (char*)malloc(300);
char* mailText = (char*)malloc(600);

//邮件发送信息
char* EmailContents = (char*)malloc(5000);
char* actaddr = (char*)malloc(100);
char* From = (char*)malloc(100);
char* To = (char*)malloc(100);
char* Title = (char*)malloc(100);
char* Text = (char*)malloc(4000);

char SingleTo[20][1000];
int Tonum = 0;
//收件人拆分
void multiUser(char mailAddr[])
{
	int i, l, k = 0;
	l = strlen(mailAddr);
	for (i = 0; i < l + 1; i++)
	{
		SingleTo[Tonum][k] = mailAddr[i];
		k++;
		if (mailAddr[i] == ';')
		{
			SingleTo[Tonum][k - 1] = '\0';
			k = 0;
			Tonum++;
		}
	}
	printf("\n----------There are(is)  %d  mail need to be sent----------\n", Tonum + 1);
	for (int j = 0; j <= Tonum; j++)
		printf("----------%s----------\n", SingleTo[j]);
}

//发送邮件//交互信息输出到文件

void SendMail2(char* email, char* body)
{
	int sockfd = { 0 };
	char buf[2500] = { 0 };
	char rbuf[1500] = { 0 };
	char login[128] = { 0 };
	char pass[200] = "gzglorfysmgdjhid";
	WSADATA WSAData;
	struct sockaddr_in their_addr = { 0 };
	int iResult = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (iResult != 0)
	{
		printf("WSAStartup failed.\n");
	}
	memset(&their_addr, 0, sizeof(their_addr));

	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(25);
	struct hostent* hptr = (struct hostent*)malloc(sizeof(struct hostent));
	memset(hptr, 0, sizeof(struct hostent));
	hptr = gethostbyname(actaddr);
	memcpy(&their_addr.sin_addr.S_un.S_addr, hptr->h_addr_list[0], hptr->h_length);
	printf("----------IP of %s is: %d:%d:%d:%d  port 25----------\n", actaddr,
		their_addr.sin_addr.S_un.S_un_b.s_b1,
		their_addr.sin_addr.S_un.S_un_b.s_b2,
		their_addr.sin_addr.S_un.S_un_b.s_b3,
		their_addr.sin_addr.S_un.S_un_b.s_b4);
	printf("IP of  is : 10.21.163.155\tport %d\n",PORT);
	printf("Email From: %s\n", mailFrom);
	//多收件人相关
	for (int i = 0; i <= Tonum; i++)
	{
		printf("Email To: %s\n", SingleTo[i]);
	}

	printf("-----------Size is %d-----------\n", strlen(EmailContents) / 8);
	printf("-----------Communicating with server.....-----------\n");
	// 连接邮件服务器，如果连接后没有响应，则2秒后重新连接
	sockfd = OpenSocket((struct sockaddr*) & their_addr);
	memset(rbuf, 0, 1500);
	while (recv(sockfd, rbuf, 1500, 0) == 0)
	{
		printf("reconnect...\n");
		Sleep(2);
		sockfd = OpenSocket((struct sockaddr*) & their_addr);
		memset(rbuf, 0, 1500);
	}
	printf("-----------Connected-----------\n");
	time_t now = time(0);
	char* txt;
	char* tim = ctime(&now);
	tim2 = ctime(&now);

	//日志文件输入
	fprintf(logfile, "%s\n", recvBuf);
	urldecode(recvBuf);
	setbuf(logfile, NULL);
	printf("this is logfile::%s\n", p);
	char k[80];
	sprintf_s(k, "-------this is a receive logfile-------\n");
	fprintf(logfile, "%s\n", k);
	fflush(logfile);
	printf("Date:%s\n", tim2);//输出当前时间
	fprintf(logfile, "Date:%s\n", tim2);//在文件中也加入

	// EHLO
	memset(buf, 0, 2500);
	sprintf_s(buf, "EHLO HYL-PC\r\n");// EHLO
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);//EHLO Receive
	fprintf(logfile, "%s\n", rbuf);

	// AUTH LOGIN
	memset(buf, 0, 2500);
	sprintf_s(buf, "AUTH LOGIN\r\n");// AUTH LOGIN
	send(sockfd, buf, strlen(buf), 0);
	fprintf(logfile, "%s\n", buf);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);//Auth Login Receive:
	fprintf(logfile, "%s\n", rbuf);

	// USER
	memset(buf, 0, 2500);
	sprintf_s(buf, "1261858942@qq.com");//user
	memset(login, 0, 128);
	EncodeBase64(login, buf, strlen(buf));//cBase64 UserName
	sprintf_s(buf, "%s\r\n", login);
	send(sockfd, buf, strlen(buf), 0);
	fprintf(logfile, "%s\n", buf);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);//User Login Receive
	fprintf(logfile, "%s\n", rbuf);

	// PASSWORD
	sprintf_s(buf, pass);//password
	memset(pass, 0, 128);
	EncodeBase64(pass, buf, strlen(buf));
	sprintf_s(buf, "%s\r\n", pass);
	send(sockfd, buf, strlen(buf), 0);//Base64 Password
	fprintf(logfile, "%s\n", buf);

	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	fprintf(logfile, "%s\n", rbuf);

	// MAIL FROM
	memset(buf, 0, 2500);
	sprintf_s(buf, "MAIL FROM: <%s>\r\n", "1261858942@qq.com");
	send(sockfd, buf, strlen(buf), 0);
	fprintf(logfile, "%s\n", buf);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	fprintf(logfile, "%s\n", rbuf);

	// RCPT TO 收件人
	memset(buf, 0, 2500);
	for (int i = 0; i <= Tonum; i++)//多收件人相关
	{
		sprintf_s(buf, "RCPT TO:<%s>\r\n", SingleTo[i]);
		send(sockfd, buf, strlen(buf), 0);
		fprintf(logfile, "%s\n", buf);
		memset(rbuf, 0, 1500);
		recv(sockfd, rbuf, 1500, 0);
		fprintf(logfile, "%s\n", rbuf);
		if (rbuf[0] != '2' && rbuf[1] != '5' && rbuf[2] != '0')//显示错误信息
			printf("\nERROR INFORMATION:%s\n", rbuf);
		else
			printf("Succeed!");
	}

	// DATA 准备开始发送邮件内容
	memset(buf, 0, 2500);
	sprintf_s(buf, "DATA\r\n");// DATA 准备开始发送邮件内容
	send(sockfd, buf, strlen(buf), 0);
	fprintf(logfile, "%s\n", buf);
	memset(buf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);//Send Mail Prepare Receive
	fprintf(logfile, "%s\n", rbuf);

	// 发送邮件内容，\r\n.\r\n内容结束标记
	memset(buf, 0, 2500);
	urldecode(EmailContents);
	sprintf_s(buf, "%s", EmailContents);// 发送邮件内容，\r\n.\r\n内容结束标记
	send(sockfd, buf, strlen(buf), 0);
	fprintf(logfile, "%s\n", buf);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	fprintf(logfile, "%s\n", rbuf);

	// QUIT
	memset(buf, 0, 2500);
	sprintf_s(buf, "QUIT\r\n");
	send(sockfd, buf, strlen(buf), 0);
	fprintf(logfile, "%s\n", buf);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	fprintf(logfile, "%s\n", rbuf);

	//清理工作
	closesocket(sockfd);
	WSACleanup();
	return;
}
//SMTP发送邮件
int mailPost()
{
	//生成邮件格式 
	memset(From, 0, 100);
	memset(To, 0, 100);
	memset(Title, 0, 100);
	memset(Text, 0, 4000);
	memset(EmailContents, 0, 5000);
	sprintf(From, "From: <%s>\r\n", mailFrom);
	sprintf(To, "To: <%s>\r\n", mailTo);
	sprintf(Title, "Subject: %s\r\n\r\n", mailTitle);
	sprintf(Text, "%s\r\n.\r\n", mailText);
	EmailContents = strcpy(EmailContents, From);
	EmailContents = strcat(EmailContents, To);
	EmailContents = strcat(EmailContents, Title);
	EmailContents = strcat(EmailContents, Text);
	printf("Prepare to send an e-mail...\n");
	SendMail2(mailTo, EmailContents);
	return 0;
}
int Server2Browser() {

	//初始化日志文件
	time_t now = time(0);
	char* txt;
	char* tim = ctime(&now);
	char* tim2 = ctime(&now);
	change_time(p, tim);
	logfile = fopen(p, "w+");

	//初始化标记
	int mark = 0;

	WSADATA wsaData;
	Init();
	bool bkeepalive = true;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	//连接初始化状态
	if (iResult != 0)
	{
		printf("WSAStartup failed.\n");
		return -1;
	}
	else
	{
		printf("WSAStartup succeed!\n");
	}
	SOCKET server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);
	printf("-----------Waiting Connect----------\n");

	//持续监听端口
	while (1) {
		bind(server_socket, (struct sockaddr*) & server_addr, sizeof(server_addr));
		listen(server_socket, 5);
		SOCKET client_socket;
		client_socket = accept(server_socket, NULL, NULL);

		//接受连接
		recv(client_socket, recvBuf, BufLength, 0);

		//输出到日志文件
		fprintf(logfile, "%s\n", recvBuf);
		fflush(logfile);

		//初始化字符数组
		memset(mailFrom, 0, 100);
		memset(mailTo, 0, 200);
		memset(mailTitle, 0, 300);
		memset(mailText, 0, 600);
		////请求与响应信息输出
		//printf("\n------------INFO------------\n");
		//puts(recvBuf);
		
		string szRequest(recvBuf);
		string szResponse;
		ParseRequest(szRequest, szResponse, bkeepalive);
		writeToSocket(client_socket, szResponse.c_str(), (int)szResponse.length());
		char* daitiqu = (char*)malloc(sizeof(char) * 200);
		char* daiti = (char*)malloc(sizeof(char) * 200);
		if (recvBuf[0] == 'P') {

			if (getMail(recvBuf)[0] != '\0') {
				printf("-----------Received Info----------\n");
				mark = 1;
				daiti = getMail(recvBuf);
				strcpy(daitiqu, Insert(getMail(recvBuf)));

				//字符串格式化分切
				sscanf(daitiqu, "mailFrom=%s &mailTo=%s &mailTitle=%s &mailText=%s", mailFrom, mailTo, mailTitle, mailText);

				//解码收到的内容
				urldecode(mailFrom);
				urldecode(mailTo);
				urldecode(mailTitle);
				urldecode(mailText);

				changeBuf(mailText);
				multiUser(mailTo);
				string st;
				for (int k = 0; k <= Tonum; k++) {
					st = SingleTo[k];
					if (!IsValidEmail(st)) {
						printf("-----Mail Format Error-----\n");
						mark = 0;
					}
				}
			}
		}
		if (mark == 1) {
			mailPost();
		}
		printf("------Communication continuing....------\n");
		closesocket(client_socket);
		mark = 0;
	}
	closesocket(server_socket);
	WSACleanup();
	return 0;
}
int main() {
	scanf("%s", actaddr);
	Server2Browser();
	return 0;
}