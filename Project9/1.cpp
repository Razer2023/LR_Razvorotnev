#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

//класс для ошибок программы
class ProgramError : public std::exception {
protected:
    std::string message;
public:
    ProgramError(const std::string& msg) : message(msg) {}
    virtual const char* what() const noexcept override {
        return message.c_str();
    }
    virtual ~ProgramError() = default;
};

class InsufficientPrivilegesError : public ProgramError {
public:
    InsufficientPrivilegesError()
        : ProgramError("Недостаточно привилегий для выполнения операции") {
    }
};

class ConversionError : public ProgramError {
public:
    ConversionError()
        : ProgramError("Ошибка преобразования типов данных") {
    }
};

class ValueConversionError : public ProgramError {
public:
    ValueConversionError()
        : ProgramError("Невозможно преобразовать значение") {
    }
};

class InterfaceCastError : public ProgramError {
public:
    InterfaceCastError()
        : ProgramError("Невозможно привести к интерфейсу") {
    }
};

//класс массива указателей
template<typename T>
class PointerArray {
private:
    std::vector<T*> pointers;

public:
    PointerArray() = default;

    PointerArray(size_t size) {
        pointers.reserve(size);
    }

    ~PointerArray() {
        clear();
    }

    PointerArray(const PointerArray&) = delete;
    PointerArray& operator=(const PointerArray&) = delete;

    PointerArray(PointerArray&& other) noexcept : pointers(std::move(other.pointers)) {}
    PointerArray& operator=(PointerArray&& other) noexcept {
        if (this != &other) {
            clear();
            pointers = std::move(other.pointers);
        }
        return *this;
    }

    void add(T* ptr) {
        if (!ptr) {
            throw std::invalid_argument("Передан нулевой указатель");
        }
        pointers.push_back(ptr);
    }

    //добавление объекта
    template<typename... Args>
    void emplace(Args&&... args) {
        pointers.push_back(new T(std::forward<Args>(args)...));
    }

    //освобождение памяти
    void clear() {
        for (T* ptr : pointers) {
            delete ptr;
        }
        pointers.clear();
    }

    size_t size() const {
        return pointers.size();
    }

    bool empty() const {
        return pointers.empty();
    }

    //перегрузка оператора []
    T& operator[](size_t index) {
        if (index >= pointers.size()) {
            throw std::out_of_range("Индекс " + std::to_string(index) +
                " выходит за границы массива размера " +
                std::to_string(pointers.size()));
        }
        return *pointers[index];
    }

    const T& operator[](size_t index) const {
        if (index >= pointers.size()) {
            throw std::out_of_range("Индекс " + std::to_string(index) +
                " выходит за границы массива размера " +
                std::to_string(pointers.size()));
        }
        return *pointers[index];
    }

    T* get(size_t index) const {
        if (index >= pointers.size()) {
            return nullptr;
        }
        return pointers[index];
    }
};
//тестовый класс
class TestClass {
private:
    std::string name;
    int value;

public:
    TestClass(const std::string& n, int v) : name(n), value(v) {
        std::cout << "Создан TestClass: " << name << " = " << value << std::endl;
    }

    ~TestClass() {
        std::cout << "Уничтожен TestClass: " << name << std::endl;
    }

    void print() const {
        std::cout << "TestClass: " << name << " = " << value << std::endl;
    }

    void setValue(int newValue) {
        if (newValue < 0) {
            throw std::invalid_argument("Значение не может быть отрицательным");
        }
        value = newValue;
    }

    std::string getName() const { return name; }
    int getValue() const { return value; }
};

void processValue(int value) {
    if (value < 0 || value > 100) {
        throw std::invalid_argument("Значение " + std::to_string(value) +
            " должно быть в диапазоне [0, 100]");
    }
    std::cout << "Обработано значение: " << value << std::endl;
}

void demonstrateCustomExceptions() {
    std::cout << "Демонстрация пользовательских исключений: " << std::endl;

    try {
        throw InsufficientPrivilegesError();
    }
    catch (const ProgramError& e) {
        std::cout << "Поймано исключение: " << e.what() << std::endl;
    }

    try {
        throw ConversionError();
    }
    catch (const ProgramError& e) {
        std::cout << "Поймано исключение: " << e.what() << std::endl;
    }
}

int main() {
    std::locale::global(std::locale(""));

    std::cout << "Демонстрация работы программы: " << std::endl;

    try {
        demonstrateCustomExceptions();

        std::cout << "Демонстрация PointerArray с TestClass: " << std::endl;
        PointerArray<TestClass> testArray;

        testArray.emplace("Объект 1", 10);
        testArray.emplace("Объект 2", 20);
        testArray.emplace("Объект 3", 30);

        std::cout << "Корректный доступ к элементам: " << std::endl;
        for (size_t i = 0; i < testArray.size(); ++i) {
            testArray[i].print();
        }

        std::cout << "Демонстрация обработки invalid_argument: " << std::endl;
        try {
            processValue(50);
            processValue(150);
        }
        catch (const std::invalid_argument& e) {
            std::cout << "Поймано invalid_argument: " << e.what() << std::endl;
        }

        std::cout << "Демонстрация исключений в методах объектов: " << std::endl;
        try {
            testArray[0].setValue(-5);
        }
        catch (const std::invalid_argument& e) {
            std::cout << "Поймано invalid_argument: " << e.what() << std::endl;
        }

        std::cout << "Демонстрация исключения при выходе за границы: " << std::endl;
        try {
            testArray[10].print();
        }
        catch (const std::out_of_range& e) {
            std::cout << "Поймано out_of_range: " << e.what() << std::endl;
        }

        std::cout << "Демонстрация PointerArray с int: " << std::endl;
        PointerArray<int> intArray;

        intArray.emplace(100);
        intArray.emplace(200);
        intArray.emplace(300);

        for (size_t i = 0; i < intArray.size(); ++i) {
            std::cout << "intArray[" << i << "] = " << intArray[i] << std::endl;
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Непредвиденная ошибка: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Программа завершена успешно: " << std::endl;
    std::cout << "Вся динамически выделенная память освобождена" << std::endl;

    return 0;
}
