#include "OS_10_2.h"
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
    HT::HTHANDLE* store1 = nullptr;  // Первое хранилище - музыкальные инструменты
    HT::HTHANDLE* store2 = nullptr;  // Второе хранилище - электроника

    try {
#ifdef CREATION_ENABLED
        wcout << L"\n--- СОЗДАНИЕ ХРАНИЛИЩ ---" << endl;

        // Создаем первое хранилище для музыкальных инструментов
        wcout << L"\n1. СОЗДАНИЕ ХРАНИЛИЩА:" << endl;
        store1 = HT::Create(500, 3, 30, 20, "Store.ht");
        if (store1 == NULL) {
            wcout << L"Ошибка: Не удалось создать хранилище " << endl;
            return 1;
        }
        else {
            wcout << L"✓ Хранилище инструментов создано" << endl;
            wcout << L"  Максимальная емкость: " << store1->Capacity << L" элементов" << endl;
        }

        // Создаем второе хранилище для электроники
        wcout << L"\n2. ОТКРЫТИЕ ХРАНИЛИЩА:" << endl;
        store2 = HT::Create(500, 3, 30, 20, "Store1.ht");
        if (store2 == NULL) {
            wcout << L"Ошибка: Не удалось создать хранилище " << endl;
            HT::Close(store1); 
            return 1;
        }
        else {
            wcout << L" Хранилище открыто" << endl;
            wcout << L" Максимальная емкость: " << store2->Capacity << L" элементов" << endl;
        }
#endif

#ifdef OPENING_ENABLED
        wcout << L"\n--- ОТКРЫТИЕ ХРАНИЛИЩ ---" << endl;

        // Закрываем и открываем хранилища для демонстрации
        if (store1) HT::Close(store1);
        if (store2) HT::Close(store2);

        wcout << L"\n1. ОТКРЫТИЕ ХРАНИЛИЩА :" << endl;
        store1 = HT::Open("Store.ht");
        if (store1 == NULL) {
            wcout << L"Ошибка: Не удалось открыть хранилище" << endl;
            return 1;
        }
        else {
            wcout << L"✓ Хранилище открыто" << endl;
            wcout << L"  Текущее количество элементов: " << store1->CurrentElements << endl;
        }

        wcout << L"\n2. ОТКРЫТИЕ ХРАНИЛИЩА:" << endl;
        store2 = HT::Open("Store1.ht");
        if (store2 == NULL) {
            wcout << L"Ошибка: Не удалось открыть хранилище" << endl;
            HT::Close(store1);
            return 1;
        }
        else {
            wcout << L"✓ Хранилище открыто" << endl;
            wcout << L"  Текущее количество элементов: " << store2->CurrentElements << endl;
        }
#endif

#ifdef INSERTION_ENABLED
        wcout << L"\n--- ДОБАВЛЕНИЕ ДАННЫХ В ОБА ХРАНИЛИЩА ---" << endl;

        wcout << L"\n1. ДОБАВЛЕНИЕ В 1-ОЕ ХРАНИЛИЩЕ:" << endl;
        int prev1 = store1->CurrentElements;
        HT::Insert(store1, new HT::Element("Гитара", 6, "6200", 4));
        HT::Insert(store1, new HT::Element("Барабаны", 8, "4500", 4));
        HT::Insert(store1, new HT::Element("Струны", 6, "120", 3));
        HT::Insert(store1, new HT::Element("Медиатор", 8, "50", 2));
        HT::Insert(store1, new HT::Element("Саксофон", 8, "8900", 4));
        wcout << L"Добавлено " << store1->CurrentElements - prev1 << L" элементов" << endl;

        wcout << L"\n2. ДОБАВЛЕНИЕ В 2-ОЕ ХРАНИЛИЩЕ:" << endl;
        int prev2 = store2->CurrentElements;
        HT::Insert(store2, new HT::Element("Наушники", 8, "900", 3));
        HT::Insert(store2, new HT::Element("Колонки", 7, "1600", 4));
        HT::Insert(store2, new HT::Element("Усилитель", 9, "2500", 4));
        HT::Insert(store2, new HT::Element("Плеер", 5, "800", 3));
        HT::Insert(store2, new HT::Element("Микрофон", 8, "1200", 4));
        wcout << L"Добавлено " << store2->CurrentElements - prev2 << L" элементов" << endl;

        wcout << L"\nИТОГО:" << endl;
        wcout << L"• В 1 хранилище : " << store1->CurrentElements << L" элементов" << endl;
        wcout << L"• В 2 хранилище : " << store2->CurrentElements << L" элементов" << endl;
#endif

#ifdef GET_ENABLED
        wcout << L"\n--- ПОИСК ДАННЫХ В ОБОИХ ХРАНИЛИЩАХ ---" << endl;

        wcout << L"\n1. ПОИСКВ 1-ОМ ХРАНИЛИЩЕ:" << endl;
        HT::Element* instrument = HT::Get(store1, new HT::Element("Гитара", 6));
        if (instrument != NULL) {
            wcout << L"Найден элемент: ";
            HT::Print(instrument);
        }
        else {
            wcout << L" Элемент не найден" << endl;
        }

        wcout << L"\n2. ПОИСК В 2-ОМ ХРАНИЛИЩЕ:" << endl;
        HT::Element* device = HT::Get(store2, new HT::Element("Наушники", 8));
        if (device != NULL) {
            wcout << L"Найден элемент: ";
            HT::Print(device);
        }
        else {
            wcout << L"Элемент не найден" << endl;
        }
#endif

#ifdef UPDATE_ENABLED
        wcout << L"\n--- ОБНОВЛЕНИЕ ЗНАЧЕНИЙ В ОБОИХ ХРАНИЛИЩАХ ---" << endl;

        wcout << L"\n1. ОБНОВЛЕНИЕ ЗНАЧЕНИЯ В 1-ОМ ХРАНИЛИЩЕ:" << endl;
        if (HT::Update(store1, new HT::Element("Барабаны", 8), "4800", 4)) {
            wcout << L"Значение обновлено в 1-ом хранилище" << endl;
            HT::Element* updated = HT::Get(store1, new HT::Element("Барабаны", 8));
            if (updated != NULL) {
                wcout << L"Новое значение: ";
                HT::Print(updated);
            }
        }
        else {
            wcout << L"Ошибка обновления значения в 1-ом хранилище" << endl;
        }

        wcout << L"\n2. ОБНОВЛЕНИЕ ЗНАЧЕНИЯ В 2-ОМ ХРАНИЛИЩЕ:" << endl;
        if (HT::Update(store2, new HT::Element("Колонки", 7), "1400", 4)) {
            wcout << L"Значение обновлено в 2-ом хранилище" << endl;
            HT::Element* updated = HT::Get(store2, new HT::Element("Колонки", 7));
            if (updated != NULL) {
                wcout << L"Новое значение: ";
                HT::Print(updated);
            }
        }
        else {
            wcout << L"Ошибка обновления значения в 2-ом хранилище" << endl;
        }
#endif

#ifdef DELETION_ENABLED
        wcout << L"\n--- УДАЛЕНИЕ ДАННЫХ ИЗ ОБОИХ ХРАНИЛИЩ ---" << endl;

        wcout << L"\n1. УДАЛЕНИЕ ЭЛЕМЕНТА В 1-ОМ ХРАНИЛИЩЕ:" << endl;
        if (HT::Delete(store1, new HT::Element("Медиатор", 8))) {
            wcout << L" Элемент удален из хранилища" << endl;
            wcout << L" Осталось элементов: " << store1->CurrentElements << endl;
        }
        else {
            wcout << L" Ошибка удаления элемента" << endl;
        }

        wcout << L"\n2. УДАЛЕНИЕ ЭЛЕМЕНТА В 2-ОМ ХРАНИЛИЩЕ:" << endl;
        if (HT::Delete(store2, new HT::Element("Плеер", 5))) {
            wcout << L" Элемент удален из хранилища" << endl;
            wcout << L" Осталось устройств: " << store2->CurrentElements << endl;
        }
        else {
            wcout << L" Ошибка удаления элемента" << endl;
        }
#endif

#ifdef CLOSURE_ENABLED
        wcout << L"\n--- ЗАКРЫТИЕ ХРАНИЛИЩ ---" << endl;

        bool success = true;

        wcout << L"\n1. ЗАКРЫТИЕ ХРАНИЛИЩА 1:" << endl;
        if (HT::Close(store1)) {
            wcout << L" Хранилище 1 закрыто " << endl;
        }
        else {
            wcout << L"✗ Ошибка закрытия хранилища 1" << endl;
            success = false;
        }

        wcout << L"\n2. ЗАКРЫТИЕ ХРАНИЛИЩА 2:" << endl;
        if (HT::Close(store2)) {
            wcout << L" Хранилище 2 закрыто " << endl;
        }
        else {
            wcout << L"Ошибка закрытия хранилища 2" << endl;
            success = false;
        }

        if (success) {
            wcout << L"\n✓ ОБА ХРАНИЛИЩА УСПЕШНО ЗАКРЫТЫ" << endl;
        }
#endif

#ifdef EXECUTION_ENABLED
        wcout << L"\n--- ЗАПУСК СЛУЖБЫ ---" << endl;
        wcout << L"Для демонстрации работы с двумя хранилищами эта функция отключена" << endl;
        // HT::ExecuteHT(); // Не запускаем для ясности демонстрации
#endif

    }
    catch (exception ex) {
        wcout << L"\n✗ Произошла ошибка: " << ex.what() << endl;
        // Пытаемся корректно закрыть оба хранилища при ошибке
        if (store1) HT::Close(store1);
        if (store2) HT::Close(store2);
    }

    wcout << L"\n--- РАБОТА ПРОГРАММЫ ЗАВЕРШЕНА ---" << endl;
    return 0;
}