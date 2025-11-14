#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

struct ThreadParam {
    int iterations;
};
static void Lab4x(int iterations) {
    const char* username = "User-512b44d0";
    DWORD pid = GetCurrentProcessId();
    DWORD tid = GetCurrentThreadId();
    size_t ulen = username ? strlen(username) : 0;
    if (ulen == 0) ulen = 1;
    for (int i = 1; i <= iterations; ++i) {
        char ch = username[(i - 1) % ulen];
        printf("%lu - %lu - n.%d - %c\n",
            (unsigned long)pid, (unsigned long)tid, i, ch);
        Sleep(300 + (rand() % 101));
    }
}
DWORD WINAPI ThreadProc(LPVOID lpParam) {
    ThreadParam* p = static_cast<ThreadParam*>(lpParam);
    if (!p) return 1;
    Lab4x(p->iterations);
    delete p;
    return 0;
}

int main() {
    srand(static_cast<unsigned int>(time(NULL)));

    const char* username = "User-512b44d0";

    ThreadParam* p1 = new ThreadParam{ 50 };
    ThreadParam* p2 = new ThreadParam{ 125 };
    if (!p1 || !p2) {
        fprintf(stderr, "Allocation failed\n");
        delete p1; delete p2;
        return 1;
    }

    HANDLE h1 = CreateThread(NULL, 0, ThreadProc, p1, 0, NULL);
    if (!h1) {
        fprintf(stderr, "CreateThread h1 failed: %lu\n", GetLastError());
        delete p1; delete p2;
        return 1;
    }
    HANDLE h2 = CreateThread(NULL, 0, ThreadProc, p2, 0, NULL);
    if (!h2) {
        fprintf(stderr, "CreateThread h2 failed: %lu\n", GetLastError());
        CloseHandle(h1);
        delete p2;
        return 1;
    }

    DWORD pid = GetCurrentProcessId();
    DWORD tid = GetCurrentThreadId();
    size_t ulen = strlen(username);
    if (ulen == 0) ulen = 1;

    for (int i = 1; i <= 100; ++i) {
        char ch = username[(i - 1) % ulen];
        printf("%lu - %lu - mn.%d - %c\n",
            (unsigned long)pid, (unsigned long)tid, i, ch);


        if (i == 40) {
            if (h2) {
                if (!TerminateThread(h2, 0)) {
                    printf("TerminateThread(h2) failed: %lu\n", GetLastError());
                }
                else {
                    printf("Main: terminated thread2 at %d\n", i);
                }
            }
        }

        Sleep(300 + (rand() % 101));
    }


    HANDLE arr[2] = { h1, h2 };
    if (WaitForMultipleObjects(2, arr, TRUE, INFINITE) == WAIT_FAILED) {
        fprintf(stderr, "WaitForMultipleObjects failed: %lu\n", GetLastError());
    }



    CloseHandle(h1);
    CloseHandle(h2);
    return 0;
}