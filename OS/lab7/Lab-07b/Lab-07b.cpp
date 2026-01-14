#include <windows.h>
#include <iostream>

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	ULONGLONG startTime = GetTickCount64();
	ULONGLONG currentTime;

	unsigned long long counter = 0;

	bool printed5 = false;
	bool printed10 = false;

	while (true)
	{
		counter++;

		currentTime = GetTickCount64();
		ULONGLONG elapsed = currentTime - startTime;

		if (elapsed == 5000 || elapsed == 10000 || elapsed >= 15000) {
			if (!printed5 && elapsed == 5000)
			{
				std::cout << "Кол-во итераций после 5 секунд: " << counter << std::endl;
				printed5 = true;
			}

			if (!printed10 && elapsed == 10000)
			{
				std::cout << "Кол-во итераций после 10 секунд: " << counter << std::endl;
				printed10 = true;
			}

			if (elapsed >= 15000)
			{
				std::cout << "Кол-во итераций после 15 секунд: " << counter << std::endl;
				break;
			}
		}
	}

}
