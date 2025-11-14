#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "RUS");

    int loops = 0;
    if (argc > 1)
        loops = atoi(argv[1]);
    else {
        char* envVar = getenv("ITER_NUM");
        if (envVar)
            loops = atoi(envVar);
    }

    if (loops <= 0) {
        cerr << "Error: loops amount is not specified\n";
        ExitProcess(1);
    }

    DWORD pid = GetCurrentProcessId();

  
    cout << "loops amount :" + to_string(loops)  <<endl;

    for (int i = 1; i <= loops; ++i) {
        cout << "i: " + to_string(i) + " PID: " + to_string(pid) << endl;
        Sleep(500);
    }
    cout << "loops finished" << endl;

    return 0;
}
