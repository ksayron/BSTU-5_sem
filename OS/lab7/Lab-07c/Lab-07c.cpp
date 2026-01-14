#include <windows.h>
#include <iostream>

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	HANDLE hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
	if (!hTimer)
		return 1;

	LARGE_INTEGER dueTime;
	dueTime.QuadPart = -3LL * 10000000LL;

	SetWaitableTimer(hTimer, &dueTime, 0, NULL, NULL, FALSE);

	unsigned long long counter = 0;
	int ticks = 0;

	while (true)
	{
		counter++;

		if (WaitForSingleObject(hTimer, 0) == WAIT_OBJECT_0)
		{
			ticks++;

			if (ticks < 5)
			{
				std::cout << "Кол-во итераций после " << ticks * 3 << " секунд: " << counter << std::endl;
			}
			else
			{
				std::cout << "Кол-во итераций после " << ticks * 3 << " секунд: " << counter << std::endl;	
				break;
			}

			SetWaitableTimer(hTimer, &dueTime, 0, NULL, NULL, FALSE);
		}
	}
	CloseHandle(hTimer);
	return 0;
}
