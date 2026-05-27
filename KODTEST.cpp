//=============================================================================
// Работа по ознакомительной практики
// Вариант: 3
// Студент: Бабиян Г. А.
// Группа: M30-111БВ-25
// Описание: Программа для учета самолетов. Выполняет сортировку по числовой
//           части бортового номера с выводом итоговой таблицы
//=============================================================================

#include <iostream>
#include <fstream>
#include <clocale>

using namespace std;

// Константы
const int MAX_RECORDS = 100;        // максимальное количество записей
const int MAX_STR_LEN = 100;        // максимальная длина строки
const int MAX_TESTS = 20;           // общее количество тестов

// Структура для хранения записи о самолёте
struct Aircraft {
    char* model;        // марка ЛА (динамическая память)
    char* regNumber;    // бортовой номер (динамическая память)
    int passengers;     // количество пассажиров
    double cargo;       // груз
};

// Структура для хранения ошибок
struct ErrorRecord {
    int recordNumber;       // номер записи (начиная с 1)
    char fieldName[50];     // название поля с ошибкой
    char errorValue[100];   // некорректное значение
    char errorMsg[100];     // сообщение об ошибке
};

//=============================================================================
// ПРОТОТИПЫ ФУНКЦИЙ
//=============================================================================

// Вывод шапки программы
void printHeader();

// Работа со строками
int stringLength(const char* str);
void stringCopy(char* dest, const char* src);
int stringCompare(const char* str1, const char* str2);

// Проверка корректности бортового номера (формат Б-XXXX)
bool isValidRegNumber(const char* reg);

// Извлечение числовой части бортового номера
int extractNumberFromReg(const char* reg);

// Чтение строки из файла
bool readLine(ifstream& file, char* buffer, int maxLen);

// Проверка и преобразование данных
bool isAllDigits(const char* str);
int stringToInt(const char* str);
bool isValidDouble(const char* str);
double stringToDouble(const char* str);

// Удаление пробелов
void trim(char* str);

// Работа с памятью
void initAircraft(Aircraft& a);
void freeAircraft(Aircraft& a);
void copyAircraft(Aircraft& dest, const Aircraft& src);

// Ввод данных
bool inputAircraft(Aircraft& a, ifstream& file, int recordNum, ErrorRecord errors[], int& errorCount);

// Сортировка
void indexSort(Aircraft data[], int n, int indices[]);

// Преобразование чисел в строки
void intToStr(int num, char* buffer);
void doubleToStr(double num, char* buffer);

// Вывод результатов
void printErrors(ErrorRecord errors[], int errorCount);
void printTable(Aircraft data[], int indices[], int n, ErrorRecord errors[], int errorCount);

// Проверка дубликатов
void checkDuplicateRegNumbers(Aircraft data[], int n, ErrorRecord errors[], int& errorCount);

//=============================================================================
// ГЛАВНАЯ ФУНКЦИЯ
//=============================================================================
int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    
    printHeader();
    
    // Массив с именами тестовых файлов (10 позитивных + 10 негативных)
    const char* testFiles[MAX_TESTS] = {
        // Позитивные тесты (1-10)
        "test1_positive.txt",
        "test2_positive.txt",
        "test3_positive.txt",
        "test4_positive.txt",
        "test5_positive.txt",
        "test6_positive.txt",
        "test7_positive.txt",
        "test8_positive.txt",
        "test9_positive.txt",
        "test10_positive.txt",
        // Негативные тесты (11-20)
        "test11_negative.txt",
        "test12_negative.txt",
        "test13_negative.txt",
        "test14_negative.txt",
        "test15_negative.txt",
        "test16_negative.txt",
        "test17_negative.txt",
        "test18_negative.txt",
        "test19_negative.txt",
        "test20_negative.txt"
    };
    
    int successCount = 0;
    int failCount = 0;
    
    for (int testIdx = 0; testIdx < MAX_TESTS; testIdx++) {
        cout << "\n\n================================================================================" << endl;
        cout << "ОБРАБОТКА ТЕСТА " << (testIdx + 1) << "/" << MAX_TESTS << ": " << testFiles[testIdx] << endl;
        cout << "================================================================================" << endl;
        
        ifstream file(testFiles[testIdx]);
        if (!file.is_open()) {
            cerr << "ОШИБКА: не удалось открыть файл " << testFiles[testIdx] << endl;
            failCount++;
            continue;
        }
        
        // Динамическое выделение памяти
        Aircraft* aircrafts = new Aircraft[MAX_RECORDS];
        for (int i = 0; i < MAX_RECORDS; i++) {
            initAircraft(aircrafts[i]);
        }
        
        ErrorRecord* errors = new ErrorRecord[MAX_RECORDS];
        int errorCount = 0;
        int recordCount = 0;
        
        // Чтение записей
        while (recordCount < MAX_RECORDS && !file.eof()) {
            Aircraft temp;
            initAircraft(temp);
            
            if (inputAircraft(temp, file, recordCount + 1, errors, errorCount)) {
                copyAircraft(aircrafts[recordCount], temp);
                recordCount++;
            }
            freeAircraft(temp);
            
            // Пропускаем пустые строки
            while (file.peek() == '\n' || file.peek() == '\r') {
                file.get();
            }
        }
        
        file.close();
        
        if (recordCount == 0) {
            cout << "Нет данных для обработки." << endl;
            delete[] aircrafts;
            delete[] errors;
            failCount++;
            continue;
        }
        
        // Проверка дубликатов
        checkDuplicateRegNumbers(aircrafts, recordCount, errors, errorCount);
        
        // Сортировка
        int* indices = new int[MAX_RECORDS];
        indexSort(aircrafts, recordCount, indices);
        
        // Вывод таблицы
        printTable(aircrafts, indices, recordCount, errors, errorCount);
        
        // Освобождение памяти
        delete[] indices;
        for (int i = 0; i < recordCount; i++) {
            freeAircraft(aircrafts[i]);
        }
        delete[] aircrafts;
        delete[] errors;
        
        successCount++;
    }
    
    // Итоговый отчет
    cout << "\n\n================================================================================" << endl;
    cout << "ИТОГОВЫЙ ОТЧЕТ ПО ТЕСТИРОВАНИЮ" << endl;
    cout << "================================================================================" << endl;
    cout << "Всего тестов: " << MAX_TESTS << endl;
    cout << "Успешно обработано: " << successCount << endl;
    cout << "С ошибками: " << failCount << endl;
    cout << "================================================================================" << endl;
    
    return 0;
}

//=============================================================================
// РЕАЛИЗАЦИИ ФУНКЦИЙ
//=============================================================================

int stringLength(const char* str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

void stringCopy(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

int stringCompare(const char* str1, const char* str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return str1[i] - str2[i];
        }
        i++;
    }
    return str1[i] - str2[i];
}

void printHeader() {
    cout << "================================================================================" << endl;
    cout << "Работа по ознакомительной практики" << endl;
    cout << "Вариант: 3" << endl;
    cout << "Описание: Учет самолетов с сортировкой по числовой части бортового номера" << endl;
    cout << "Формат бортового номера: Б-XXXX" << endl;
    cout << "================================================================================" << endl;
}

void trim(char* str) {
    if (!str || str[0] == '\0') return;
    
    // Находим начало
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t') {
        start++;
    }
    
    // Находим конец
    int end = stringLength(str) - 1;
    while (end >= start && (str[end] == ' ' || str[end] == '\t' || str[end] == '\n' || str[end] == '\r')) {
        end--;
    }
    
    // Сдвигаем строку
    if (start > 0) {
        for (int i = 0; i <= end - start; i++) {
            str[i] = str[start + i];
        }
        str[end - start + 1] = '\0';
    } else {
        str[end + 1] = '\0';
    }
}

bool isValidRegNumber(const char* reg) {
    if (!reg || reg[0] == '\0') return false;
    
    // Проверяем длину (должно быть 6 символов: Б-XXXX)
    // В UTF-8 русская буква Б занимает 2 байта, но в char она представляется как два символа
    // Поэтому проверяем по байтам
    
    int len = stringLength(reg);
    
    // Вариант 1: строка начинается с 'Б' в кодировке UTF-8 (два байта: 0xD0 0x91)
    // или 'б' (0xD0 0xB1)
    // или в однобайтовой кодировке (Windows-1251) - 0xC1 для 'Б' и 0xE1 для 'б'
    
    // Проверяем в однобайтовой кодировке (Windows-1251, CP866)
    unsigned char firstChar = (unsigned char)reg[0];
    bool isRussianB = (firstChar == 0xC1 || firstChar == 0xE1); // Б или б в Windows-1251
    
    // Проверяем в UTF-8
    bool isRussianB_UTF8 = false;
    if (len >= 2) {
        unsigned char b1 = (unsigned char)reg[0];
        unsigned char b2 = (unsigned char)reg[1];
        // Б в UTF-8: 0xD0 0x91, б в UTF-8: 0xD0 0xB1
        if (b1 == 0xD0 && (b2 == 0x91 || b2 == 0xB1)) {
            isRussianB_UTF8 = true;
        }
    }
    
    // Если это UTF-8, то длина должна быть больше
    if (isRussianB_UTF8) {
        if (len != 7) return false; // Б-XXXX (2 байта на Б + 5 остальных)
        
        // Проверяем второй символ (должен быть '-')
        if (reg[2] != '-') return false;
        
        // Проверяем, что следующие 4 символа - цифры
        for (int i = 3; i < 7; i++) {
            if (reg[i] < '0' || reg[i] > '9') return false;
        }
        return true;
    }
    // Если это однобайтовая кодировка
    else if (isRussianB) {
        if (len != 6) return false;
        
        // Второй символ должен быть '-'
        if (reg[1] != '-') return false;
        
        // Следующие 4 символа - цифры
        for (int i = 2; i < 6; i++) {
            if (reg[i] < '0' || reg[i] > '9') return false;
        }
        return true;
    }
    
    return false;
}

int extractNumberFromReg(const char* reg) {
    if (!reg) return 0;
    
    int result = 0;
    for (int i = 0; reg[i] != '\0'; i++) {
        if (reg[i] >= '0' && reg[i] <= '9') {
            result = result * 10 + (reg[i] - '0');
        }
    }
    return result;
}

bool readLine(ifstream& file, char* buffer, int maxLen) {
    if (!file.getline(buffer, maxLen)) return false;
    trim(buffer);
    return true;
}

bool isAllDigits(const char* str) {
    if (!str || str[0] == '\0') return false;
    
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') return false;
    }
    return true;
}

int stringToInt(const char* str) {
    int result = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        result = result * 10 + (str[i] - '0');
    }
    return result;
}

bool isValidDouble(const char* str) {
    if (!str || str[0] == '\0') return false;
    
    int dotCount = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '.') {
            dotCount++;
            if (dotCount > 1) return false;
        } else if (str[i] < '0' || str[i] > '9') {
            return false;
        }
    }
    return true;
}

double stringToDouble(const char* str) {
    double result = 0.0;
    double fraction = 0.0;
    bool afterDot = false;
    double divisor = 10.0;
    
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '.') {
            afterDot = true;
        } else {
            int digit = str[i] - '0';
            if (!afterDot) {
                result = result * 10 + digit;
            } else {
                fraction += digit / divisor;
                divisor *= 10;
            }
        }
    }
    return result + fraction;
}

void initAircraft(Aircraft& a) {
    a.model = new char[MAX_STR_LEN];
    a.regNumber = new char[MAX_STR_LEN];
    a.model[0] = '\0';
    a.regNumber[0] = '\0';
    a.passengers = 0;
    a.cargo = 0.0;
}

void freeAircraft(Aircraft& a) {
    if (a.model) {
        delete[] a.model;
        a.model = nullptr;
    }
    if (a.regNumber) {
        delete[] a.regNumber;
        a.regNumber = nullptr;
    }
}

void copyAircraft(Aircraft& dest, const Aircraft& src) {
    stringCopy(dest.model, src.model);
    stringCopy(dest.regNumber, src.regNumber);
    dest.passengers = src.passengers;
    dest.cargo = src.cargo;
}

bool inputAircraft(Aircraft& a, ifstream& file, int recordNum, ErrorRecord errors[], int& errorCount) {
    // Чтение марки
    if (!readLine(file, a.model, MAX_STR_LEN)) {
        return false;
    }
    
    if (a.model[0] == '\0' && file.eof()) {
        return false;
    }
    
    // Чтение бортового номера
    if (!readLine(file, a.regNumber, MAX_STR_LEN)) {
        stringCopy(errors[errorCount].fieldName, "Бортовой номер");
        stringCopy(errors[errorCount].errorValue, "Отсутствует");
        stringCopy(errors[errorCount].errorMsg, "Неожиданный конец файла");
        errors[errorCount].recordNumber = recordNum;
        errorCount++;
        return false;
    }
    
    if (!isValidRegNumber(a.regNumber)) {
        stringCopy(errors[errorCount].fieldName, "Бортовой номер");
        stringCopy(errors[errorCount].errorValue, a.regNumber);
        stringCopy(errors[errorCount].errorMsg, "Должен быть в формате Б-XXXX (пример: Б-1234)");
        errors[errorCount].recordNumber = recordNum;
        errorCount++;
    }
    
    // Чтение количества пассажиров
    char passStr[MAX_STR_LEN];
    if (!readLine(file, passStr, MAX_STR_LEN)) {
        stringCopy(errors[errorCount].fieldName, "Пассажиры");
        stringCopy(errors[errorCount].errorValue, "Отсутствует");
        stringCopy(errors[errorCount].errorMsg, "Неожиданный конец файла");
        errors[errorCount].recordNumber = recordNum;
        errorCount++;
        return false;
    }
    
    if (!isAllDigits(passStr)) {
        stringCopy(errors[errorCount].fieldName, "Пассажиры");
        stringCopy(errors[errorCount].errorValue, passStr);
        stringCopy(errors[errorCount].errorMsg, "Должно быть целым неотрицательным числом");
        errors[errorCount].recordNumber = recordNum;
        errorCount++;
        a.passengers = 0;
    } else {
        a.passengers = stringToInt(passStr);
        if (a.passengers < 0) {
            stringCopy(errors[errorCount].fieldName, "Пассажиры");
            stringCopy(errors[errorCount].errorValue, passStr);
            stringCopy(errors[errorCount].errorMsg, "Количество пассажиров не может быть отрицательным");
            errors[errorCount].recordNumber = recordNum;
            errorCount++;
            a.passengers = 0;
        }
    }
    
    // Чтение веса груза
    char cargoStr[MAX_STR_LEN];
    if (!readLine(file, cargoStr, MAX_STR_LEN)) {
        stringCopy(errors[errorCount].fieldName, "Вес груза");
        stringCopy(errors[errorCount].errorValue, "Отсутствует");
        stringCopy(errors[errorCount].errorMsg, "Неожиданный конец файла");
        errors[errorCount].recordNumber = recordNum;
        errorCount++;
        return false;
    }
    
    if (!isValidDouble(cargoStr)) {
        stringCopy(errors[errorCount].fieldName, "Вес груза");
        stringCopy(errors[errorCount].errorValue, cargoStr);
        stringCopy(errors[errorCount].errorMsg, "Должно быть неотрицательным числом");
        errors[errorCount].recordNumber = recordNum;
        errorCount++;
        a.cargo = 0.0;
    } else {
        a.cargo = stringToDouble(cargoStr);
        if (a.cargo < 0) {
            stringCopy(errors[errorCount].fieldName, "Вес груза");
            stringCopy(errors[errorCount].errorValue, cargoStr);
            stringCopy(errors[errorCount].errorMsg, "Вес груза не может быть отрицательным");
            errors[errorCount].recordNumber = recordNum;
            errorCount++;
            a.cargo = 0.0;
        }
    }
    
    return true;
}

void indexSort(Aircraft data[], int n, int indices[]) {
    // Инициализация индексов
    for (int i = 0; i < n; i++) {
        indices[i] = i;
    }
    
    // Сортировка выбором
    for (int i = 0; i < n - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < n; j++) {
            int num1 = extractNumberFromReg(data[indices[j]].regNumber);
            int num2 = extractNumberFromReg(data[indices[minIndex]].regNumber);
            if (num1 < num2) {
                minIndex = j;
            }
        }
        if (minIndex != i) {
            int temp = indices[i];
            indices[i] = indices[minIndex];
            indices[minIndex] = temp;
        }
    }
}

void intToStr(int num, char* buffer) {
    if (num == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }
    
    char temp[MAX_STR_LEN];
    int pos = 0;
    while (num > 0) {
        temp[pos++] = '0' + (num % 10);
        num /= 10;
    }
    
    for (int i = 0; i < pos; i++) {
        buffer[i] = temp[pos - 1 - i];
    }
    buffer[pos] = '\0';
}

void doubleToStr(double num, char* buffer) {
    int integerPart = (int)num;
    int fractionalPart = (int)((num - integerPart) * 10 + 0.5);
    if (fractionalPart < 0) fractionalPart = 0;
    
    char intBuf[MAX_STR_LEN];
    intToStr(integerPart, intBuf);
    
    char fracBuf[10];
    if (fractionalPart >= 10) {
        fracBuf[0] = '0' + (fractionalPart / 10);
        fracBuf[1] = '0' + (fractionalPart % 10);
        fracBuf[2] = '\0';
    } else {
        fracBuf[0] = '0' + fractionalPart;
        fracBuf[1] = '\0';
    }
    
    int pos = 0;
    for (int i = 0; intBuf[i] != '\0'; i++) {
        buffer[pos++] = intBuf[i];
    }
    buffer[pos++] = '.';
    for (int i = 0; fracBuf[i] != '\0'; i++) {
        buffer[pos++] = fracBuf[i];
    }
    buffer[pos] = '\0';
}

void printErrors(ErrorRecord errors[], int errorCount) {
    if (errorCount == 0) {
        cout << "Ошибок не обнаружено." << endl << endl;
        return;
    }
    
    cout << "ОБНАРУЖЕННЫЕ ОШИБКИ:" << endl;
    cout << "--------------------------------------------------------------" << endl;
    for (int i = 0; i < errorCount; i++) {
        cout << "  Запись #" << errors[i].recordNumber 
             << ", поле \"" << errors[i].fieldName 
             << "\", значение \"" << errors[i].errorValue 
             << "\": " << errors[i].errorMsg << endl;
    }
    cout << "--------------------------------------------------------------" << endl << endl;
}

void printTable(Aircraft data[], int indices[], int n, ErrorRecord errors[], int errorCount) {
    printErrors(errors, errorCount);
    
    cout << "ТАБЛИЦА САМОЛЕТОВ:" << endl;
    cout << "----------------------------------------------------------------" << endl;
    cout << "Марка ЛА                      Борт. номер     Пасс.   Груз(т)" << endl;
    cout << "----------------------------------------------------------------" << endl;
    
    int totalPass = 0;
    double totalCargo = 0.0;
    
    for (int i = 0; i < n; i++) {
        int idx = indices[i];
        Aircraft& a = data[idx];
        
        // Вывод марки
        cout << a.model;
        int len = stringLength(a.model);
        for (int j = len; j < 30; j++) cout << " ";
        
        // Вывод бортового номера
        if (isValidRegNumber(a.regNumber)) {
            cout << a.regNumber;
            len = stringLength(a.regNumber);
            for (int j = len; j < 15; j++) cout << " ";
        } else {
            cout << "[НЕКОРРЕКТНЫЙ] ";
            cout << "   ";
        }
        
        // Вывод пассажиров
        char passStr[MAX_STR_LEN];
        intToStr(a.passengers, passStr);
        cout << passStr;
        len = stringLength(passStr);
        for (int j = len; j < 8; j++) cout << " ";
        
        // Вывод груза
        char cargoStr[MAX_STR_LEN];
        doubleToStr(a.cargo, cargoStr);
        cout << cargoStr << endl;
        
        totalPass += a.passengers;
        totalCargo += a.cargo;
    }
    
    cout << "----------------------------------------------------------------" << endl;
    cout << "ИТОГО:" << endl;
    cout << "  Пассажиров: " << totalPass << endl;
    cout << "  Груза: " << totalCargo << " тонн" << endl;
    cout << "----------------------------------------------------------------" << endl;
}

void checkDuplicateRegNumbers(Aircraft data[], int n, ErrorRecord errors[], int& errorCount) {
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (stringCompare(data[i].regNumber, data[j].regNumber) == 0 && 
                isValidRegNumber(data[i].regNumber) && 
                isValidRegNumber(data[j].regNumber)) {
                
                if (stringCompare(data[i].model, data[j].model) == 0) {
                    stringCopy(errors[errorCount].fieldName, "Бортовой номер");
                    stringCopy(errors[errorCount].errorValue, data[i].regNumber);
                    stringCopy(errors[errorCount].errorMsg, "Дубликат: два одинаковых самолета с одинаковым БН");
                    errors[errorCount].recordNumber = i + 1;
                    errorCount++;
                } else {
                    stringCopy(errors[errorCount].fieldName, "Бортовой номер");
                    stringCopy(errors[errorCount].errorValue, data[i].regNumber);
                    stringCopy(errors[errorCount].errorMsg, "Предупреждение: одинаковый БН у разных моделей ЛА");
                    errors[errorCount].recordNumber = i + 1;
                    errorCount++;
                }
                break;
            }
        }
    }
}