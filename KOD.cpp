//=============================================================================
// Работа по ознакомительной практики
// Вариант: 3
// Студент: Бабиян Г. А.
// Группа: М30-111БВ-25
// Описание: Программа для учета самолетов. Пользователь вводит имя файла
//           для обработки. Выполняет сортировку по числовой части
//           бортового номера с выводом итоговой таблицы
//=============================================================================

#include <iostream>
#include <fstream>
#include <clocale>

using namespace std;

// Константы
const int MAX_RECORDS = 100;        // максимальное количество записей
const int MAX_STR_LEN = 100;        // максимальная длина строки
const int MAX_FILENAME_LEN = 200;   // максимальная длина имени файла

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

// Ввод имени файла от пользователя
void inputFilename(char* filename, int maxLen);

//=============================================================================
// ГЛАВНАЯ ФУНКЦИЯ
//=============================================================================
int main() {
    // Установка локали для поддержки русских букв
    setlocale(LC_ALL, "ru_RU.UTF-8");
    
    printHeader();
    
    char filename[MAX_FILENAME_LEN];
    char choice[MAX_STR_LEN];
    int totalFilesProcessed = 0;
    int totalRecordsProcessed = 0;
    
    do {
        // Ввод имени файла
        inputFilename(filename, MAX_FILENAME_LEN);
        
        cout << "\n================================================================================" << endl;
        cout << "ОБРАБОТКА ФАЙЛА: " << filename << endl;
        cout << "================================================================================" << endl;
        
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "ОШИБКА: не удалось открыть файл " << filename << endl;
            cerr << "Проверьте, существует ли файл и правильно ли указано имя." << endl;
        } else {
            // Динамическое выделение памяти
            Aircraft* aircrafts = new Aircraft[MAX_RECORDS];
            for (int i = 0; i < MAX_RECORDS; i++) {
                initAircraft(aircrafts[i]);
            }
            
            ErrorRecord* errors = new ErrorRecord[MAX_RECORDS];
            int errorCount = 0;
            int recordCount = 0;
            
            // Чтение записей
            cout << "Чтение данных из файла..." << endl;
            
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
                cout << "Нет данных для обработки в файле." << endl;
            } else {
                cout << "Успешно прочитано записей: " << recordCount << endl;
                cout << "Обнаружено ошибок: " << errorCount << endl << endl;
                
                // Проверка дубликатов
                checkDuplicateRegNumbers(aircrafts, recordCount, errors, errorCount);
                
                // Сортировка
                int* indices = new int[MAX_RECORDS];
                indexSort(aircrafts, recordCount, indices);
                
                // Вывод таблицы
                printTable(aircrafts, indices, recordCount, errors, errorCount);
                
                // Освобождение памяти
                delete[] indices;
                
                totalRecordsProcessed += recordCount;
            }
            
            // Освобождение памяти
            for (int i = 0; i < recordCount; i++) {
                freeAircraft(aircrafts[i]);
            }
            delete[] aircrafts;
            delete[] errors;
            
            totalFilesProcessed++;
        }
        
        // Спрашиваем пользователя, хочет ли он обработать еще один файл
        cout << "\nХотите обработать другой файл? (y/n): ";
        cin.getline(choice, MAX_STR_LEN);
        
        // Если пользователь ввел несколько символов, берем первый
        char firstChar = choice[0];
        if (firstChar != 'y' && firstChar != 'Y' && firstChar != 'д' && firstChar != 'Д') {
            break;
        }
        
    } while (true);
    
    // Итоговый отчет
    cout << "\n\n================================================================================" << endl;
    cout << "ИТОГОВЫЙ ОТЧЕТ" << endl;
    cout << "================================================================================" << endl;
    cout << "Обработано файлов: " << totalFilesProcessed << endl;
    cout << "Всего записей: " << totalRecordsProcessed << endl;
    cout << "================================================================================" << endl;
    cout << "Программа завершена. Спасибо за использование!" << endl;
    
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

void inputFilename(char* filename, int maxLen) {
    cout << "\nВведите имя файла для обработки: ";
    cin.getline(filename, maxLen);
    
    // Удаляем возможные кавычки в начале и конце
    int len = stringLength(filename);
    if (len > 0 && (filename[0] == '"' || filename[0] == '\'')) {
        // Сдвигаем строку влево, удаляя первый символ
        for (int i = 0; i < len - 1; i++) {
            filename[i] = filename[i + 1];
        }
        len--;
    }
    if (len > 0 && (filename[len - 1] == '"' || filename[len - 1] == '\'')) {
        filename[len - 1] = '\0';
    }
    
    // Если имя файла не содержит расширение, добавляем .txt
    bool hasExtension = false;
    for (int i = 0; filename[i] != '\0'; i++) {
        if (filename[i] == '.') {
            hasExtension = true;
            break;
        }
    }
    
    if (!hasExtension && filename[0] != '\0') {
        int currentLen = stringLength(filename);
        if (currentLen + 5 < maxLen) {
            filename[currentLen] = '.';
            filename[currentLen + 1] = 't';
            filename[currentLen + 2] = 'x';
            filename[currentLen + 3] = 't';
            filename[currentLen + 4] = '\0';
        }
    }
}

void printHeader() {
    cout << "================================================================================" << endl;
    cout << "Работа по ознакомительной практики" << endl;
    cout << "Вариант: 3" << endl;
    cout << "Описание: Учет самолетов с сортировкой по числовой части бортового номера" << endl;
    cout << "Формат бортового номера: Б-XXXX" << endl;
    cout << "================================================================================" << endl;
    cout << "\nПрограмма позволяет обработать любой текстовый файл с данными о самолетах." << endl;
    cout << "Формат файла:" << endl;
    cout << "  - Каждая запись состоит из 4 строк:" << endl;
    cout << "    1. Марка самолета" << endl;
    cout << "    2. Бортовой номер (формат Б-XXXX)" << endl;
    cout << "    3. Количество пассажиров" << endl;
    cout << "    4. Вес груза (тонны)" << endl;
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
    
    int len = stringLength(reg);
    
    // Проверяем в однобайтовой кодировке (Windows-1251, CP866)
    unsigned char firstChar = (unsigned char)reg[0];
    bool isRussianB = (firstChar == 0xC1 || firstChar == 0xE1); // Б или б в Windows-1251
    
    // Проверяем в UTF-8
    bool isRussianB_UTF8 = false;
    if (len >= 2) {
        unsigned char b1 = (unsigned char)reg[0];
        unsigned char b2 = (unsigned char)reg[1];
        if (b1 == 0xD0 && (b2 == 0x91 || b2 == 0xB1)) {
            isRussianB_UTF8 = true;
        }
    }
    
    // Если это UTF-8
    if (isRussianB_UTF8) {
        if (len != 7) return false;
        if (reg[2] != '-') return false;
        for (int i = 3; i < 7; i++) {
            if (reg[i] < '0' || reg[i] > '9') return false;
        }
        return true;
    }
    // Если это однобайтовая кодировка
    else if (isRussianB) {
        if (len != 6) return false;
        if (reg[1] != '-') return false;
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