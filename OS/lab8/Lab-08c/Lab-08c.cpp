#include <windows.h>
#include <iostream>
#include <vector>

void PauseClear()
{
    system("pause & cls");
}

void HeapInfo(HANDLE heap)
{
    PROCESS_HEAP_ENTRY entry;
    entry.lpData = nullptr;

    SIZE_T totalSize = 0;
    std::cout << "Heap entries info:\n";

    while (HeapWalk(heap, &entry))
    {
        std::cout << "Address: " << entry.lpData
            << ", Size: " << entry.cbData
            << " bytes, ";

        if (entry.wFlags & PROCESS_HEAP_ENTRY_BUSY)
        {
            std::cout << "Type: Busy\n";
            totalSize += entry.cbData;
        }
        else if (entry.wFlags & PROCESS_HEAP_REGION)
        {
            std::cout << "Type: Heap region\n";
        }
        else if (entry.wFlags & PROCESS_HEAP_UNCOMMITTED_RANGE)
        {
            std::cout << "Type: Uncommitted range\n";
        }
        else
        {
            std::cout << "Type: Free block\n";
        }
    }

    std::cout << "Total heap committed size (busy blocks only): "
        << totalSize << " bytes\n\n";
}


int main()
{
    const SIZE_T initialHeapSize = 1 * 1024 * 1024;
    const SIZE_T maxHeapSize = 8 * 1024 * 1024;

    SIZE_T blockSize = 1024 * 1024;
    int blockCount = 5;

    HANDLE heap = HeapCreate(0, initialHeapSize, maxHeapSize);
    if (!heap)
    {
        std::cerr << "HeapCreate failed\n";
        return 1;
    }

    std::cout << "Heap created. Initial size: " << initialHeapSize
        << " bytes, Max size: " << maxHeapSize << " bytes\n";
    HeapInfo(heap);
    PauseClear();

    std::vector<void*> blocks;
    for (int i = 0; i < blockCount; ++i)
    {
        void* block = HeapAlloc(heap, 0, blockSize);
        if (!block)
        {
            std::cerr << "HeapAlloc failed for block " << i << "\n";
            break;
        }

        blocks.push_back(block);

        std::cout << "Allocated block " << i << " at " << block << ", size: " << blockSize << " bytes\n";
        HeapInfo(heap);
        PauseClear();
    }

    for (auto block : blocks)
    {
        int* intArray = static_cast<int*>(block);
        SIZE_T numInts = blockSize / sizeof(int);
        for (SIZE_T j = 0; j < numInts; ++j)
            intArray[j] = static_cast<int>(j);
    }

    std::cout << "Blocks filled with sequential integers\n";
    PauseClear();

    for (auto block : blocks)
    {
        HeapFree(heap, 0, block);
    }
    blocks.clear();

    std::cout << "All blocks freed\n";
    HeapInfo(heap);
    PauseClear();

    if (!HeapDestroy(heap))
    {
        std::cerr << "HeapDestroy failed\n";
        return 1;
    }

    std::cout << "Heap destroyed\n";

    return 0;
}

