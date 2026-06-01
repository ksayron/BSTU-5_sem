#pragma once

#include <iostream>

#include "WinSock2.h"                   // заголовок  WS2_32.dll
#pragma comment (lib, "WS2_32.lib")     // экспорт  WS2_32.dll

using namespace std;

string GetErrorMsgText(int code);
string SetErrorMsgText(string msgText, int code);