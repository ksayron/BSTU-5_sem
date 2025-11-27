#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

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
    if (argc < 4) {
        std::cout << "Usage: Lab-05b P1 P2 P3\n";
        std::cout << "P1 - affinity mask (decimal or 0x... hex)\n";
        std::cout << "P2,P3 - class codes: 0 Idle,1 BelowNormal,2 Normal,3 AboveNormal,4 High,5 RealTime\n";
        return 1;
    }

    DWORD_PTR mask = ParseMask(argv[1]);
    int p2 = std::stoi(argv[2]);
    int p3 = std::stoi(argv[3]);

    std::cout << "Starting Lab-05b with mask=0x" << std::hex << mask << std::dec << ", P2=" << p2 << ", P3=" << p3 << "\n";

    char modulePath[MAX_PATH];
    GetModuleFileNameA(NULL, modulePath, MAX_PATH);
    std::string dir(modulePath);
    size_t pos = dir.find_last_of("\\/");
    if (pos != std::string::npos) dir = dir.substr(0, pos + 1);
    std::string childPath = dir + "lab05x.exe";

    STARTUPINFOA si1 = { sizeof(si1) };
    PROCESS_INFORMATION pi1;
    STARTUPINFOA si2 = { sizeof(si2) };
    PROCESS_INFORMATION pi2;

    if (!CreateProcessA(childPath.c_str(), NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si1, &pi1)) {
        std::cerr << "CreateProcess 1 failed: " << GetLastError() << "\n";
        return 1;
    }

    if (!CreateProcessA(childPath.c_str(), NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si2, &pi2)) {
        std::cerr << "CreateProcess 2 failed: " << GetLastError() << "\n";
        TerminateProcess(pi1.hProcess, 1);
        return 1;
    }

    DWORD cls1 = PriorityClassFromCode(p2);
    DWORD cls2 = PriorityClassFromCode(p3);

    if (!SetProcessAffinityMask(pi1.hProcess, mask)) {
        std::cerr << "SetProcessAffinityMask 1 failed: " << GetLastError() << "\n";
    }
    if (!SetPriorityClass(pi1.hProcess, cls1)) {
        std::cerr << "SetPriorityClass 1 failed: " << GetLastError() << "\n";
    }

    if (!SetProcessAffinityMask(pi2.hProcess, mask)) {
        std::cerr << "SetProcessAffinityMask 2 failed: " << GetLastError() << "\n";
    }
    if (!SetPriorityClass(pi2.hProcess, cls2)) {
        std::cerr << "SetPriorityClass 2 failed: " << GetLastError() << "\n";
    }

    std::cout << "Launched two children. Waiting for them to finish...\n";

    HANDLE arr[2] = { pi1.hProcess, pi2.hProcess };
    WaitForMultipleObjects(2, arr, TRUE, INFINITE);

    DWORD exit1 = 0, exit2 = 0;
    GetExitCodeProcess(pi1.hProcess, &exit1);
    GetExitCodeProcess(pi2.hProcess, &exit2);
    std::cout << "Child1 exit code: " << exit1 << "\n";
    std::cout << "Child2 exit code: " << exit2 << "\n";

    CloseHandle(pi1.hProcess); CloseHandle(pi1.hThread);
    CloseHandle(pi2.hProcess); CloseHandle(pi2.hThread);

    return 0;
}