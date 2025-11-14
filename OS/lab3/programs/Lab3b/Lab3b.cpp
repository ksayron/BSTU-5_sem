#include <windows.h>
#include <iostream>

using namespace std;

int main() {
    setlocale(LC_ALL, "RUS");

    constexpr int processAmount = 3;
    STARTUPINFOA processStartUpInfo[processAmount]{};
    PROCESS_INFORMATION processInfo[processAmount]{};

    for (auto& s : processStartUpInfo)
        s.cb = sizeof(STARTUPINFOA);

    LPCSTR exeFile = "C:\\Users\\User-512b44d0.WIN-UCLB12VI625\\source\\repos\\Lab3\\x64\\Debug\\Lab3.exe";
    char cmdLine[256];
    sprintf_s(cmdLine, "Lab3.exe 10");



    BOOL createdProcesses[processAmount]{ FALSE, FALSE, FALSE };

    createdProcesses[0] = CreateProcessA(exeFile, nullptr, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &processStartUpInfo[0], &processInfo[0]);
    createdProcesses[1] = CreateProcessA(nullptr, cmdLine, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &processStartUpInfo[1], &processInfo[1]);
    SetEnvironmentVariableA("ITER_NUM", "5");
    createdProcesses[2] = CreateProcessA(exeFile, nullptr, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &processStartUpInfo[2], &processInfo[2]);

    HANDLE procHandles[processAmount];
    int running = 0;

    for (int i = 0; i < processAmount; ++i) {
        if (createdProcesses[i]) {
            procHandles[running++] = processInfo[i].hProcess;
            cout << "Process n." << i + 1 << " created with PID: " << processInfo[i].dwProcessId << "\n";
        }
        else {
            cerr << "Error creating process n." << i + 1 << ", Error Code: " << GetLastError() << "\n";
        }
    }

    if (running > 0)
        WaitForMultipleObjects(running, procHandles, TRUE, INFINITE);

    for (int i = 0; i < processAmount; ++i) {
        if (createdProcesses[i]) {
            CloseHandle(processInfo[i].hThread);
            CloseHandle(processInfo[i].hProcess);
        }
    }

    cout << "All processes finished\n";
    return 0;
}
