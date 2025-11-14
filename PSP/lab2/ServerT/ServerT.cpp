// ServerT.cpp
#include <iostream>
#include <string>
#include <cstring>
#include <ctime>

#include <winsock2.h> 
#include <windows.h>

#pragma comment(lib, "WS2_32.lib")
#pragma warning(disable:4996)


using namespace std;

string GetErrorMsgText(int code)
{
    string msgText;
    switch (code)
    {
    case WSAEINTR: msgText = "Работа функции прервана"; break;
    case WSAEACCES: msgText = "Разрешение отвергнуто"; break;
    case WSAEFAULT: msgText = "Ошибочный адрес"; break;
    case WSAEINVAL: msgText = "Ошибка в аргументе"; break;
    case WSAEMFILE: msgText = "Открыто слишком много файлов"; break;
    case WSAEWOULDBLOCK: msgText = "Ресурс временно недоступен"; break;
    case WSAEINPROGRESS: msgText = "Операция в процессе"; break;
    case WSAEALREADY: msgText = "Операция уже выполняется"; break;
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
    default: msgText = "Unknown error"; break;
    }
    return msgText;
}


string SetErrorMsgText(const string& msgText, int code)
{
    return msgText + " : " + GetErrorMsgText(code) + " (" + to_string(code) + ")";
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    bool wsStarted = false;
    SOCKET listenSocket = INVALID_SOCKET;

    try
    {
        WSADATA wsaData;
        int wsaInitResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (wsaInitResult != 0) {
            throw SetErrorMsgText("WSAStartup failed", wsaInitResult);
        }
        wsStarted = true;

        //создание сокета
        listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (listenSocket == INVALID_SOCKET) {
            throw SetErrorMsgText("socket failed", WSAGetLastError());
        }

        //настройка адреса сервера
        sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(2000);          // порт 2000
        serverAddr.sin_addr.s_addr = INADDR_ANY;    // слушать все интерфейсы

        //связываем сокет с адресом
        if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) { //Привязывает сокет (listenSocket) к конкретному IP-адресу и порту.
            closesocket(listenSocket);
            listenSocket = INVALID_SOCKET;
            throw SetErrorMsgText("bind failed", WSAGetLastError());
        }

        //переводим сокет в режим прослушивания
        if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) { //Переводит сокет в режим прослушивания входящих соединений.
            closesocket(listenSocket);
            listenSocket = INVALID_SOCKET;
            throw SetErrorMsgText("listen failed", WSAGetLastError());
        }

         std::cout << "Server is listening on port 2000..." << endl;

        // Основной цикл: принимать клиентов последовательно
        while (true)
        {
            sockaddr_in clientAddr;
            int clientAddrLen = sizeof(clientAddr);
            SOCKET clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &clientAddrLen); //блокирующая функция, ждёт, пока не подключится клиент.
            
            //listenSocket — сокет, который слушает входящие соединения (listen() уже вызван).
            //(sockaddr*)&clientAddr — адрес структуры, куда accept записывает данные клиента.
            //&clientAddrLen — размер структуры.

            if (clientSocket == INVALID_SOCKET) {
                // В некоторых случаях можно продолжить цикл, но здесь считаем критической ошибкой
                throw SetErrorMsgText("accept failed", WSAGetLastError());
            }

            std::cout << "Client connected." << endl;
            std::cout << "Client IP:   " << inet_ntoa(clientAddr.sin_addr) << endl;
            std::cout << "Client Port: " << ntohs(clientAddr.sin_port) << endl;

            // Измерение времени обмена для этой сессии
            clock_t startTime = clock();

            const int BUF_SIZE = 1024;
            char buffer[BUF_SIZE + 1];

            // Читаем и эхо отправляем, пока клиент не закроет соединение (recv == 0) или не будет ошибка
            while (true)
            {
                int received = recv(clientSocket, buffer, BUF_SIZE, 0); //функция Winsock для чтения данных из сокета.
                //clientSocket — сокет, через который общаемся с текущим клиентом.
                //buffer — массив char, куда будут записаны полученные данные.
                //BUF_SIZE — максимально читаемое количество байт (1024 в нашем случае).
                //0 — флаги, обычно 0 (без специальных опций).
                
                if (received == SOCKET_ERROR)
                {
                    cerr << "recv error: " << GetErrorMsgText(WSAGetLastError()) << " (" << WSAGetLastError() << ")" << endl;
                    break;
                }
                if (received == 0)
                {
                    // Клиент корректно закрыл соединение
                    std::cout << "Client closed the connection." << endl;
                    break;
                }

                // Сделаем безопасную нуль-терминацию для вывода как строки
                int toPrint = (received < BUF_SIZE) ? received : BUF_SIZE;
                buffer[toPrint] = '\0';
                std::cout << "Received (" << received << " bytes): " << buffer << endl;

                // Эхо: отправляем обратно ровно тот набор байт, что получили
                int sent = send(clientSocket, buffer, received, 0);
                if (sent == SOCKET_ERROR) {
                    cerr << "send error: " << GetErrorMsgText(WSAGetLastError()) << " (" << WSAGetLastError() << ")" << endl;
                    break;
                }
            } // конец внутреннего while (приёма)

            clock_t endTime = clock();
            double seconds = double(endTime - startTime) / CLOCKS_PER_SEC;
            std::cout << "Session duration: " << seconds << " seconds." << endl;

            // Закрываем клиентский сокет и продолжаем принимать следующий
            if (closesocket(clientSocket) == SOCKET_ERROR) {
                cerr << "closesocket(client) error: " << GetErrorMsgText(WSAGetLastError()) << " (" << WSAGetLastError() << ")" << endl;
            }
            std::cout << "Waiting for next client..." << endl << endl;
        } // конец внешнего while
    }
    catch (const string& errMsg)
    {
        cerr << "Exception: " << errMsg << endl;
    }
    catch (...)
    {
        cerr << "Unknown exception occurred." << endl;
    }

    // Очистка ресурсов перед выходом
    if (listenSocket != INVALID_SOCKET) {
        closesocket(listenSocket);
        listenSocket = INVALID_SOCKET;
    }
    if (wsStarted) {
        WSACleanup();
    }

    std::cout << "Server terminated." << endl;
    return 0;
}
