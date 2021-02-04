#include <string>
#ifndef HEAD_RESPOND
#define HEAD_RESPOND
void Init();
BOOL writeToSocket(SOCKET s, const char* buff, int count);
bool ParseRequest(std::string szRequest, std::string& szResponse, bool& bKeepAlive);
#endif