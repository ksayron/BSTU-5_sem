#include <windows.h>
#include <iostream>

void Pause()
{
    std::cout << "Press Enter to continue...\n";
    std::cin.get();
}

int main()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    SIZE_T pageSize = si.dwPageSize;
    SIZE_T totalPages = 256;
    SIZE_T halfPages = totalPages / 2;

    SIZE_T totalSize = pageSize * totalPages;

    std::cout << "Page size: " << pageSize << " bytes\n";
    std::cout << "Reserving " << totalPages << " pages (" << totalSize << " bytes)\n";

    LPVOID reservedMemory = VirtualAlloc(
        nullptr,
        totalSize,
        MEM_RESERVE,
        PAGE_NOACCESS
    );

    if (!reservedMemory)
    {
        std::cerr << "VirtualAlloc MEM_RESERVE failed\n";
        return 1;
    }

    std::cout << "Reserved memory at: " << reservedMemory << "\n";
    Pause();

    LPVOID commitMemory = static_cast<char*>(reservedMemory) + pageSize * halfPages;
    if (!VirtualAlloc(
        commitMemory,
        pageSize * halfPages,
        MEM_COMMIT,
        PAGE_READWRITE
    ))
    {
        std::cerr << "VirtualAlloc MEM_COMMIT failed\n";
        return 1;
    }

    std::cout << "Committed memory at: " << commitMemory << "\n";
    Pause();

    int* intArray = static_cast<int*>(commitMemory);
    SIZE_T numInts = (pageSize * halfPages) / sizeof(int);

    for (SIZE_T i = 0; i < numInts; ++i)
        intArray[i] = static_cast<int>(i);

    std::cout << "Memory filled with sequential integers\n";
    Pause();

    DWORD oldProtect;
    if (!VirtualProtect(commitMemory, pageSize * halfPages, PAGE_READONLY, &oldProtect))
    {
        std::cerr << "VirtualProtect failed\n";
        return 1;
    }

    std::cout << "Memory protection changed to READONLY\n";
    Pause();

    if (!VirtualFree(commitMemory, pageSize * halfPages, MEM_DECOMMIT))
    {
        std::cerr << "VirtualFree MEM_DECOMMIT failed\n";
        return 1;
    }

    std::cout << "Decommitted second half of memory\n";
    Pause();

    if (!VirtualFree(reservedMemory, 0, MEM_RELEASE))
    {
        std::cerr << "VirtualFree MEM_RELEASE failed\n";
        return 1;
    }

    std::cout << "Released all reserved memory\n";
    Pause();

    return 0;
}
