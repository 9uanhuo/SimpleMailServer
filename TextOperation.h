#include <string>
#ifndef TEXT_OPERATION
#define TEXT_OPERATION
int hex2dec(char c);
std::string UTF8ToGBK(const char* strUTF8);
void urldecode(char url[]);
char ConvertToBase64(char uc);
void EncodeBase64(char* dbuf, char* buf128, int len);
char* changeBuf(char buf[]);
char* Insert(char* a);
char* getMail(char buf[]);
bool IsValidChar(char ch);
bool IsValidEmail(std::string strEmail);
//int checkusername(char username[40]);
#endif
