#include "HT.h"
#include <iostream>

#define CREATION_ENABLED
//#define OPENING_ENABLED
#define INSERTION_ENABLED
#define DELETION_ENABLED
#define CLOSURE_ENABLED
#define GET_ENABLED
#define UPDATE_ENABLED
#define EXECUTION_ENABLED
using namespace std;


int main() {
	setlocale(LC_ALL, "rus");
	HT::HTHANDLE* handle = nullptr;
	try {


#ifdef CREATION_ENABLED
		wcout << L"\n--- СОЗДАНИЕ ХРАНИЛИЩА ---" << endl;
		handle = HT::Create(1000, 3, 10, 256, "Test.ht");

		if (handle == NULL) {
			wcout << L"Ошибка: Не удалось создать хранилище" << endl;
			return 1;
		}
		else {
			wcout << L"Максимальное количество элементов: " << handle->Capacity << endl;
		}



#endif // CREATION_ENABLED



#ifdef OPENING_ENABLED
		wcout << L"\n--- ОТКРЫТИЕ ХРАНИЛИЩА ---" << endl;
		handle = HT::Open("Test.ht");

		if (handle == NULL) {
			wcout << L"Ошибка: Не удалось открыть хранилище" << endl;
			return 1;
		}
		else {
			wcout << L"Хранилище успешно открыта!" << endl;
			wcout << L"Текущее количество элементов: " << handle->CurrentElements << endl;
		}
#endif


#ifdef INSERTION_ENABLED
		wcout << L"\n--- ДОБАВЛЕНИЕ ЭЛЕМЕНТОП ---" << endl;
		int prev = handle->CurrentElements;

		HT::Insert(handle, new HT::Element("Наушники", 8, "90", 2));
		HT::Insert(handle, new HT::Element("Барабаны", 8, "450", 3));
		HT::Insert(handle, new HT::Element("Струны", 6, "12", 2));
		HT::Insert(handle, new HT::Element("Колонки", 7, "160", 3));
		HT::Insert(handle, new HT::Element("Усилитель", 9, "250", 3));
		HT::Insert(handle, new HT::Element("Гитара", 6, "620", 3));
		HT::Insert(handle, new HT::Element("Медиатор", 8, "5", 1));
		HT::Insert(handle, new HT::Element("Плеер", 5, "80", 2));

		wcout << L"Добавлено "<<handle->CurrentElements - prev << " элементов" << endl;
		wcout << L"Всего элементов в базе: " << handle->CurrentElements << endl;
#endif


#ifdef GET_ENABLED
		wcout << L"\n--- ПОИСК ЭЛЕМЕНТА ---" << endl;

		HT::Element* product = HT::Get(handle, new HT::Element("Гитара", 6));
		if (product != NULL) {
			wcout << L"Найден элемент: ";
			HT::Print(product);
		}
		else {
			wcout << L"Элемент не найден" << endl;
		}
#endif
#ifdef DELETION_ENABLED
		wcout << L"\n--- УДАЛЕНИЕ ЭЛЕМЕНТА ---" << endl;

		if (HT::Delete(handle, new HT::Element("Медиатор", 5))) {
			wcout << L"Элеемент удалён из хранилища" << endl;
			wcout << L"Осталось элементов: " << handle->CurrentElements << endl;
		}
		else {
			wcout << L"Ошибка удаления элемента" << endl;
		}
#endif
#ifdef UPDATE_ENABLED
		wcout << L"\n--- ИЗМЕНЕНИЕ ЭЛЕМЕНТА ---" << endl;

		if (HT::Update(handle, new HT::Element("Барабаны", 8), "500", 3)) {
			wcout << L"Значение элемента обновлено" << endl;

			HT::Element* updated = HT::Get(handle, new HT::Element("Барабаны",8));
			if (updated != NULL) {
				wcout << L"Новое значение: ";
				HT::Print(updated);
			}
		}
		else {
			wcout << L"Ошибка изменения элемента" << endl;
		}
#endif


#ifdef CLOSURE_ENABLED
		if (HT::Close(handle)) {
			cout << "--ХРАНИЛИЩЕ ЗАКРЫТО--" << endl;
		}
		else {
			cout << "--НЕ УДАЛОСЬ ЗАКРЫТЬ ХРАНИЛИЩЕ--" << endl;
		}
#endif // CLOSURE_ENABLED
#ifdef EXECUTION_ENABLED

		HT::ExecuteHT();
#endif // EXECUTION_ENABLED


	}
	catch (exception ex) {
		cout << ex.what() << endl;

	}


	return 0;

}