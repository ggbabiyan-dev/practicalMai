#include <iostream>
#include <fstream>

using namespace std;

const int MAX_RECORDS = 100;      // максимальное количество записей
const int MAX_STR_LEN = 100;      // максимальная длина строки

// Структура для хранения записи о самолёте
struct Aircraft {
    char model[MAX_STR_LEN];       // марка ЛА
    char regNumber[MAX_STR_LEN];   // бортовой номер
    int passengers;                // количество пассажиров
    double cargo;                  // груз
};

// Проверка корректности бортового номера
bool isValidRegNumber(const char* reg) {
    if (reg[0] == '\0') return false;
    for (int i = 0; reg[i] != '\0'; i++) {
        char c = reg[i];
        // Проверка: буква (A-Z, a-z), цифра (0-9) или дефис
        bool isLetter = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
        bool isDigit = (c >= '0' && c <= '9');
        if (!isLetter && !isDigit && c != '-') return false;
    }
    return true;
}

// Извлечение числовой части бортового номера
int extractNumberFromReg(const char* reg) {
    int result = 0;
    for (int i = 0; reg[i] != '\0'; i++) {
        if (reg[i] >= '0' && reg[i] <= '9') {
            result = result * 10 + (reg[i] - '0');
        }
    }
    return result;
}

// Ввод строки из файла
bool readLine(ifstream& file, char* buffer, int maxLen) {
    if (!file.getline(buffer, maxLen)) return false;
    int len = 0;
    while (buffer[len] != '\0' && buffer[len] != '\r') len++;
    buffer[len] = '\0';
    return true;
}

// Проверка, что строка состоит только из цифр
bool isAllDigits(const char* str) {
    if (str[0] == '\0') return false;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') return false;
    }
    return true;
}

// Преобразование строки в int
int stringToInt(const char* str) {
    int result = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        result = result * 10 + (str[i] - '0');
    }
    return result;
}

// Проверка, что строка - корректное число с плавающей точкой
bool isValidDouble(const char* str) {
    if (str[0] == '\0') return false;
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

// Преобразование строки в double
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

// Ввод данных с контролем
bool inputAircraft(Aircraft& a, ifstream& file) {
    // Марка ЛА
    if (!readLine(file, a.model, MAX_STR_LEN)) return false;
    
    // Бортовой номер
    if (!readLine(file, a.regNumber, MAX_STR_LEN)) return false;
    if (!isValidRegNumber(a.regNumber)) {
        cerr << "Ошибка: некорректный бортовой номер '" << a.regNumber << "'\n";
        return false;
    }
    
    // Количество пассажиров
    char passStr[MAX_STR_LEN];
    if (!readLine(file, passStr, MAX_STR_LEN)) return false;
    if (!isAllDigits(passStr)) {
        cerr << "Ошибка: некорректное количество пассажиров '" << passStr << "'\n";
        return false;
    }
    a.passengers = stringToInt(passStr);
    
    // Вес груза
    char cargoStr[MAX_STR_LEN];
    if (!readLine(file, cargoStr, MAX_STR_LEN)) return false;
    if (!isValidDouble(cargoStr)) {
        cerr << "Ошибка: некорректный вес груза '" << cargoStr << "'\n";
        return false;
    }
    a.cargo = stringToDouble(cargoStr);
    
    return true;
}

// Индексная сортировка линейным методом (вставками)
void indexSort(const Aircraft data[], int n, int indices[]) {
    for (int i = 0; i < n; i++) indices[i] = i;
    
    for (int i = 1; i < n; i++) {
        int keyIndex = indices[i];
        int keyValue = extractNumberFromReg(data[keyIndex].regNumber);
        int j = i - 1;
        while (j >= 0 && extractNumberFromReg(data[indices[j]].regNumber) > keyValue) {
            indices[j + 1] = indices[j];
            j--;
        }
        indices[j + 1] = keyIndex;
    }
}

// Ручное преобразование int в строку для вывода
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

// Ручное преобразование double в строку (1 знак после запятой)
void doubleToStr(double num, char* buffer) {
    int integerPart = (int)num;
    int fractionalPart = (int)((num - integerPart) * 10 + 0.5);
    
    char intBuf[MAX_STR_LEN];
    intToStr(integerPart, intBuf);
    
    char fracBuf[10];
    if (fractionalPart < 0) fractionalPart = 0;
    fracBuf[0] = '0' + fractionalPart;
    fracBuf[1] = '\0';
    
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

// Печать таблицы
void printTable(const Aircraft data[], int indices[], int n) {
    // Заголовок
    cout << "Марка ЛА        Борт. номер     Пасс.   Груз" << endl;
    cout << "----------------------------------------------" << endl;
    
    int totalPass = 0;
    double totalCargo = 0.0;
    
    for (int i = 0; i < n; i++) {
        int idx = indices[i];
        const Aircraft& a = data[idx];
        
        // Вывод марки с выравниванием до 15 символов
        cout << a.model;
        int len = 0;
        while (a.model[len] != '\0') len++;
        for (int j = len; j < 15; j++) cout << " ";
        
        // Вывод бортового номера
        cout << a.regNumber;
        len = 0;
        while (a.regNumber[len] != '\0') len++;
        for (int j = len; j < 15; j++) cout << " ";
        
        // Вывод пассажиров
        char passStr[MAX_STR_LEN];
        intToStr(a.passengers, passStr);
        cout << passStr;
        len = 0;
        while (passStr[len] != '\0') len++;
        for (int j = len; j < 8; j++) cout << " ";
        
        // Вывод груза
        char cargoStr[MAX_STR_LEN];
        doubleToStr(a.cargo, cargoStr);
        cout << cargoStr << endl;
        
        totalPass += a.passengers;
        totalCargo += a.cargo;
    }
    
    cout << "----------------------------------------------" << endl;
    cout << "ИТОГО:" << endl;
    cout << "Пассажиров: " << totalPass << endl;
    cout << "Груза: " << totalCargo << endl;
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    ifstream file("data.txt");
    if (!file.is_open()) {
        cerr << "Ошибка: не удалось открыть файл data.txt" << endl;
        return 1;
    }
    
    Aircraft aircrafts[MAX_RECORDS];
    int recordCount = 0;
    
    while (recordCount < MAX_RECORDS && inputAircraft(aircrafts[recordCount], file)) {
        recordCount++;
    }
    
    file.close();
    
    if (recordCount == 0) {
        cout << "Нет данных для обработки." << endl;
        return 0;
    }
    
    int indices[MAX_RECORDS];
    indexSort(aircrafts, recordCount, indices);
    printTable(aircrafts, indices, recordCount);
    
    return 0;
}
