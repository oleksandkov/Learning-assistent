#include <iostream>
#include <string>

// У вузлі зберігаємо до K чисел; у записі загалом — від 1 до N.
const int K = 3;
const int N = 10;

// Одне число додаткової гілки. nullptr означає відсутність наступного вузла.
struct BranchNode {
    int value;
    BranchNode* next = nullptr;
};

// Основний запис та зв'язки з його сусідами.
struct Node {
    std::string name;
    int numbers[K]{};
    int count = 0; // Кількість чисел у цьому масиві, без гілки.
    BranchNode* branch = nullptr;
    Node* prev = nullptr;
    Node* next = nullptr;
};

struct List {
    Node* head = nullptr; // Перший запис.
    Node* tail = nullptr; // Останній запис.
};

// & передає список за посиланням: зміни стосуються початкового списку.
// const означає, що функція не змінює передане ім'я та числа.
bool add(List& list, const std::string& name, const int numbers[], int count) {
    bool has_name = false;
    for (int i = 0; i < int(name.length()); i++) {
        if (name[i] != ' ' && name[i] != '\t' && name[i] != '\r')
            has_name = true;
    }
    if (!has_name || count < 1 || count > N || numbers == nullptr)
        return false;

    // new виділяє пам'ять, а -> звертається до поля за адресою вузла.
    Node* node = new Node;
    node->name = name;
    node->count = count;
    if (count > K) node->count = K;
    for (int i = 0; i < node->count; i++)
        node->numbers[i] = numbers[i];

    // Числа понад K послідовно додаємо в кінець гілки.
    BranchNode* branch_tail = nullptr;
    for (int i = K; i < count; i++) {
        BranchNode* number = new BranchNode;
        number->value = numbers[i];
        if (node->branch == nullptr) node->branch = number;
        else branch_tail->next = number;
        branch_tail = number;
    }

    // З'єднуємо новий запис зі старим хвостом в обидва боки.
    node->prev = list.tail;
    if (list.tail == nullptr) list.head = node;
    else list.tail->next = node;
    list.tail = node;
    return true;
}

// Шукаємо перший збіг імені, рухаючись від голови до хвоста.
Node* find(const List& list, const std::string& name) {
    Node* node = list.head;
    while (node != nullptr) {
        if (node->name == name) return node;
        node = node->next;
    }
    return nullptr;
}

// Перед викликом основний вузол потрібно від'єднати від списку.
void destroy_node(Node* node) {
    while (node->branch != nullptr) {
        BranchNode* old = node->branch;
        node->branch = old->next; // Зберігаємо наступний вузол до видалення.
        delete old;
    }
    delete node; // Після гілки звільняємо сам запис.
}

bool remove(List& list, const std::string& name) {
    Node* node = find(list, name);
    if (node == nullptr) return false;

    // Сусіди мають посилатися один на одного, пропускаючи видалений запис.
    if (node->prev == nullptr) list.head = node->next;
    else node->prev->next = node->next;
    if (node->next == nullptr) list.tail = node->prev;
    else node->next->prev = node->prev;
    destroy_node(node);
    return true;
}

// Рахуємо основні записи, а не числа чи вузли гілок.
// ponytail: обхід O(m); окремий лічильник потрібний для частого підрахунку.
int size(const List& list) {
    int count = 0;
    Node* node = list.head;
    while (node != nullptr) {
        count++;
        node = node->next;
    }
    return count;
}

void print_node(const Node* node) {
    if (node == nullptr) {
        std::cout << "Елемент не знайдено.\n";
        return;
    }
    std::cout << node->name << " | Основні числа:";
    for (int i = 0; i < node->count; i++)
        std::cout << ' ' << node->numbers[i];
    std::cout << " | Гілка:";
    if (node->branch == nullptr) std::cout << " відсутня";
    BranchNode* number = node->branch;
    while (number != nullptr) {
        std::cout << ' ' << number->value;
        number = number->next;
    }
    std::cout << '\n';
}

void print_list(const List& list) {
    if (list.head == nullptr) std::cout << "Список порожній.\n";
    Node* node = list.head;
    while (node != nullptr) {
        print_node(node);
        node = node->next;
    }
}

// Видаляємо всі записи разом із гілками перед завершенням роботи.
void clear(List& list) {
    while (list.head != nullptr) {
        Node* old = list.head;
        list.head = old->next;
        destroy_node(old);
    }
    list.tail = nullptr;
}

// Читаємо рівно count цілих чисел з одного рядка.
// peek переглядає наступний символ, get забирає його з потоку.
bool read_numbers(int numbers[], int count) {
    for (int i = 0; i < count; i++) {
        while (std::cin.peek() == ' ' || std::cin.peek() == '\t' ||
               std::cin.peek() == '\r')
            std::cin.get();
        if (std::cin.peek() == '\n' || std::cin.eof()) {
            std::cin.get();
            return false; // Чисел у рядку менше, ніж потрібно.
        }
        if (!(std::cin >> numbers[i])) {
            std::cin.clear(); // Скидаємо помилку, щоб наступний ввід працював.
            std::string rest;
            std::getline(std::cin, rest); // Відкидаємо некоректний залишок рядка.
            return false;
        }
    }
    while (std::cin.peek() == ' ' || std::cin.peek() == '\t' ||
           std::cin.peek() == '\r')
        std::cin.get();
    if (std::cin.eof()) return true;
    if (std::cin.peek() == '\n') {
        std::cin.get();
        return true;
    }
    std::string rest;
    std::getline(std::cin, rest);
    return false; // Після чисел є зайві дані.
}

int main() {
    List list;
    int choice = -1;
    std::cout << "Лабораторна робота №3. Варіант 12\n"
              << "Двозв'язний список із розгалуженнями. K = 3, N = 10\n";
    while (choice != 0) {
        std::cout << "\n1. Додати\n2. Знайти\n3. Видалити\n"
                  << "4. Кількість\n5. Друк списку\n6. Приклад\n0. Вийти\nВибір: ";
        if (!read_numbers(&choice, 1)) {
            if (std::cin.eof()) break;
            choice = -1;
            std::cout << "Введіть ціле число від 0 до 6.\n";
            continue; // Показуємо меню знову.
        }
        switch (choice) {
            case 0:
                break; // На наступній перевірці умови цикл завершиться.
            case 1: {
                std::string name;
                int count, numbers[N];
                std::cout << "Ім'я: ";
                if (!std::getline(std::cin, name)) break;
                std::cout << "Кількість чисел (1..10): ";
                if (!read_numbers(&count, 1) || count < 1 || count > N) {
                    std::cout << "Некоректна кількість чисел.\n";
                    break;
                }
                std::cout << "Числа одним рядком: ";
                if (!read_numbers(numbers, count)) {
                    std::cout << "Некоректна послідовність чисел.\n";
                    break;
                }
                if (add(list, name, numbers, count))
                    std::cout << "Елемент додано.\n";
                else std::cout << "Ім'я не може бути порожнім.\n";
                break;
            }
            case 2:
            case 3: {
                std::string name;
                std::cout << "Ім'я: ";
                if (!std::getline(std::cin, name)) break;
                if (choice == 2) print_node(find(list, name));
                else if (remove(list, name)) std::cout << "Елемент видалено.\n";
                else std::cout << "Елемент не знайдено.\n";
                break;
            }
            case 4:
                std::cout << "Кількість елементів: " << size(list) << '\n';
                break;
            case 5:
                print_list(list);
                break;
            case 6: {
                int a[] = {5, 12};
                int b[] = {10, 20, 30, 40, 50, 60};
                add(list, "Запис_1", a, 2);
                add(list, "Запис_2", b, 6);
                std::cout << "Додано 2 приклади.\n";
                break;
            }
            default:
                std::cout << "Введіть ціле число від 0 до 6.\n";
        }
    }
    clear(list);
    std::cout << "Пам'ять звільнено. Завершення роботи.\n";
    return 0;
}
