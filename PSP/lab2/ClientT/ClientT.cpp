// ClientT.cpp
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
    setlocale(LC_ALL, "rus");

    WSADATA wsaData;
    SOCKET clientSock = INVALID_SOCKET;
    bool wsaStarted = false;

    try {
        //инициализация Winsock
        int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (res != 0) throw SetErrorMsgText("WSAStartup failed", res);
        wsaStarted = true;

        //Создание сокета
        clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (clientSock == INVALID_SOCKET) throw SetErrorMsgText("socket failed", WSAGetLastError());

        //параметры сервера (локальная петля для отладки)
        sockaddr_in serv;
        memset(&serv, 0, sizeof(serv));
        serv.sin_family = AF_INET;
        serv.sin_port = htons(2000);
        serv.sin_addr.s_addr = inet_addr("10.146.150.148"); // как требует задание

        //попытка подключения
        if (connect(clientSock, (sockaddr*)&serv, sizeof(serv)) == SOCKET_ERROR) {
            int err = WSAGetLastError();
            //Если сервер не запущен, здесь будет ошибка
            throw SetErrorMsgText("connect failed", err);
        }

        cout << "Connected to server (127.0.0.1:2000)." << endl;

        //cколько обменов хотим сделать (количество инкрементов)
        int iterations = 0;
        cout << "Enter number of messages (e.g. 1000): ";
        if (!(cin >> iterations) || iterations <= 0) {
            cout << "Invalid number, exiting." << endl;
            // Закрываем нормально
            closesocket(clientSock);
            if (wsaStarted) WSACleanup();
            return 0;
        }

        // начинаем обмен: клиент будет
        // 1) сформировать "Hello from Client xxx" с xxx=currentCounter
        // 2) отправить, получить эхо от сервера
        // 3) распарсить xxx из возвращённого сообщения, увеличить xxx на 1 и продолжить
        // цикл завершится, когда увеличенный счетчик >= iterations (или сервер закроет соединение)

        int counter = 0;
        const int BUF_SZ = 1024;
        char sendBuf[BUF_SZ];
        char recvBuf[BUF_SZ + 1];

        clock_t start = clock();
        while (counter < iterations)
        {
            // Формируем сообщение
            string msg = "Hello from Client " + to_string(counter);
            int toSendLen = (int)msg.size() + 1; // +1 чтобы отправить нуль-терминатор (удобно для печати)
            if (toSendLen > BUF_SZ) throw string("Message too long");

            memcpy(sendBuf, msg.c_str(), toSendLen);

            // Отправляем
            int sent = send(clientSock, sendBuf, toSendLen, 0);
            if (sent == SOCKET_ERROR) throw SetErrorMsgText("send failed", WSAGetLastError());

            // Получаем ответ (эхо)
            int received = recv(clientSock, recvBuf, BUF_SZ, 0);
            if (received == SOCKET_ERROR) throw SetErrorMsgText("recv failed", WSAGetLastError());
            if (received == 0) {
                // Сервер закрыл соединение
                cout << "Server closed connection (recv==0)." << endl;
                break;
            }

            // Нуль-терминируем для безопасного вывода/парсинга
            int printable = (received < BUF_SZ) ? received : BUF_SZ;
            recvBuf[printable] = '\0';

            cout << "Round " << counter << " - received: \"" << recvBuf << "\"" << endl;

            // Парсим последнее число (xxx) из полученного сообщения, инкрементируем
            // Находим последний пробел и берём число после него
            string receivedStr(recvBuf);
            size_t pos = receivedStr.find_last_of(' ');
            if (pos == string::npos) {
                // Не смогли распарсить — присвоим counter++ и продолжим
                counter++;
            }
            else {
                string numPart = receivedStr.substr(pos + 1);
                // защищённый atoi
                int parsed = 0;
                try {
                    parsed = stoi(numPart);
                }
                catch (...) {
                    parsed = counter; // fallback
                }
                counter = parsed + 1; // увеличиваем и продолжаем
            }
        }
        clock_t end = clock();

        double seconds = double(end - start) / CLOCKS_PER_SEC;
        cout << "Completed. Exchanged up to counter=" << counter << ". Time elapsed: " << seconds << " s" << endl;

        // Заканчиваем соединение — закрытие сокета даст серверу recv==0
        if (shutdown(clientSock, SD_SEND) == SOCKET_ERROR) {
            cerr << "shutdown failed: " << GetErrorMsgText(WSAGetLastError()) << endl;
        }
        // Закрываем локально
        closesocket(clientSock);
        clientSock = INVALID_SOCKET;

        if (wsaStarted) WSACleanup();
        wsaStarted = false;

        cout << "Client finished." << endl;
    }
    catch (const string& err) {
        cerr << "Error: " << err << endl;
        if (clientSock != INVALID_SOCKET) closesocket(clientSock);
        if (wsaStarted) WSACleanup();
        return 1;
    }
    catch (...) {
        cerr << "Unknown exception." << endl;
        if (clientSock != INVALID_SOCKET) closesocket(clientSock);
        if (wsaStarted) WSACleanup();
        return 1;
    }

    return 0;
}
