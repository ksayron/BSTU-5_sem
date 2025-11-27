#include <windows.h>
#include <iostream>
#include <ctime>

int main() {
    clock_t startTime = clock();

    DWORD pid = GetCurrentProcessId();
    DWORD tid = GetCurrentThreadId();
    DWORD processPriority = GetPriorityClass(GetCurrentProcess());
    int threadPriority = GetThreadPriority(GetCurrentThread());

    for (int i = 1; i <= 1000000; ++i) {
        if (i % 1000 == 0) {
            DWORD currentProcessor = GetCurrentProcessorNumber();
            std::cout << "Iteration: " << i
                << " | PID: " << pid
                << " | TID: " << tid
                << " | Process Priority: " << processPriority
                << " | Thread Priority: " << threadPriority
                << " | Processor: " << currentProcessor << std::endl;
            Sleep(20);
        }
    }

    clock_t endTime = clock();
    double elapsedSeconds = static_cast<double>(endTime - startTime) / CLOCKS_PER_SEC * 7;
    std::cout << "Total execution time: " << elapsedSeconds << " seconds" << std::endl;
    int i;
    std::cin >> i;
    return 0;
}