#include <iostream>
#include <string>

using namespace std;

int main()
{
    int choice = 0;
    while (true)
    {
        cout << "Обери дію:" << endl;
        cout << "1 - DISKCOMP (буде використано команду FC)" << endl;
        cout << "2 - DIR (буде виведення вмісту папки)" << endl;
        cout << "3 - ERA (буде використано команду DEL)" << endl;
        cout << "4 - вести самостійно" << endl;
        cout << "5 - Вихід" << endl;
        cin >> choice;
        cin.ignore();

        if (choice == 5)
        {
            break;
        }
        if (choice == 1)
        {
            string file1, file2;
            cout << "Введіть шлях до першого файлу: ";
            getline(cin, file1);
            cout << "Введіть шлях до другого файлу: ";
            getline(cin, file2);
            string command = "FC \"" + file1 + "\" \"" + file2 + "\"";
            system(command.c_str());
            cin.get();
        }
        else if (choice == 2)
        {

            system("DIR");
            cin.get();
        }
        else if (choice == 3)
        {
            cout << "Введіть шлях до файлу для видалення: ";
            string fileToDelete;
            getline(cin, fileToDelete);
            string command = "DEL /p \"" + fileToDelete + "\"";
            system(command.c_str());
            cin.get();
        }
        else if (choice == 4)
        {
            cout << "Введіть команду для виконання: ";
            string userCommand;
            getline(cin, userCommand);
            if (userCommand.empty())
            {
                cout << "Команда не може бути порожньою." << endl;
                continue;
            }
            else if (userCommand == "exit" || userCommand == "quit")
            {
                break;
            }
            else if (userCommand == "DISKCOMP")

                system(userCommand.c_str());
            cin.get();
        }
        else
        {
            cout << "Невірний вибір. Спробуйте ще раз." << endl;
        }
    }

    return 0;
}