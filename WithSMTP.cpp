#include <winsock2.h>
#include <iostream>
#include <windows.h>
#include "WithSMTP.h"
#include "TextOperation.h"
#include <time.h>

using namespace std;

int OpenSocket(struct sockaddr* addr)
{
	int sockfd = 0;
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		cout << "Open sockfd(TCP) error!" << endl;
		exit(-1);
	}
	if (connect(sockfd, addr, sizeof(struct sockaddr)) < 0)
	{
		cout << "Connect sockfd(TCP) error!" << endl;
		exit(-1);
	}
	return sockfd;
}

//ʱ���
void change_time(char p[], char* tim) {
	int pos = 0, pos1 = 0, flag = 0, i = 0;
	int j = 0;
	char str1[200];
	char str2[200];
	int len = strlen(tim);
	memset(str1, 0, sizeof(str1));
	memset(str2, 0, sizeof(str2));
	memset(p, 0, sizeof(p));
	while (tim[i] != '\0')
	{
		if (tim[i] == ' ')
			flag++;
		if (flag == 1 && !pos)
			pos = i;
		else if (flag == 4 && !pos1)
			pos1 = i;

		i++;
	}

	j = 0;
	for (i = pos; i < pos1; )
		str1[j++] = tim[i++];
	str1[j] = '\0';
	int len1 = strlen(str1);
	for (i = 0; i < len1; ++i) {
		if (str1[i] == ':') {
			for (j = i; j < len1; ++j)
				str1[j] = str1[j + 1];
			len1--;
		}
	}

	j = 0;
	for (i = pos1 + 1; i < len - 1; )
		str2[j++] = tim[i++];
	str2[j] = '\0';

	char p1[100] = "log_";
	char p2[100] = ".txt";

	j = 0;
	for (i = 0; i < strlen(p1);)
		p[j++] = p1[i++];
	for (i = 0; i < strlen(str2); )
		p[j++] = str2[i++];
	for (i = 0; i < strlen(str1); )
		p[j++] = str1[i++];
	for (i = 0; i < strlen(p2); )
		p[j++] = p2[i++];
	p[j] = '\0';
	for (i = 0; i < strlen(p); ++i) {
		if (p[i] == ' ')
			p[i] = '_';
	}
}
