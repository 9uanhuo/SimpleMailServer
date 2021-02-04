#include <windows.h>
#include <string>
#include <stringapiset.h>
#include "TextOperation.h"

#define c0 0
#define c1 1
#define c2 2
#define c3 3


using namespace std;

struct Base64Date6
{
    unsigned int d4 : 6;
    unsigned int d3 : 6;
    unsigned int d2 : 6;
    unsigned int d1 : 6;
};
int hex2dec(char c)
{
    if ('0' <= c && c <= '9')
    {
        return c - '0';
    }
    else if ('a' <= c && c <= 'f')
    {
        return c - 'a' + 10;
    }
    else if ('A' <= c && c <= 'F')
    {
        return c - 'A' + 10;
    }
    else
    {
        return -1;
    }
}
std::string UTF8ToGBK(const char* strUTF8)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);
    wchar_t* wszGBK = new wchar_t[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char* szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    std::string strTemp(szGBK);

    if (wszGBK) delete[] wszGBK;
    if (szGBK) delete[] szGBK;

    return strTemp;
}
void urldecode(char url[])
{
    int i = 0;
    int len = strlen(url);
    int res_len = 0;
    char res[2048];
    for (i = 0; i < len; ++i)
    {
        char c = url[i];
        if (c != '%')
        {
            res[res_len++] = c;
        }
        else
        {
            char cp1 = url[++i];
            char cp0 = url[++i];
            int num = 0;
            num = hex2dec(cp1) * 16 + hex2dec(cp0);
            res[res_len++] = num;
        }
    }
    res[res_len] = '\0';
    strcpy(url, res);
}

char ConvertToBase64(char uc)
{
    if (uc < 26)
    {
        return 'A' + uc;
    }
    if (uc < 52)
    {
        return 'a' + (uc - 26);
    }
    if (uc < 62)
    {
        return '0' + (uc - 52);
    }
    if (uc == 62)
    {
        return '+';
    }
    return '/';
}

// base64的实现
void EncodeBase64(char* dbuf, char* buf128, int len)
{
    struct Base64Date6* ddd = NULL;
    int i = 0;
    char buf[256] = { 0 };
    char* tmp = NULL;
    char cc = '\0';

    memset(buf, 0, 256);
    strcpy_s(buf, 256, buf128);
    for (i = 1; i <= len / 3; i++)
    {
        tmp = buf + (i - 1) * 3;
        cc = tmp[2];
        tmp[2] = tmp[0];
        tmp[0] = cc;
        ddd = (struct Base64Date6*)tmp;
        dbuf[(i - 1) * 4 + 0] = ConvertToBase64((unsigned int)ddd->d1);
        dbuf[(i - 1) * 4 + 1] = ConvertToBase64((unsigned int)ddd->d2);
        dbuf[(i - 1) * 4 + 2] = ConvertToBase64((unsigned int)ddd->d3);
        dbuf[(i - 1) * 4 + 3] = ConvertToBase64((unsigned int)ddd->d4);
    }
    if (len % 3 == 1)
    {
        tmp = buf + (i - 1) * 3;
        cc = tmp[2];
        tmp[2] = tmp[0];
        tmp[0] = cc;
        ddd = (struct Base64Date6*)tmp;
        dbuf[(i - 1) * 4 + 0] = ConvertToBase64((unsigned int)ddd->d1);
        dbuf[(i - 1) * 4 + 1] = ConvertToBase64((unsigned int)ddd->d2);
        dbuf[(i - 1) * 4 + 2] = '=';
        dbuf[(i - 1) * 4 + 3] = '=';
    }
    if (len % 3 == 2)
    {
        tmp = buf + (i - 1) * 3;
        cc = tmp[2];
        tmp[2] = tmp[0];
        tmp[0] = cc;
        ddd = (struct Base64Date6*)tmp;
        dbuf[(i - 1) * 4 + 0] = ConvertToBase64((unsigned int)ddd->d1);
        dbuf[(i - 1) * 4 + 1] = ConvertToBase64((unsigned int)ddd->d2);
        dbuf[(i - 1) * 4 + 2] = ConvertToBase64((unsigned int)ddd->d3);
        dbuf[(i - 1) * 4 + 3] = '=';
    }
    return;
}


char* changeBuf(char buf[]){
    for(int i=0; buf[i]!='\0'; i++){
        if(buf[i]=='+')
            buf[i]=' ';
    } 
    return buf;
}
char* Insert(char* a)
{
    int l = strlen(a);
    int i = 0, sum = 0;
    char b[1000];
    for (i = 0; i < l; i++)
    {
        if (a[i] == '&')
        {
            b[sum + i] = ' ';
            sum = sum + 1;
        }
        b[sum + i] = a[i];
    }
    b[sum + i] = '\0';
    return b;
}
char* getMail(char buf[])
{
    std::string str = buf;
    std::size_t pos = str.find("mailFrom");
    std::string str3 = str.substr(pos);
    char* buf2 = (char*)malloc(2048);
    strcpy(buf2, str3.c_str());
    return buf2;
}
bool IsValidChar(char ch)
{
    if ((ch >= 97) && (ch <= 122)) //26个小写字母
        return true;
    if ((ch >= 65) && (ch <= 90)) //26个大写字母
        return true;
    if ((ch >= 48) && (ch <= 57)) //0~9
        return true;
    if (ch == 95 || ch == 45 || ch == 46 || ch == 64) //_-.@
        return true;
    return false;
}
bool IsValidEmail(string strEmail)
{
    if (strEmail.length() < 5) //26个小写字母
        return false;

    char ch = strEmail[0];
    if (((ch >= 97) && (ch <= 122)) || ((ch >= 65) && (ch <= 90)))
    {
        int atCount = 0;
        int atPos = 0;
        int dotCount = 0;
        for (int i = 1; i < strEmail.length(); i++) //0已经判断过了，从1开始
        {
            ch = strEmail[i];
            if (IsValidChar(ch))
            {
                if (ch == 64) //"@"
                {
                    atCount++;
                    atPos = i;
                }
                else if ((atCount > 0) && (ch == 46))//@符号后的"."号
                    dotCount++;
            }
            else
                return false;
        }
        //6. 结尾不得是字符“@”或者“.”
        if (ch == 46)
            return false;
        //2. 必须包含一个并且只有一个符号“@”
        //3. @后必须包含至少一个至多三个符号“.”
        if ((atCount != 1) || (dotCount < 1) || (dotCount > 3))
            return false;
        //5. 不允许出现“@.”或者.@
        int x, y;
        x = strEmail.find("@.");
        y = strEmail.find(".@");
        if (x > 0 || y > 0)
        {
            return false;
        }
        return true;
    }
    return false;
}
