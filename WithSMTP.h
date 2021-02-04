#include <string>
#ifndef WITH_SMTP
#define WITH_SMTP
int OpenSocket(struct sockaddr* addr);
void change_time(char p[], char* tim);
#endif
