#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <cstdio>
#include <thread>
#include <chrono>
#include <iostream>
#include <tchar.h>
#include "Error.h"

using namespace std;


int extract_and_increment(const std::string& message) {
	size_t pos = message.find_last_of(' ');
	if (pos != std::string::npos) {
		std::string num_str = message.substr(pos + 1);
		int num = stoi(num_str);
		return num + 1;
	}
	return 1;
}

int main(int argc, _TCHAR* argv[])
{
	SOCKET cS;              // дескриптор сокета, клиентский сокет
	WSADATA wsaData;

	try {
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw SetErrorMsgText("Startup: ", WSAGetLastError());

		if ((cS = socket(
			AF_INET,            // формат адреса(IPv4)
			SOCK_STREAM,        // тип мокета(SOCK_DGRAM сокет для сообщений, SOCK_STREAM потоковый сокет)
			NULL                // протокол транспортного уровня
		)) == INVALID_SOCKET)
			throw SetErrorMsgText("socket: ", WSAGetLastError());

		SOCKADDR_IN serv;								// параметры  сокета сервера
		serv.sin_family = AF_INET;						// используется IP-адресация  
		serv.sin_port = htons(2000);                   // TCP-порт 2000
		serv.sin_addr.s_addr = inet_addr("127.0.0.1");  // адрес сервера

		if ((connect(cS, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR)
			throw SetErrorMsgText("connect: ", WSAGetLastError());

		cout << "Connected to server" << endl;

		char ibuf[50],                      // буфер ввода
			obuf[50] = "Hello server ";		// буфер вывода
		int libuf = 0,                      // количество принятых байт
			lobuf = 0;                      // количество отправленных байт


		/*int num_messages;
		cout << "Enter number of messages: ";
		cin >> num_messages;*/

		int counter = 1;

		if ((lobuf = send(cS, obuf, strlen(obuf) + 1, NULL)) == SOCKET_ERROR)
			throw SetErrorMsgText("send: ", WSAGetLastError());
		if ((libuf = recv(cS, ibuf, sizeof(ibuf), NULL)) == SOCKET_ERROR)
			throw SetErrorMsgText("recv: ", WSAGetLastError());

		cout << "Message recieved: " << ibuf << endl;

		/*while(num_messages-- > 0) {
			snprintf(obuf, sizeof(obuf), "Hello server %d", counter);





			counter = extract_and_increment(ibuf);

			snprintf(ibuf, sizeof(ibuf), "Hello server %d", counter);

			if ((lobuf = send(cS, ibuf, strlen(ibuf) + 1, NULL)) == SOCKET_ERROR)
				throw SetErrorMsgText("send: ", WSAGetLastError());

			Sleep(5);
		}*/

		if ((lobuf = send(cS, "", 0, NULL)) == SOCKET_ERROR)
			throw SetErrorMsgText("send: ", WSAGetLastError());


		if (closesocket(cS) == SOCKET_ERROR)
			throw SetErrorMsgText("closesocket: ", WSAGetLastError());

		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup: ", WSAGetLastError());

		cout << "Message sended!" << endl;
	}
	catch (string errorMessage) {
		cout << endl << errorMessage;
	}



	return 0;
}

