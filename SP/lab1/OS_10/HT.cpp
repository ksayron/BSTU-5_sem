#include <iostream>
#include "HT.h"
#include <mutex>
#include <thread>
#include <cstring>
#include <Windows.h>
#include <ctime>

using namespace std;

namespace HT {

    std::mutex ht_mutex;

    // ---------------- Element ----------------
    Element::Element() :
        key(nullptr),
        keylength(0),
        payload(nullptr),
        payloadlength(0) {
    }

    Element::Element(const void* key, int keylength) :
        key(key),
        keylength(keylength),
        payload(nullptr),
        payloadlength(0) {
    }

    Element::Element(const void* key, int keylength, const void* payload, int payloadlength) :
        key(key),
        keylength(keylength),
        payload(payload),
        payloadlength(payloadlength) {
    }

    Element::Element(Element* oldelement, const void* newpayload, int newpayloadlength) :
        key(oldelement->key),
        keylength(oldelement->keylength),
        payload(newpayload),
        payloadlength(newpayloadlength) {
    }

    // ---------------- HTHANDLE ----------------
    HTHANDLE::HTHANDLE() :
        Capacity(0),
        SecSnapshotInterval(0),
        MaxKeyLength(0),
        MaxPayloadLength(0),
        File(NULL),
        FileMapping(NULL),
        Addr(NULL),
        lastsnaptime(0),
        CurrentElements(0) {
    }

    HTHANDLE::HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512])
        : Capacity(Capacity),
        SecSnapshotInterval(SecSnapshotInterval),
        MaxKeyLength(MaxKeyLength),
        MaxPayloadLength(MaxPayloadLength),
        lastsnaptime(0),
        File(NULL),
        FileMapping(NULL),
        Addr(NULL),
        CurrentElements(0)
    {
        strcpy_s(this->FileName, 512, FileName);
    }

    // ---------------- Create ----------------
    HTHANDLE* Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512]) {
        lock_guard<mutex> lock(ht_mutex);

        HTHANDLE* ht = new HTHANDLE(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName);
        cout << "=== Создание HT-хранилища ===" << endl;

        ht->File = CreateFileA(FileName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (ht->File == INVALID_HANDLE_VALUE) {
            cout << "Ошибка: не удалось создать/открыть файл (" << GetLastError() << ")" << endl;
            delete ht;
            return NULL;
        }

        int storage_size = ht->Capacity * (ht->MaxKeyLength + ht->MaxPayloadLength);

        ht->FileMapping = CreateFileMappingA(
            ht->File,
            NULL,
            PAGE_READWRITE,
            0,
            storage_size,
            "HtMapping");

        if (ht->FileMapping == NULL) {
            cout << "Ошибка: не удалось создать FileMapping (" << GetLastError() << ")" << endl;
            CloseHandle(ht->File);
            delete ht;
            return NULL;
        }

        ht->Addr = MapViewOfFile(ht->FileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if (ht->Addr == NULL) {
            cout << "Ошибка: не удалось отобразить файл в память (" << GetLastError() << ")" << endl;
            CloseHandle(ht->FileMapping);
            CloseHandle(ht->File);
            delete ht;
            return NULL;
        }



        cout << "Хранилище успешно создано." << endl;
        return ht;
    }

    // ---------------- Open ----------------
    HTHANDLE* Open(const char FileName[512]) {
        cout << "=== Открытие HT-хранилища ===" << endl;
        HTHANDLE* ht = new HTHANDLE();

        ht->File = CreateFileA(FileName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (ht->File == INVALID_HANDLE_VALUE) {
            cout << "Ошибка: не удалось открыть файл (" << GetLastError() << ")" << endl;
            delete ht;
            return NULL;
        }

        ht->FileMapping = CreateFileMappingA(ht->File, NULL, PAGE_READWRITE, 0, 0, "HtMapping");
        if (ht->FileMapping == NULL) {
            cout << "Ошибка: не удалось открыть FileMapping (" << GetLastError() << ")" << endl;
            CloseHandle(ht->File);
            delete ht;
            return NULL;
        }

        ht->Addr = MapViewOfFile(ht->FileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if (ht->Addr == NULL) {
            cout << "Ошибка: не удалось отобразить файл (" << GetLastError() << ")" << endl;
            CloseHandle(ht->FileMapping);
            CloseHandle(ht->File);
            delete ht;
            return NULL;
        }

        cout << "Хранилище успешно открыто." << endl;
        return ht;
    }

    // ---------------- Snap ----------------
    const char* CreateSnapshotFileName(HTHANDLE* handle) {
        static char buffer[100];
        char time_buffer[80];
        tm time_info;

        localtime_s(&time_info, &handle->lastsnaptime);
        strftime(time_buffer, sizeof(time_buffer), "%Y%m%d_%H%M%S", &time_info);
        snprintf(buffer, sizeof(buffer), "Snapshot-%s.htsnap", time_buffer);

        return buffer;

    }

    BOOL Snap(HTHANDLE* hthandle) {

        cout << endl << "----------Snap----------" << endl;

        if (!hthandle) {
            cout << "--Snap:Failed to open the handle--" << "Error: " << GetLastError() << endl;
            return FALSE;
        }

        //not needed here if the snapshot is being executed during closure procedure
        //lock_guard<mutex>lock(ht_mutex)

        hthandle->lastsnaptime = time(nullptr);

        HANDLE HTSnapshot = CreateFileA(
            CreateSnapshotFileName(hthandle),
            //"Snapshotfile.htsnap",
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );


        if (HTSnapshot == INVALID_HANDLE_VALUE) {
            cout << "--Snap:Failed to create a snapshot file--" << "Error: " << GetLastError() << endl;
            return FALSE;
        }
        else {
            cout << "--Snap:Snapshot file created--" << endl;
        }

        SIZE_T data_size = hthandle->CurrentElements * (hthandle->MaxKeyLength + hthandle->MaxPayloadLength);
        cout << "--Data size to write: " << data_size << "--" << endl;

        cout << "--Buffer size: " << sizeof(hthandle->Addr) << "--" << endl;

        DWORD bytesWritten;

        BOOL writeResult = WriteFile(
            HTSnapshot,
            hthandle->Addr,
            data_size,
            &bytesWritten,
            NULL
        );

        if (!writeResult) {
            DWORD writeEror = GetLastError();
            cout << "--Snap:Failed to execute a snapshot(WriteFile error)--" << " Error: " << writeEror << endl;
            cout << endl << "----------End----------" << endl;
            CloseHandle(HTSnapshot);
            return FALSE;
        }
        else if (bytesWritten != data_size) {
            DWORD bytesError = GetLastError();
            cout << "--Snap:failed to execute a snapshot(Bytes loss)--" << " Error: " << bytesError << endl;
            cout << endl << "----------End----------" << endl;
            CloseHandle(HTSnapshot);
            return FALSE;
        }
        else {
            cout << "--Snap: Snapshot Executed--" << endl;
        }

        CloseHandle(HTSnapshot);
        return TRUE;
        cout << endl << "----------End----------" << endl;

    }

    // ---------------- Close ----------------
    BOOL Close(HTHANDLE* hthandle) {
        if (!hthandle) return FALSE;
        lock_guard<mutex>lock(ht_mutex);
        Snap(hthandle);

        if (hthandle->Addr) UnmapViewOfFile(hthandle->Addr);
        if (hthandle->FileMapping) CloseHandle(hthandle->FileMapping);
        if (hthandle->File) CloseHandle(hthandle->File);

        cout << "Хранилище закрыто." << endl;
        delete hthandle;
        return TRUE;
    }

    // ---------------- Insert ----------------
    BOOL Insert(HTHANDLE* hthandle, const Element* element) {
        if (!hthandle || !hthandle->Addr || !element || !element->key || !element->payload)
            return FALSE;

        if (hthandle->CurrentElements >= hthandle->Capacity) return FALSE;

        char* base = static_cast<char*>(hthandle->Addr) +
            hthandle->CurrentElements * (hthandle->MaxKeyLength + hthandle->MaxPayloadLength);

        memcpy(base, element->key, element->keylength);
        memset(base + element->keylength, 0, hthandle->MaxKeyLength - element->keylength);

        memcpy(base + hthandle->MaxKeyLength, element->payload, element->payloadlength);
        memset(base + hthandle->MaxKeyLength + element->payloadlength, 0,
            hthandle->MaxPayloadLength - element->payloadlength);

        hthandle->CurrentElements++;
        cout << "Элемент вставлен." << endl;

        if (hthandle->SecSnapshotInterval > 0) {
            time_t timeSinceLastSnapshot = time(NULL) - hthandle->lastsnaptime;
            if (timeSinceLastSnapshot >= hthandle->SecSnapshotInterval) {
                lock_guard<mutex>lock(ht_mutex);
                Snap(hthandle);
            }
        }
        return TRUE;
    }

    // ---------------- Delete ----------------
    BOOL Delete(HTHANDLE* hthandle, const Element* element) {
        if (!hthandle || !hthandle->Addr || !element || !element->key) return FALSE;

        size_t slot = hthandle->MaxKeyLength + hthandle->MaxPayloadLength;
        int index = -1;

        for (int i = 0; i < hthandle->CurrentElements; i++) {
            char* base = static_cast<char*>(hthandle->Addr) + i * slot;
            if (memcmp(base, element->key, element->keylength) == 0) {
                index = i;
                break;
            }
        }

        if (index == -1) return FALSE;

        for (int i = index; i < hthandle->CurrentElements - 1; i++) {
            char* dst = static_cast<char*>(hthandle->Addr) + i * slot;
            char* src = static_cast<char*>(hthandle->Addr) + (i + 1) * slot;
            memcpy(dst, src, slot);
        }

        hthandle->CurrentElements--;
        cout << "Элемент удалён." << endl;

        if (hthandle->SecSnapshotInterval > 0) {
            time_t timeSinceLastSnapshot = time(NULL) - hthandle->lastsnaptime;
            if (timeSinceLastSnapshot >= hthandle->SecSnapshotInterval) {
                lock_guard<mutex>lock(ht_mutex);
                Snap(hthandle);
            }
        }
        return TRUE;
    }

    // ---------------- Get ----------------
    Element* Get(HTHANDLE* hthandle, const Element* element) {
        if (!hthandle) return NULL;
        if (!hthandle || !hthandle->Addr || !element || !element->key) {
            if (hthandle->SecSnapshotInterval > 0) {
                time_t timeSinceLastSnapshot = time(NULL) - hthandle->lastsnaptime;
                if (timeSinceLastSnapshot >= hthandle->SecSnapshotInterval) {
                    lock_guard<mutex>lock(ht_mutex);
                    Snap(hthandle);
                }
            }
            return NULL;
        }

        size_t slot = hthandle->MaxKeyLength + hthandle->MaxPayloadLength;

        for (int i = 0; i < hthandle->CurrentElements; i++) {
            char* base = static_cast<char*>(hthandle->Addr) + i * slot;
            if (memcmp(base, element->key, element->keylength) == 0) {
                return new Element(base, element->keylength, base + hthandle->MaxKeyLength, hthandle->MaxPayloadLength);
            }
        }

        if (hthandle->SecSnapshotInterval > 0) {
            time_t timeSinceLastSnapshot = time(NULL) - hthandle->lastsnaptime;
            if (timeSinceLastSnapshot >= hthandle->SecSnapshotInterval) {
                lock_guard<mutex>lock(ht_mutex);
                Snap(hthandle);
            }
        }
       
        return NULL;
    }

    // ---------------- Update ----------------
    BOOL Update(HTHANDLE* hthandle, const Element* oldelement, const void* newpayload, int newpayloadlength) {
        if (!hthandle || !hthandle->Addr || !oldelement || !oldelement->key || !newpayload) return FALSE;
        if (newpayloadlength > hthandle->MaxPayloadLength) return FALSE;

        size_t slot = hthandle->MaxKeyLength + hthandle->MaxPayloadLength;

        for (int i = 0; i < hthandle->CurrentElements; i++) {
            char* base = static_cast<char*>(hthandle->Addr) + i * slot;
            if (memcmp(base, oldelement->key, oldelement->keylength) == 0) {
                memcpy(base + hthandle->MaxKeyLength, newpayload, newpayloadlength);
                memset(base + hthandle->MaxKeyLength + newpayloadlength, 0,
                    hthandle->MaxPayloadLength - newpayloadlength);
                cout << "Элемент обновлён." << endl;
                return TRUE;
            }
        }
        return FALSE;

        if (hthandle->SecSnapshotInterval > 0) {
            time_t timeSinceLastSnapshot = time(NULL) - hthandle->lastsnaptime;
            if (timeSinceLastSnapshot >= hthandle->SecSnapshotInterval) {
                lock_guard<mutex>lock(ht_mutex);
                Snap(hthandle);
            }
        }
    }

    // ---------------- GetLastErrorMsg ----------------
    char* GetLastErrorMsg(HTHANDLE* ht) {
        return ht ? ht->LastErrorMessage : (char*)"HTHANDLE не найден";
    }

    // ---------------- Print ----------------
    void Print(const Element* element) {
        cout << "Ключ: " << (const char*)element->key
            << " | Значение: " << (const char*)element->payload << endl;
    }

    // ---------------- ExecuteHT ----------------
    void ExecuteHT() {
        HTHANDLE* handle = nullptr;
        int choice;

        do {
            cout << "\n=== Меню HT ===\n"
                << "1. Создать\n"
                << "2. Открыть\n"
                << "3. Вставить\n"
                << "4. Найти\n"
                << "5. Обновить\n"
                << "6. Удалить\n"
                << "7. Snapshot\n"
                << "8. Закрыть\n"
                << "0. Выход\n"
                << "Ваш выбор: ";
            cin >> choice;

            switch (choice) {
            case 1:
                handle = Create(100, 3, 16, 256, "Test.ht");
                break;
            case 2:
                handle = Open("Test.ht");
                break;
            case 3: {
                char key[16], value[256];
                cout << "Ключ: "; cin >> key;
                cout << "Значение: "; cin >> value;
                Element el(key, strlen(key) + 1, value, strlen(value) + 1);
                Insert(handle, &el);
                break;
            }
            case 4: {
                char key[16];
                cout << "Ключ: "; cin >> key;
                Element el(key, strlen(key) + 1);
                Element* res = Get(handle, &el);
                if (res) Print(res); else cout << "Элемент не найден." << endl;
                break;
            }
            case 5: {
                char key[16], value[256];
                cout << "Ключ: "; cin >> key;
                cout << "Новое значение: "; cin >> value;
                Element oldel(key, strlen(key) + 1);
                Update(handle, &oldel, value, strlen(value) + 1);
                break;
            }
            case 6: {
                char key[16];
                cout << "Ключ: "; cin >> key;
                Element el(key, strlen(key) + 1);
                Delete(handle, &el);
                break;
            }
            case 7:
                Snap(handle);
                break;
            case 8:
                Close(handle);
                handle = nullptr;
                break;
            }
        } while (choice != 0);
    }

}
