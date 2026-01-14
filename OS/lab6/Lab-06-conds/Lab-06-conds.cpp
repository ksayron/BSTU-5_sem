#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

static int* g_buffer = NULL;
static int g_capacity = 0;
static int g_count = 0;
static int g_head = 0;
static int g_tail = 0;

static CRITICAL_SECTION g_cs;
static CONDITION_VARIABLE g_notFull;
static CONDITION_VARIABLE g_notEmpty;

static volatile LONG g_totalProduced = 0;
static volatile LONG g_totalConsumed = 0;
static ULONGLONG g_appStartTime = 0;
static ULONGLONG g_prodStartTime = 0;
static ULONGLONG g_consStartTime = 0;
static ULONGLONG g_lastProdTime = 0;
static ULONGLONG g_lastConsTime = 0;
static double g_sumProdIntervals = 0.0;
static double g_sumConsIntervals = 0.0;
static volatile LONGLONG g_prodActiveMs = 0;
static volatile LONGLONG g_consActiveMs = 0;

static int g_prodMin = 0, g_prodMax = 100;
static int g_consMin = 50, g_consMax = 150;

static volatile LONG g_stopFlag = 0;

static volatile LONG g_lcgState = 0;
static LONG lcg_rand()
{
	LONG old = InterlockedExchangeAdd(&g_lcgState, 1103515245);
	unsigned long x = (unsigned long)old;
	x = (1664525u * x) + 1013904223u;
	g_lcgState = (LONG)x;
	return (LONG)x;
}

static int rand_range(int minv, int maxv)
{
	if (maxv <= minv) return minv;
	unsigned int r = (unsigned int)lcg_rand();
	return minv + (int)(r % (unsigned int)(maxv - minv + 1));
}

static ULONGLONG now_ms()
{
	return GetTickCount64();
}
static void print_msg(const char* fmt, ...)
{
	char buf[512];
	char out[640];
	SYSTEMTIME st;
	GetLocalTime(&st);

	va_list args;
	va_start(args, fmt);
	_vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, args);
	va_end(args);

	_snprintf_s(out, sizeof(out), _TRUNCATE, "%s\n", buf);
	printf("%s", out);
}

DWORD WINAPI ProducerThread(LPVOID lp)
{
	UNREFERENCED_PARAMETER(lp);


	while (!InterlockedCompareExchange(&g_stopFlag, 0, 0)) {
		// Симулируем время производства
		ULONGLONG t0 = now_ms();
		int delay = rand_range(g_prodMin, g_prodMax);
		Sleep((DWORD)delay);
		ULONGLONG t1 = now_ms();
		// учитываем это как "активное" время производителя (симуляция работы)
		InterlockedExchangeAdd64(&g_prodActiveMs, (LONGLONG)(t1 - t0));


		// Попытка положить в буфер
		ULONGLONG enterWaitStart = now_ms();
		EnterCriticalSection(&g_cs);
		ULONGLONG entered = now_ms();
		// Время ожидания входа в CS (entered - enterWaitStart) не учитываем как активное


		// Если буфер полон — будем ждать на условной переменной.
		while (g_count == g_capacity) {
			print_msg("Производитель: буфер полон, блокировка производителя");
			// перед тем как ждать — освобождаем CS и спим на CV; время ожидания не учитываем
			BOOL waitRes = SleepConditionVariableCS(&g_notFull, &g_cs, INFINITE);
			if (waitRes) {
				print_msg("Пробуждение потоков (после освобождения места) - Producer awakened");
			}
			if (InterlockedCompareExchange(&g_stopFlag, 0, 0)) break;
		}

		if (InterlockedCompareExchange(&g_stopFlag, 0, 0)) {
			LeaveCriticalSection(&g_cs);
			break;
		}

		ULONGLONG csStart = now_ms();

		int producedId = (int)InterlockedIncrement(&g_totalProduced);
		g_buffer[g_tail] = producedId;
		g_tail = (g_tail + 1) % g_capacity;
		g_count++;

		ULONGLONG prodTime = now_ms();
		if (g_lastProdTime != 0) {
			g_sumProdIntervals += (double)(prodTime - g_lastProdTime);
		}
		g_lastProdTime = prodTime;
		ULONGLONG csEnd = now_ms();
		InterlockedExchangeAdd64(&g_prodActiveMs, (LONGLONG)(csEnd - csStart));
		print_msg("Успешное добавление элемента: id=%d, тек.размер=%d", producedId, g_count);

		// сигнал потребителю
		WakeConditionVariable(&g_notEmpty);
		LeaveCriticalSection(&g_cs);
	}


	return 0;
}
DWORD WINAPI ConsumerThread(LPVOID lp)
{
	UNREFERENCED_PARAMETER(lp);

	while (!InterlockedCompareExchange(&g_stopFlag, 0, 0)) {
		// Симулируем время обработки (вне CS) — считаем его как активное
		ULONGLONG t0 = now_ms();
		int delay = rand_range(g_consMin, g_consMax);
		Sleep((DWORD)delay);
		ULONGLONG t1 = now_ms();
		InterlockedExchangeAdd64(&g_consActiveMs, (LONGLONG)(t1 - t0));


		ULONGLONG enterWaitStart = now_ms();
		EnterCriticalSection(&g_cs);
		ULONGLONG entered = now_ms();
		// ожидание входа в CS не учитываем


		while (g_count == 0) {
			print_msg("Потребитель: буфер пуст, блокировка потребителя");
			BOOL waitRes = SleepConditionVariableCS(&g_notEmpty, &g_cs, INFINITE);
			if (waitRes) {
				print_msg("Пробуждение потоков (после появления элемента) - Consumer awakened");
			}
			if (InterlockedCompareExchange(&g_stopFlag, 0, 0)) break;
		}


		if (InterlockedCompareExchange(&g_stopFlag, 0, 0)) {
			LeaveCriticalSection(&g_cs);
			break;
		}


		ULONGLONG csStart = now_ms();


		int id = g_buffer[g_head];
		g_head = (g_head + 1) % g_capacity;
		g_count--;
		InterlockedIncrement(&g_totalConsumed);


		ULONGLONG consTime = now_ms();
		if (g_lastConsTime != 0) {
			g_sumConsIntervals += (double)(consTime - g_lastConsTime);
		}
		g_lastConsTime = consTime;


		ULONGLONG csEnd = now_ms();
		InterlockedExchangeAdd64(&g_consActiveMs, (LONGLONG)(csEnd - csStart));


		print_msg("Успешное извлечение элемента: id=%d, тек.размер=%d", id, g_count);


		// сигнал производителю
		WakeConditionVariable(&g_notFull);


		LeaveCriticalSection(&g_cs);
	}


	return 0;
}

DWORD WINAPI StatsThread(LPVOID lp)
{
	UNREFERENCED_PARAMETER(lp);
	const DWORD periodMs = 5000;
	LONG prevTotalProd = 0;
	LONG prevTotalCons = 0;
	ULONGLONG start = g_appStartTime;


	while (!InterlockedCompareExchange(&g_stopFlag, 0, 0)) {
		Sleep(periodMs);


		ULONGLONG now = now_ms();
		double runSec = (double)(now - start) / 1000.0;

		LONG totalP = InterlockedCompareExchange(&g_totalProduced, 0, 0);
		LONG totalC = InterlockedCompareExchange(&g_totalConsumed, 0, 0);
		int curCount = 0;
		EnterCriticalSection(&g_cs);
		curCount = g_count;
		LeaveCriticalSection(&g_cs);
		int producedInPeriod = totalP - prevTotalProd;
		int consumedInPeriod = totalC - prevTotalCons;
		double avgProdInterval = (totalP > 1) ? (g_sumProdIntervals / (double)(totalP - 1)) : 0.0;
		double avgConsInterval = (totalC > 1) ? (g_sumConsIntervals / (double)(totalC - 1)) : 0.0;
		double prodSpeed = (runSec > 0.0) ? (double)totalP / runSec : 0.0; // elements/sec
		double consSpeed = (runSec > 0.0) ? (double)totalC / runSec : 0.0;
		double fillPercent = (g_capacity > 0) ? (100.0 * (double)curCount / (double)g_capacity) : 0.0;
		LONGLONG prodActiveMs = InterlockedCompareExchange64(&g_prodActiveMs, 0, 0);
		LONGLONG consActiveMs = InterlockedCompareExchange64(&g_consActiveMs, 0, 0);


		print_msg("--- Статистика за период ---");
		print_msg("Время работы приложения: %.2f сек", runSec);
		print_msg("Всего произведено: %d", totalP);
		print_msg("Всего потреблено: %d", totalC);
		print_msg("Текущий размер буфера: %d", curCount);
		print_msg("Процент заполнения: %.2f%%", fillPercent);
		print_msg("Произведено за последний период: %d", producedInPeriod);
		print_msg("Среднее время между производствами: %.2f мс", avgProdInterval);
		print_msg("Текущая скорость производства: %.2f эл/сек (средняя)", prodSpeed);
		print_msg("Общее активное время производителя (не включая ожидания): %.2f сек", (double)prodActiveMs / 1000.0);
		print_msg("Потреблено за последний период: %d", consumedInPeriod);
		print_msg("Среднее время обработки элемента: %.2f мс", avgConsInterval);
		print_msg("Текущая скорость потребления: %.2f эл/сек (средняя)", consSpeed);
		print_msg("Общее активное время потребителя (не включая ожидания): %.2f сек", (double)consActiveMs / 1000.0);

		prevTotalProd = totalP;
		prevTotalCons = totalC;
	}


	return 0;
}

DWORD WINAPI WaitEnterThread(LPVOID lp)
{
	UNREFERENCED_PARAMETER(lp);
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	if (hStdin == INVALID_HANDLE_VALUE) return 0;
	char buf[8];
	DWORD read = 0;
	ReadConsoleA(hStdin, buf, 1, &read, NULL);

	InterlockedExchange(&g_stopFlag, 1);
	WakeAllConditionVariable(&g_notEmpty);
	WakeAllConditionVariable(&g_notFull);
	return 0;
}

int main(int argc, char* argv[])
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	if (argc < 6) {
		printf("Usage: %s [buffer] [pMin] [pMax] [cMin] [cMax]\n", argv[0]);
		return 1;
	}

	g_capacity = atoi(argv[1]);
	g_prodMin = atoi(argv[2]);
	g_prodMax = atoi(argv[3]);
	g_consMin = atoi(argv[4]);
	g_consMax = atoi(argv[5]);

	if (g_capacity <= 0) {
		printf("Неверный размер буфера\n");
		return 1;
	}

	g_buffer = (int*)HeapAlloc(GetProcessHeap(), 0, sizeof(int) * g_capacity);
	if (!g_buffer) {
		printf("Не удалось выделить буфер\n");
		return 1;
	}

	InitializeCriticalSection(&g_cs);
	InitializeConditionVariable(&g_notFull);
	InitializeConditionVariable(&g_notEmpty);

	// инициировать LCG
	g_lcgState = (LONG)(now_ms() & 0xFFFFFFFF);

	// стартовые значения
	g_appStartTime = now_ms();
	g_prodStartTime = 0;
	g_consStartTime = 0;

	printf("Текущий размер буфера: %d\n", g_count);
	printf("Максимальный размер буфера: %d\n", g_capacity);
	printf("Нажмите любую клавишу (Enter) для остановки приложения...\n");

	// создаём потоки
	HANDLE hProd = CreateThread(NULL, 0, ProducerThread, NULL, 0, NULL);
	HANDLE hCons = CreateThread(NULL, 0, ConsumerThread, NULL, 0, NULL);
	HANDLE hStats = CreateThread(NULL, 0, StatsThread, NULL, 0, NULL);
	HANDLE hWait = CreateThread(NULL, 0, WaitEnterThread, NULL, 0, NULL);

	// ждём завершения (после установки г_stopFlag потоки сами завершатся)
	WaitForSingleObject(hProd, INFINITE);
	WaitForSingleObject(hCons, INFINITE);

	// остановить также статистический поток
	InterlockedExchange(&g_stopFlag, 1);
	WaitForSingleObject(hStats, INFINITE);

	// освобождение ресурсов
	CloseHandle(hProd);
	CloseHandle(hCons);
	CloseHandle(hStats);
	CloseHandle(hWait);

	DeleteCriticalSection(&g_cs);
	HeapFree(GetProcessHeap(), 0, g_buffer);

	printf("Приложение завершено.\n");
	return 0;
}
