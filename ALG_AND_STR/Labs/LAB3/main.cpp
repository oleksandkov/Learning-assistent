#include <iostream>
#include <string>

const int K = 3;
const int N = 10;

struct BranchNode {
    int value;
    BranchNode* next = nullptr;
};

struct Node {
    std::string name;
    int numbers[K]{};
    int count = 0; 
    BranchNode* branch = nullptr;
    Node* prev = nullptr;
    Node* next = nullptr;
};

struct List {
    Node* head = nullptr; 
    Node* tail = nullptr; 
};


bool add(List& list, const std::string& name, const int numbers[], int count) {
    bool has_name = false;
    for (int i = 0; i < int(name.length()); i++) {
        if (name[i] != ' ' && name[i] != '\t' && name[i] != '\r')
            has_name = true;
    }
    if (!has_name || count < 1 || count > N || numbers == nullptr)
        return false;

    Node* node = new Node;
    node->name = name;
    node->count = count;
    if (count > K) node->count = K;
    for (int i = 0; i < node->count; i++)
        node->numbers[i] = numbers[i];

    BranchNode* branch_tail = nullptr;
    for (int i = K; i < count; i++) {
        BranchNode* number = new BranchNode;
        number->value = numbers[i];
        if (node->branch == nullptr) node->branch = number;
        else branch_tail->next = number;
        branch_tail = number;
    }

    node->prev = list.tail;
    if (list.tail == nullptr) list.head = node;
    else list.tail->next = node;
    list.tail = node;
    return true;
}

Node* find(const List& list, const std::string& name) {
    Node* node = list.head;
    while (node != nullptr) {
        if (node->name == name) return node;
        node = node->next;
    }
    return nullptr;
}

void destroy_node(Node* node) {
    while (node->branch != nullptr) {
        BranchNode* old = node->branch;
        node->branch = old->next; 
        delete old;
    }
    delete node; 
}

bool remove(List& list, const std::string& name) {
    Node* node = find(list, name);
    if (node == nullptr) return false;

    if (node->prev == nullptr) list.head = node->next;
    else node->prev->next = node->next;
    if (node->next == nullptr) list.tail = node->prev;
    else node->next->prev = node->prev;
    destroy_node(node);
    return true;
}


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


void clear(List& list) {
    while (list.head != nullptr) {
        Node* old = list.head;
        list.head = old->next;
        destroy_node(old);
    }
    list.tail = nullptr;
}


bool read_numbers(int numbers[], int count) {
    for (int i = 0; i < count; i++) {
        while (std::cin.peek() == ' ' || std::cin.peek() == '\t' ||
               std::cin.peek() == '\r')
            std::cin.get();
        if (std::cin.peek() == '\n' || std::cin.eof()) {
            std::cin.get();
            return false; 
        }
        if (!(std::cin >> numbers[i])) {
            std::cin.clear(); 
            std::string rest;
            std::getline(std::cin, rest);
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
    return false; 
}

int main() {
    List list;
    int choice = -1;
    std::cout  << "Двозв'язний список із розгалуженнями. K = 3, N = 10\n";
    while (choice != 0) {
        std::cout << "\n1. Додати\n2. Знайти\n3. Видалити\n"
                  << "4. Кількість\n5. Друк списку\n6. Приклад\n0. Вийти\nВибір: ";
        if (!read_numbers(&choice, 1)) {
            if (std::cin.eof()) break;
            choice = -1;
            std::cout << "Введіть ціле число від 0 до 6.\n";
            continue; 
        }
        switch (choice) {
            case 0:
                break; 
            case 1: {
                std::string name;
                int count, numbers[N];
                std::cout << "Ім'я: ";
                if (!std::getline(std::cin, name)) break;
                std::cout << "Кількість чисел: ";
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
    std::cout << "Кінець\n";
    return 0;
}
