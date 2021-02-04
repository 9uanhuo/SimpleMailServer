#include <winsock2.h>
#include <windows.h>
#include <string>
#include <map>
#include <iterator>
#include <time.h>
#include <ws2def.h>
#include "HeadRespond.h"

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"Mswsock.lib")
#pragma comment(lib,"AdvApi32.lib")

#define ERROR404 "404.html"
#define ERROR501 "501.html"
#define SERVERNAME "localhost"

using namespace std;

typedef map<string, string>	MIMETYPES;
MIMETYPES  MimeTypes;
string m_HomeDir;
string m_DefIndex;

void Init()
{
	//初始化 设置虚拟目录
	m_HomeDir = ".\\html";
	m_DefIndex = "index.html";

	if (m_HomeDir.substr(m_HomeDir.size() - 1, 1) != "\\")
		m_HomeDir += "\\";
	MimeTypes["js"] = "application/x-javascript";
	MimeTypes["htm"] = "text/html";
	MimeTypes["html"] = "text/html";
	MimeTypes["css"] = "text/css";
}
BOOL writeToSocket(SOCKET s, const char* buff, int count)
{
	int sendCount = 0;
	int currentPosition = 0;
	while (count > 0 && (sendCount = send(s, buff + currentPosition, count, 0)) != SOCKET_ERROR)
	{
		count -= sendCount;
		currentPosition += sendCount;
	}
	if (sendCount == SOCKET_ERROR)return FALSE;
	return TRUE;
}
bool ParseRequest(string szRequest, string& szResponse, bool& bKeepAlive)
{
	// 简单处理请求
	string szMethod;
	string szFileName;
	string szFileExt;
	string szStatusCode("200 OK");
	string szContentType("text/html");
	string szConnectionType("close");
	string szNotFoundMessage;
	string szDateTime;
	char pResponseHeader[2048];
	fpos_t lengthActual = 0, length = 0;
	char* pBuf = NULL;
	int n;

	// 检查请求类型
	n = szRequest.find(" ", 0);
	if (n != string::npos)
	{
		szMethod = szRequest.substr(0, n);
		if (szMethod == "GET")
		{
			// 获取请求文件名
			int n1 = szRequest.find(" ", n + 1);
			if (n != string::npos)
			{
				szFileName = szRequest.substr(n + 1, n1 - n - 1);
				if (szFileName == "/")
				{
					szFileName = m_DefIndex;
				}
				else if (szFileName == "http://10.21.163.155/")
				{
					szFileName = m_DefIndex;
				}
				else if (szFileName == "http://10.21.163.155/layui.css")
				{
					szFileName = m_HomeDir + "\\layui.css";
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			szStatusCode = "501 Not Implemented";
			szFileName = ERROR501;
		}
	}
	else
	{
		return false;
	}

	// 决定连接类型
	n = szRequest.find("\nConnection: Keep-Alive", 0);
	if (n != string::npos)
		bKeepAlive = true;

	// 处理目录类型
	int nPointPos = szFileName.rfind(".");
	if (nPointPos != string::npos)
	{
		szFileExt = szFileName.substr(nPointPos + 1, szFileName.size());
		strlwr((char*)szFileExt.c_str());
		MIMETYPES::iterator it;
		it = MimeTypes.find(szFileExt);
		if (it != MimeTypes.end())
			szContentType = (*it).second;
	}

	// 附加当前时间
	char szDT[128];
	struct tm* newtime;
	time_t ltime;

	time(&ltime);
	newtime = gmtime(&ltime);
	strftime(szDT, 128,
		"%a, %d %b %Y %H:%M:%S GMT", newtime);

	// 读取文件
	FILE* f;
	f = fopen((m_HomeDir + szFileName).c_str(), "r+b");
	if (f != NULL)
	{
		// 获取文件大小
		fseek(f, 0, SEEK_END);
		fgetpos(f, &lengthActual);
		fseek(f, 0, SEEK_SET);

		pBuf = new char[lengthActual + 1];

		length = fread(pBuf, 1, lengthActual, f);
		fclose(f);

		// 制作响应
		sprintf(pResponseHeader, "HTTP/1.1 %s\r\nDate: %s\r\nServer: %s\r\nAccept-Ranges: bytes\r\nContent-Length: %d\r\nConnection: %s\r\nContent-Type: %s\r\n\r\n",
			szStatusCode.c_str(), szDT, SERVERNAME, (int)length, bKeepAlive ? "Keep-Alive" : "close", szContentType.c_str());
	}
	else
	{
		// 文件未找到
		if (szFileName == ERROR501)
		{
			szNotFoundMessage = "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"></head>"
				"<body><h2>Casper Simple Web Server</h2><div>501 - Method Not Implemented</div></body></html>";
		}
		else
		{
			f = fopen((m_HomeDir + ERROR404).c_str(), "r+b");
			if (f != NULL)
			{
				// 重新获取文件大小
				fseek(f, 0, SEEK_END);
				fgetpos(f, &lengthActual);
				fseek(f, 0, SEEK_SET);
				pBuf = new char[lengthActual + 1];
				length = fread(pBuf, 1, lengthActual, f);
				fclose(f);
				szNotFoundMessage = string(pBuf, length);
				delete[] pBuf;
				pBuf = NULL;
			}
			else szNotFoundMessage = "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"></head>"
				"<body><h2>Casper Simple Web Server</h2><div>404 - Not Found</div></body></html>";
			szStatusCode = "404 Resource not found";
		}

		sprintf(pResponseHeader, "HTTP/1.1 %s\r\nContent-Length: %d\r\nContent-Type: text/html\r\nDate: %s\r\nServer: %s\r\n\r\n%s",
			szStatusCode.c_str(), szNotFoundMessage.size(), szDT, SERVERNAME, szNotFoundMessage.c_str());
		bKeepAlive = false;
	}
	szResponse = string(pResponseHeader);
	if (pBuf)
		szResponse += string(pBuf, length);
	delete[] pBuf;
	pBuf = NULL;
	return false;
}