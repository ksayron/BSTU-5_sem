#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

typedef long long ll;

static int g_threads = 0;
static int g_matrices = 0;
static int g_size = 0; // M

// Storage for matrices: allocated as int (32-bit)
static int* g_matrices_data = NULL; // size = matrices * M * M
// Global result (64-bit to avoid overflow)
static volatile LONGLONG* g_result = NULL; // size = M * M

// Partial sums per thread (allocated as LONGLONG)
static LONGLONG* g_partials = NULL; // size = threads * M * M

// Barrier for synchronization
static SYNCHRONIZATION_BARRIER g_barrier;

// Simple LCG for random numbers (single-threaded generation)
static unsigned long g_lcg = 0x12345678u;
static unsigned long lcg_next()
{
    g_lcg = 1664525u * g_lcg + 1013904223u;
    return g_lcg;
}

static int randint_range(int lo, int hi)
{
    unsigned long v = lcg_next();
    return lo + (int)(v % (unsigned long)(hi - lo + 1));
}

static void print_msg(const char* fmt, ...)
{
    char buf[512];
    va_list args;
    va_start(args, fmt);
    _vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, args);
    va_end(args);
    printf("%s\n", buf);
}

typedef struct {
    int threadIndex;
    int startMatrix;
    int endMatrix;
} ThreadArg;

DWORD WINAPI WorkerThread(LPVOID lp)
{
    ThreadArg* arg = (ThreadArg*)lp;
    int ti = arg->threadIndex;
    int s = arg->startMatrix;
    int e = arg->endMatrix;
    int M = g_size;

    LONGLONG* partial = g_partials + (ll)ti * M * M;

    for (int mi = s; mi <= e; ++mi) {
        int* mat = g_matrices_data + (ll)mi * M * M;
        for (int i = 0; i < M * M; ++i) {
            partial[i] += (LONGLONG)mat[i];
        }
    }

    print_msg("Поток %d: высчитал частичную суммую для матрицы %d..%d", ti, s, e);

    EnterSynchronizationBarrier(&g_barrier, 0);
    for (int i = 0; i < M * M; ++i) {
        InterlockedExchangeAdd64((volatile LONGLONG*)&g_result[i], partial[i]);
    }

    print_msg("Поток %d: свел частичные суммы в глобальную", ti);

    EnterSynchronizationBarrier(&g_barrier, 0);

    return 0;
}

int main(int argc, char* argv[])
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    if (argc < 4) {
        printf("Usage: %s [threads] [matrices] [size]\n", argv[0]);
        return 1;
    }

    g_threads = atoi(argv[1]);
    g_matrices = atoi(argv[2]);
    g_size = atoi(argv[3]);

    size_t totalCells = (size_t)g_matrices * g_size * g_size;
    g_matrices_data = (int*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, totalCells * sizeof(int));
    if (!g_matrices_data) {
        printf("Не удалось выделить место матрицам\n");
        return 1;
    }

    // allocate global result
    size_t resCells = (size_t)g_size * g_size;
    g_result = (volatile LONGLONG*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, resCells * sizeof(LONGLONG));
    if (!g_result) {
        printf("Не удалось выделить место матрице-результату \n");
        HeapFree(GetProcessHeap(), 0, g_matrices_data);
        return 1;
    }

    g_partials = (LONGLONG*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (size_t)g_threads * resCells * sizeof(LONGLONG));
    if (!g_partials) {
        printf("Не удалось выделить место рабочим потокам\n");
        HeapFree(GetProcessHeap(), 0, g_matrices_data);
        HeapFree(GetProcessHeap(), 0, (void*)g_result);
        return 1;
    }

    g_lcg = (unsigned long)(GetTickCount64() & 0xFFFFFFFFull);

    // Generate matrices with random ints in [-2000,2000]
    for (int m = 0; m < g_matrices; ++m) {
        int* mat = g_matrices_data + (ll)m * g_size * g_size;
        for (int i = 0; i < g_size * g_size; ++i) {
            mat[i] = randint_range(-2000, 2000);
        }
    }

    print_msg("Сгенерированно %d матриц размером %dx%d", g_matrices, g_size, g_size);

    // Initialize synchronization barrier for worker threads
    if (!InitializeSynchronizationBarrier(&g_barrier, g_threads, 0)) {
        printf("Не удалось создать синхронный барьер\n");
        HeapFree(GetProcessHeap(), 0, g_matrices_data);
        HeapFree(GetProcessHeap(), 0, (void*)g_result);
        HeapFree(GetProcessHeap(), 0, g_partials);
        return 1;
    }

    HANDLE* threads = (HANDLE*)HeapAlloc(GetProcessHeap(), 0, g_threads * sizeof(HANDLE));
    ThreadArg* args = (ThreadArg*)HeapAlloc(GetProcessHeap(), 0, g_threads * sizeof(ThreadArg));

    int base = g_matrices / g_threads;
    int rem = g_matrices % g_threads;
    int cur = 0;

    for (int t = 0; t < g_threads; ++t) {
        int cnt = base + (t < rem ? 1 : 0);
        args[t].threadIndex = t;
        args[t].startMatrix = cur;
        args[t].endMatrix = cur + cnt - 1;
        cur += cnt;
        threads[t] = CreateThread(NULL, 0, WorkerThread, &args[t], 0, NULL);
        if (!threads[t]) {
            printf("Не удалось создать поток %d\n", t);
            // mark as NULL and continue (cleanup later)
            threads[t] = NULL;
        }
    }

    // Wait for all threads to finish
    WaitForMultipleObjects(g_threads, threads, TRUE, INFINITE);

    // All threads finished. Print resulting matrix
    print_msg("Все потоки успешно завершились. Результирующая матрица (%dx%d):", g_size, g_size);
    for (int r = 0; r < g_size; ++r) {
        for (int c = 0; c < g_size; ++c) {
            int idx = r * g_size + c;
            printf("%12lld ", (long long)g_result[idx]);
        }
        printf("\n");
    }

    // Cleanup
    for (int t = 0; t < g_threads; ++t) if (threads[t]) CloseHandle(threads[t]);
    DeleteSynchronizationBarrier(&g_barrier);
    HeapFree(GetProcessHeap(), 0, threads);
    HeapFree(GetProcessHeap(), 0, args);
    HeapFree(GetProcessHeap(), 0, g_matrices_data);
    HeapFree(GetProcessHeap(), 0, (void*)g_result);
    HeapFree(GetProcessHeap(), 0, g_partials);

    return 0;
}
