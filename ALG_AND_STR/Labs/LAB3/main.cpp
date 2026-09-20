#include <iostream>
#include <string>
#include <vector>
#include <cassert>

#ifdef _WIN32
#include <windows.h>
#endif

constexpr int K = 3;
constexpr int N = 10;

struct BranchNode {
    int value;
    BranchNode* next = nullptr;
};

struct Node {
    std::string name;
    int main_count = 0;
    int main_nums[K]{};
    BranchNode* branch = nullptr;
    Node* prev = nullptr;
    Node* next = nullptr;
};

struct DoublyBranchedList {
    Node* head = nullptr;
    Node* tail = nullptr;
    int count = 0;

    ~DoublyBranchedList() {
        clear();
    }

    void clear() {
        Node* curr = head;
        while (curr) {
            Node* nxt = curr->next;
            free_branch(curr->branch);
            delete curr;
            curr = nxt;
        }
        head = tail = nullptr;
        count = 0;
    }

    static void free_branch(BranchNode* b) {
        while (b) {
            BranchNode* nxt = b->next;
            delete b;
            b = nxt;
        }
    }

    void push_back(const std::string& name, const std::vector<int>& nums) {
        Node* node = new Node();
        node->name = name;
        node->prev = tail;
        node->next = nullptr;

        int total = static_cast<int>(nums.size());
        if (total > N) total = N;

        node->main_count = (total < K) ? total : K;
        for (int i = 0; i < node->main_count; ++i) {
            node->main_nums[i] = nums[i];
        }

        BranchNode** b_tail = &node->branch;
        for (int i = K; i < total; ++i) {
            *b_tail = new BranchNode{nums[i], nullptr};
            b_tail = &((*b_tail)->next);
        }

        if (!head) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
        ++count;
    }

    Node* find(const std::string& name) const {
        for (Node* curr = head; curr; curr = curr->next) {
            if (curr->name == name) return curr;
        }
        return nullptr;
    }

    bool remove(const std::string& name) {
        Node* target = find(name);
        if (!target) return false;

        if (target->prev) target->prev->next = target->next;
        else head = target->next;

        if (target->next) target->next->prev = target->prev;
        else tail = target->prev;

        free_branch(target->branch);
        delete target;
        --count;
        return true;
    }

    int size() const {
        return count;
    }

    static void print_node(const Node* node) {
        if (!node) {
            std::cout << "Елемент не знайдено.\n";
            return;
        }
        std::cout << "Елемент: " << node->name << "\n";
        std::cout << "  Основний вузол (<= " << K << " чисел): [";
        for (int i = 0; i < node->main_count; ++i) {
            std::cout << node->main_nums[i] << (i + 1 < node->main_count ? ", " : "");
        }
        std::cout << "]\n";

        std::cout << "  Гілка розгалуження (> " << K << " чисел): ";
        if (!node->branch) {
            std::cout << "відсутня\n";
        } else {
            std::cout << "[";
            for (BranchNode* b = node->branch; b; b = b->next) {
                std::cout << b->value << (b->next ? " -> " : "");
            }
            std::cout << "]\n";
        }
    }

    void print_all() const {
        if (!head) {
            std::cout << "Список порожній.\n";
            return;
        }
        std::cout << "\n=== Вміст списку (всього елементів: " << count << ") ===\n";
        int idx = 1;
        for (Node* curr = head; curr; curr = curr->next, ++idx) {
            std::cout << idx << ") ";
            print_node(curr);
        }
        std::cout << "==========================================\n\n";
    }
};

void run_tests() {
    DoublyBranchedList list;
    assert(list.size() == 0);

    list.push_back("Alpha", {10, 20});
    assert(list.size() == 1);
    Node* a = list.find("Alpha");
    assert(a != nullptr && a->main_count == 2 && a->branch == nullptr);

    list.push_back("Beta", {1, 2, 3, 4, 5});
    assert(list.size() == 2);
    Node* b = list.find("Beta");
    assert(b != nullptr && b->main_count == K && b->branch != nullptr);
    assert(b->branch->value == 4 && b->branch->next->value == 5);

    assert(list.size() == 2);

    assert(list.remove("Alpha"));
    assert(list.size() == 1);
    assert(list.find("Alpha") == nullptr);
    assert(list.remove("Beta"));
    assert(list.size() == 0);
    assert(list.head == nullptr && list.tail == nullptr);

    std::cout << "Автотести успішно пройдено.\n";
}

void load_samples(DoublyBranchedList& list) {
    list.push_back("Запис_1", {5, 12});
    list.push_back("Запис_2", {10, 20, 30, 40, 50, 60});
    list.push_back("Запис_3", {7, 8, 9});
    list.push_back("Запис_4", {100, 200, 300, 400});
    std::cout << "Завантажено 4 зразкові записи (із гілками та без).\n";
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    if (argc > 1 && std::string(argv[1]) == "--test") {
        run_tests();
        return 0;
    }

    DoublyBranchedList list;
    std::cout << "=== Лабораторна робота №3. Варіант 12 ===\n";
    std::cout << "Двозв'язний список із розгалуженнями (K = " << K << ", N = " << N << ")\n";

    while (true) {
        std::cout << "\nМеню операцій:\n"
                  << "1. Додати елемент\n"
                  << "2. Знайти та вивести елемент за ім'ям\n"
                  << "3. Видалити елемент за ім'ям\n"
                  << "4. Підрахувати кількість елементів\n"
                  << "5. Роздрукувати весь список\n"
                  << "6. Завантажити демонстраційні дані\n"
                  << "0. Вийти\n"
                  << "Ваш вибір: ";

        int choice = -1;
        if (!(std::cin >> choice)) {
            break;
        }

        if (choice == 0) {
            std::cout << "Завершення роботи.\n";
            break;
        }

        switch (choice) {
            case 1: {
                std::string name;
                int count = 0;
                std::cout << "Введіть ім'я елемента: ";
                std::cin >> name;
                std::cout << "Введіть кількість чисел (1 до " << N << "): ";
                std::cin >> count;
                if (count < 1) count = 1;
                if (count > N) count = N;

                std::vector<int> nums(count);
                std::cout << "Введіть " << count << " цілих чисел: ";
                for (int i = 0; i < count; ++i) {
                    std::cin >> nums[i];
                }
                list.push_back(name, nums);
                std::cout << "Елемент успішно додано.\n";
                break;
            }
            case 2: {
                std::string name;
                std::cout << "Введіть ім'я для пошуку: ";
                std::cin >> name;
                Node* found = list.find(name);
                DoublyBranchedList::print_node(found);
                break;
            }
            case 3: {
                std::string name;
                std::cout << "Введіть ім'я для видалення: ";
                std::cin >> name;
                if (list.remove(name)) {
                    std::cout << "Елемент \"" << name << "\" видалено.\n";
                } else {
                    std::cout << "Елемент не знайдено.\n";
                }
                break;
            }
            case 4: {
                std::cout << "Кількість елементів у списку: " << list.size() << "\n";
                break;
            }
            case 5: {
                list.print_all();
                break;
            }
            case 6: {
                load_samples(list);
                break;
            }
            default:
                std::cout << "Невірний вибір. Спробуйте ще раз.\n";
                break;
        }
    }

    return 0;
}
