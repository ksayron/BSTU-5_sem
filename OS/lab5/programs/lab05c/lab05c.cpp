#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <bitset>
#include <locale>


DWORD PriorityClassFromCode(int code) {
    switch (code) {
    case 0: return IDLE_PRIORITY_CLASS;
    case 1: return BELOW_NORMAL_PRIORITY_CLASS;
    case 2: return NORMAL_PRIORITY_CLASS;
    case 3: return ABOVE_NORMAL_PRIORITY_CLASS;
    case 4: return HIGH_PRIORITY_CLASS;
    case 5: return REALTIME_PRIORITY_CLASS;
    default: return NORMAL_PRIORITY_CLASS;
    }
}

int ThreadPrioFromCode(int code) {
    switch (code) {
    case 0: return THREAD_PRIORITY_LOWEST;
    case 1: return THREAD_PRIORITY_BELOW_NORMAL;
    case 2: return THREAD_PRIORITY_NORMAL;
    case 3: return THREAD_PRIORITY_ABOVE_NORMAL;
    case 4: return THREAD_PRIORITY_HIGHEST;
    case 5: return THREAD_PRIORITY_TIME_CRITICAL;
    default: return THREAD_PRIORITY_NORMAL;
    }
}

DWORD WINAPI ThreadFunction(LPVOID param) {
    int id = (int)(intptr_t)param;
    const int MAX_IT = 1000000;
    for (int i = 1; i <= MAX_IT; ++i) {
        if (i % 1000 == 0) {
            DWORD pid = GetCurrentProcessId();
            DWORD tid = GetCurrentThreadId();
            DWORD cls = GetPriorityClass(GetCurrentProcess());
            int tprio = GetThreadPriority(GetCurrentThread());
            DWORD cpu = GetCurrentProcessorNumber();

            std::cout << "Thread[" << id << "] Iter: " << i
                << " | PID: " << pid
                << " | TID: " << tid
                << " | ProcClass: " << cls
                << " | ThPr: " << tprio
                << " | CPU: " << cpu
                << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        volatile int x = i * 2;
        (void)x;
    }
    std::cin.get();
    return 0;
}

DWORD_PTR ParseMask(const char* s) {
    std::string str(s);
    DWORD_PTR mask = 0;
    if (str.size() > 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        std::stringstream ss;
        ss << std::hex << str;
        ss >> mask;
    }
    else {
        mask = std::stoull(str);
    }
    return mask;
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cout << "Usage: Lab-05c P1 P2 P3 P4\n";
        std::cout << "P1 - affinity mask (decimal or 0x... hex)\n";
        std::cout << "P2 - process class code (0 Idle,1 BelowNormal,2 Normal,3 AboveNormal,4 High,5 RealTime)\n";
        std::cout << "P3/P4 - thread priority codes: 0 Lowest,1 BelowNormal,2 Normal,3 AboveNormal,4 Highest,5 TimeCritical\n";
        return 1;
    }
    DWORD_PTR mask = ParseMask(argv[1]);
    int p2 = std::stoi(argv[2]);
    int p3 = std::stoi(argv[3]);
    int p4 = std::stoi(argv[4]);

    std::cout << "Starting Lab-05c mask=0x" << std::hex << mask << std::dec << ", P2=" << p2 << ", P3=" << p3 << ", P4=" << p4 << "\n";

    if (!SetProcessAffinityMask(GetCurrentProcess(), mask)) {
        std::cerr << "SetProcessAffinityMask failed: " << GetLastError() << "\n";
    }
    if (!SetPriorityClass(GetCurrentProcess(), PriorityClassFromCode(p2))) {
        std::cerr << "SetPriorityClass failed: " << GetLastError() << "\n";
    }

    HANDLE h1 = CreateThread(NULL, 0, ThreadFunction, (LPVOID)1, CREATE_SUSPENDED, NULL);
    HANDLE h2 = CreateThread(NULL, 0, ThreadFunction, (LPVOID)2, CREATE_SUSPENDED, NULL);
    if (!h1 || !h2) {
        std::cerr << "CreateThread failed: " << GetLastError() << "\n";
        return 1;
    }

    if (!SetThreadPriority(h1, ThreadPrioFromCode(p3))) {
        std::cerr << "SetThreadPriority h1 failed: " << GetLastError() << "\n";
    }
    if (!SetThreadPriority(h2, ThreadPrioFromCode(p4))) {
        std::cerr << "SetThreadPriority h2 failed: " << GetLastError() << "\n";
    }

    ResumeThread(h1);
    ResumeThread(h2);

    HANDLE arr[2] = { h1, h2 };
    WaitForMultipleObjects(2, arr, TRUE, INFINITE);

    CloseHandle(h1);
    CloseHandle(h2);
    return 0;
}
