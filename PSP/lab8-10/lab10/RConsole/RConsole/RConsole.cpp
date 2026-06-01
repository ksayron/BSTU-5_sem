#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")
using std::string;
using namespace std;

#define BUFFER_SIZE 200

string GetErrorMsgText(int code) // cформировать текст ошибки
{
	char buff[50];
	DWORD bufflen = sizeof(buff);
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0) {
		return std::string(); //No error message has been recorded
	}
	else {
		return std::system_category().message(errorMessageID);
	}
};
string SetPipeError(string msgText, int code)
{
	return msgText + to_string(code) + ". " + GetErrorMsgText(code);
};


int main() {
	setlocale(LC_ALL, "Russian");

	WSADATA wsaData;
	SOCKET clientSocket;
	sockaddr_in serverAddr;
	char buffer[BUFFER_SIZE];
	int result;

	try {
		// Инициализация WinSock
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			throw SetPipeError("WSaStartup error: ", GetLastError());
		}

		// Создание сокета
		clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (clientSocket == INVALID_SOCKET) {
			throw SetPipeError("Socket creation failed ", GetLastError());
		}

		// Настройка адреса сервера
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(2001);
		inet_pton(AF_INET, "127.0.0.2", &serverAddr.sin_addr);

		// Подключение к серверу
		if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
			throw SetPipeError("Connection to server failed ", GetLastError());
		}

		cout << "Connected to server." << endl;

		while (true) {
			string msg;
			printf_s("1 - start  \t (разрешить подключение клиентов к серверу)\n");
			printf_s("2 - stop  \t (запретить подключение клиентов к серверу)\n");
			printf_s("3 - exit  \t (завершить работу сервера)\n");
			printf_s("4 - statistics\t (вывод статистики)\n");
			printf_s("5 - wait  \t (приостанавливает подключение клиентов)\n");
			printf_s("6 - shutdown  \t (wait + exit)\n");
			printf_s("0 - закрыть\n");

			getline(cin, msg);

			if (msg == "0") {
				result = send(clientSocket, msg.c_str(), msg.length() + 1, 0);
				if (result == SOCKET_ERROR) {
					throw SetPipeError("Send failed ", GetLastError());
				}
				break;
			}
			result = send(clientSocket, msg.c_str(), msg.length() + 1, 0);
			if (result == SOCKET_ERROR) {
				throw SetPipeError("Send failed ", GetLastError());
			}
			cout << "command sent\n";

			result = recv(clientSocket, buffer, BUFFER_SIZE, 0);
			if (result > 0) {
				cout << buffer << endl;
			}
			else if (result == 0) {
				cout << "Connection closed by server. " << endl;
				break;
			}
			else {
				throw SetPipeError("Recv failed ", GetLastError());
			}
		}
	}
	catch (string errorMsgText) {
		cout << endl << "WSAGetLastError: " << errorMsgText;
	}
	return 0;
}
