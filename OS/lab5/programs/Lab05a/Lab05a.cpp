#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <bitset>

int main() {
    DWORD pid = GetCurrentProcessId();
    DWORD tid = GetCurrentThreadId();

    DWORD processPriority = GetPriorityClass(GetCurrentProcess());
    int threadPriority = GetThreadPriority(GetCurrentThread());

    DWORD_PTR processAffinity, systemAffinity;
    GetProcessAffinityMask(GetCurrentProcess(), &processAffinity, &systemAffinity);

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    DWORD currentProcessor = GetCurrentProcessorNumber();

    std::cout << "PID: " << pid << std::endl;
    std::cout << "TID: " << tid << std::endl;
    std::cout << "Process Priority Class: " << processPriority << std::endl;
    std::cout << "Thread Priority: " << threadPriority << std::endl;
    std::cout << "Process Affinity Mask (binary): " << std::bitset<64>(processAffinity) << std::endl;
    std::cout << "System Affinity Mask (binary): " << std::bitset<64>(systemAffinity) << std::endl;
    std::cout << "Available Processors: " << sysInfo.dwNumberOfProcessors << std::endl;
    std::cout << "Current Processor: " << currentProcessor << std::endl;

    return 0;
}