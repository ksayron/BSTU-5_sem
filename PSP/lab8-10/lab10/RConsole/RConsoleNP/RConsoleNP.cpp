#include <iostream>
#include "Winsock2.h"
#include <string>
#pragma comment(lib, "WS2_32.lib")
#pragma warning(disable : 4996)
using namespace std;


#define BUFFER_SIZE 200

#define INADDR_ANY        (u_long)0x00000000 //любой адрес       +++ 
#define INADDR_LOOPBACK    0x7f000001        // внутренняя петля +++
#define INADDR_BROADCAST  (u_long)0xffffffff // широковещание    +++  
#define INADDR_NONE        0xffffffff        // нет адреса  
#define ADDR_ANY           INADDR_ANY        // любой адрес       

typedef struct sockaddr_in SOCKADDR_IN;    //                     +++
typedef struct sockaddr_in* PSOCKADDR_IN;
typedef struct sockaddr_in FAR* LPSOCKADDR_IN;

string GetErrorMsgText(int code)
{
	string msgText;
	switch (code)
	{
	case WSAEINTR: msgText = "Работа функции прервана"; break;
	case WSAEACCES:	msgText = "Разрешение отвергнуто"; break;
	case WSAEFAULT: msgText = "Ошибочный адрес"; break;
	case WSAEINVAL:	msgText = "Ошибка в аргументе";	break;
	case WSAEMFILE:	msgText = "Открыто слишком много файлов"; break;
	case WSAEWOULDBLOCK: msgText = "Ресурс временно недоступен"; break;
	case WSAEINPROGRESS: msgText = "Операция в процессе развития"; break;
	case WSAEALREADY: msgText = "Операция уже выполняется";	break;
	case WSAENOTSOCK: msgText = "Сокет задан неправильно"; break;
	case WSAEDESTADDRREQ: msgText = "Требуется адрес расположения"; break;
	case WSAEMSGSIZE: msgText = "Сообщение слишком длинное"; break;
	case WSAEPROTOTYPE: msgText = "Неправильный тип протокола для сокета"; break;
	case WSAENOPROTOOPT: msgText = "Ошибка в опции протокола"; break;
	case WSAEPROTONOSUPPORT: msgText = "Протокол не поддерживается"; break;
	case WSAESOCKTNOSUPPORT: msgText = "Тип сокета не поддерживается"; break;
	case WSAEOPNOTSUPP: msgText = "Операция не поддерживается"; break;
	case WSAEPFNOSUPPORT: msgText = "Тип протоколов не поддерживается"; break;
	case WSAEAFNOSUPPORT: msgText = "Тип адресов не поддерживается протоколом"; break;
	case WSAEADDRINUSE: msgText = "Адрес уже используется"; break;
	case WSAEADDRNOTAVAIL: msgText = "Запрошенный адрес не может быть использован"; break;
	case WSAENETDOWN: msgText = "Сеть отключена"; break;
	case WSAENETUNREACH: msgText = "Сеть не достижима"; break;
	case WSAENETRESET: msgText = "Сеть разорвала соединение"; break;
	case WSAECONNABORTED: msgText = "Программный отказ связи"; break;
	case WSAECONNRESET: msgText = "Связь не восстановлена"; break;
	case WSAENOBUFS: msgText = "Не хватает памяти для буферов"; break;
	case WSAEISCONN: msgText = "Сокет уже подключен"; break;
	case WSAENOTCONN: msgText = "Сокет не подключен"; break;
	case WSAESHUTDOWN: msgText = "Нельзя выполнить send: сокет завершил работу"; break;
	case WSAETIMEDOUT: msgText = "Закончился отведенный интервал времени"; break;
	case WSAECONNREFUSED: msgText = "Соединение отклонено"; break;
	case WSAEHOSTDOWN: msgText = "Хост в неработоспособном состоянии"; break;
	case WSAEHOSTUNREACH: msgText = "Нет маршрута для хоста"; break;
	case WSAEPROCLIM: msgText = "Слишком много процессов"; break;
	case WSASYSNOTREADY: msgText = "Сеть не доступна"; break;
	case WSAVERNOTSUPPORTED: msgText = "Данная версия недоступна"; break;
	case WSANOTINITIALISED: msgText = "Не выполнена инициализация WS2_32.dll"; break;
	case WSAEDISCON: msgText = "Выполняется отключение"; break;
	case WSATYPE_NOT_FOUND: msgText = "Класс не найден"; break;
	case WSAHOST_NOT_FOUND: msgText = "Хост не найден"; break;
	case WSATRY_AGAIN: msgText = "Неавторизованный хост не найден"; break;
	case WSANO_RECOVERY: msgText = "Неопределенная ошибка"; break;
	case WSANO_DATA: msgText = "Нет записи запрошенного типа"; break;
	case WSASYSCALLFAILURE: msgText = "Аварийное завершение системного вызова"; break;
	case 2: msgText = "Неудачное завершение"; break;
	case ERROR_INVALID_PARAMETER: msgText = "Значение параметра pimax превосходит величину  PIPE_UNLMITED_INSTANCES"; break;
	case ERROR_NO_DATA: msgText = "The pipe is being closed"; break;
	case ERROR_PIPE_CONNECTED: msgText = "There is a process on other end of the pipe"; break;
	case ERROR_PIPE_LISTENING: msgText = "Waiting for a process to open the other end of the pipe"; break;
	case ERROR_CALL_NOT_IMPLEMENTED: msgText = "This function is not supported on this system"; break;
	default: msgText = "**ERROR**"; break;
	}
	return msgText;
};

string SetPipeError(string msgText, int code)
{
	return msgText + GetErrorMsgText(code);
}



int main()
{
	setlocale(LC_ALL, "Russian");
	HANDLE hPipe;

	LPCSTR pipeName = "\\\\.\\pipe\\Tube";
	//LPCSTR pipeName = "\\\\DESKTOP-9MC22NJ\\pipe\\Tube";

	try {
		hPipe = CreateFileA(
			pipeName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			NULL,
			NULL
		);

		//hPipe = CreateFileA(
		//	pipeName,                    // Имя канала
		//	GENERIC_READ | GENERIC_WRITE, // Режим чтения и записи
		//	0,                            // Нет общего доступа
		//	NULL,                         // Атрибуты безопасности
		//	OPEN_EXISTING,                // Открытие существующего канала
		//	0,                            // Атрибуты
		//	NULL                          // Шаблон файла
		//);


		if (hPipe == INVALID_HANDLE_VALUE)
			throw SetPipeError("create file: ", GetLastError());

		cout << "connected to the server." << endl;

		while (true) {
			string msg;
			DWORD rbufl = 0;
			DWORD wbufl = 0;
			char rbuf[100];
			char buffer[BUFFER_SIZE];

			printf_s("1 - start  \t (разрешить подключение клиентов к серверу)\n");
			printf_s("2 - stop  \t (запретить подключение клиентов к серверу)\n");
			printf_s("3 - exit  \t (завершить работу сервера)\n");
			printf_s("4 - statistics\t (вывод статистики)\n");
			printf_s("5 - wait  \t (приостанавливает подключение клиентов)\n");
			printf_s("6 - shutdown  \t (wait + exit)\n");
			printf_s("0 - закрыть\n");

			getline(cin, msg);

			if (msg == "0") {
				//result = send(clientSocket, msg.c_str(), msg.length() + 1, 0);
				if (!WriteFile(hPipe, msg.c_str(), msg.length() + 1, &wbufl, NULL)) {
					throw SetPipeError("writefile: ", GetLastError());
				}
				break;
			}

			if (!WriteFile(hPipe, msg.c_str(), msg.length() + 1, &wbufl, NULL))
				throw SetPipeError("writefile: ", GetLastError());
			else
				cout << "message sended." << endl;


			if (!ReadFile(hPipe, buffer, BUFFER_SIZE, &rbufl, NULL))
				throw SetPipeError("readFile: ", GetLastError());
			else
				cout << buffer << endl;
			

			/*result = recv(clientSocket, buffer, BUFFER_SIZE, 0);
			if (result > 0) {
				cout << buffer << endl;
			}
			else if (result == 0) {
				cout << "Connection closed by server. " << endl;
				break;
			}
			else {
				throw SetPipeError("Recv failed ", GetLastError());
			}*/
		}

		DisconnectNamedPipe(hPipe);
		CloseHandle(hPipe);
	}
	catch (string errorMsgText) {
		cout << endl << "Error: " << errorMsgText;
		CloseHandle(hPipe);
	}


	return 0;
}
