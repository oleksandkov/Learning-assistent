#include <iostream>
#include <string>
#include <algorithm>

using namespace std;


// C:\Users\muaro\Documents\GitHub\Learning-assistent\PEOPLE-MACHINE\LABA1-2\ 

int main()
{
    int choice = 0;
    while (true)
    {
        cout << "Обери дію:" << endl;
        cout << "1 - DISKCOMP (буде використано команду COMP)" << endl;
        cout << "2 - DIR (буде виведення вмісту папки)" << endl;
        cout << "3 - ERA (буде використано команду DEL)" << endl;
        cout << "4 - TASKKILL (буде використано команду TASKKILL)" << endl;
        cout << "5 - вести самостійно" << endl;
        cout << "6 - Вихід" << endl;
        cin >> choice;
        cin.ignore();

        if (choice == 6)
        {
            break;
        }
        if (choice == 1)
        {
            string file1, file2;
            cout << "Введіть шлях до першого файлу: ";
            // C:\Users\muaro\Documents\GitHub\Learning-assistent\PEOPLE-MACHINE\LABA1-2\Example1.txt
            getline(cin, file1);
            cout << "Введіть шлях до другого файлу: ";
            // C:\Users\muaro\Documents\GitHub\Learning-assistent\PEOPLE-MACHINE\LABA1-2\Example2.txt
            getline(cin, file2);

            cout << "Оберіть опції порівняння (COMP):" << endl;
            cout << "1 - Без додаткових опцій" << endl;
            cout << "2 - /A  (виводити відмінності як ASCII символи)" << endl;
            cout << "3 - /L  (показувати номери рядків)" << endl;
            cout << "4 - /C  (без врахування регістру)" << endl;
            cout << "5 - /A /L /C (усі опції)" << endl;
            int compOpt = 1;
            cin >> compOpt;
            cin.ignore();

            string flags = "";
            if (compOpt == 2) flags = " /A";
            else if (compOpt == 3) flags = " /L";
            else if (compOpt == 4) flags = " /C";
            else if (compOpt == 5) flags = " /A /L /C";

            string command = "echo N | COMP "  + flags + " \"" + file1 + "\" \"" + file2 + "\"";
            system(command.c_str());
            system("pause");
        }
        else if (choice == 2)
        {
            cout << "Вкажіть вміст до якої папки ви хочете переглянути (наприклад, C:\\): ";
            string directory;
            getline(cin, directory);
            if (directory.empty())
            {
                system("DIR");
            } else {
                system(("DIR \"" + directory + "\"").c_str());
            }
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
            int subChoice = 0;
            cout << "Оберіть опцію TASKKILL:" << endl;
            cout << "1 - Завершити процес за іменем" << endl;
            cout << "2 - Завершити процес за PID" << endl;
            cout << "3 - Переглянути список процесів (TASKLIST)" << endl;
            cout << "0 - Повернутись до головного меню" << endl;
            cin >> subChoice;
            cin.ignore();

            if (subChoice == 0)
            {
                continue;
            }
            else if (subChoice == 1)
            {
                cout << "Введіть ім'я процесу ";
                string procName;
                getline(cin, procName);
                if (!procName.empty())
                {
                    string command = "TASKKILL /IM \"" + procName + "\" /F";
                    system(command.c_str());
                }
                else
                {
                    cout << "Ім'я процесу не може бути порожнім." << endl;
                }
                cin.get();
            }
            else if (subChoice == 2)
            {
                cout << "Введіть PID процесу: ";
                string pid;
                getline(cin, pid);
                if (!pid.empty())
                {
                    string command = "TASKKILL /PID " + pid + " /F";
                    system(command.c_str());
                }
                else
                {
                    cout << "PID не може бути порожнім." << endl;
                }
                cin.get();
            }
            else if (subChoice == 3)
            {
                system("TASKLIST");
                cin.get();
            }
            else
            {
                cout << "Невірний вибір опції TASKKILL." << endl;
            }
        }
        else if (choice == 5)
        {
            cout << "Введіть команду для виконання: ";
            string userCommand;
            getline(cin, userCommand);
            cin.get();
            if (userCommand.empty())
            {
                cout << "Команда не може бути порожньою." << endl;
                continue;
            }
            string userCommandUpper = userCommand;
            transform(userCommandUpper.begin(), userCommandUpper.end(), userCommandUpper.begin(), ::toupper);
            if (userCommandUpper == "EXIT" || userCommandUpper == "QUIT")
            {
                break;
            }
            else if (userCommandUpper == "DISKCOMP")
            {
                string file1, file2;
                cout << "Введіть шлях до першого файлу: ";
                getline(cin, file1);
                cout << "Введіть шлях до другого файлу: ";
                getline(cin, file2);

                cout << "Оберіть опції порівняння (COMP):" << endl;
                cout << "1 - Без додаткових опцій" << endl;
                cout << "2 - /A  (виводити відмінності як ASCII символи)" << endl;
                cout << "3 - /L  (показувати номери рядків)" << endl;
                cout << "4 - /C  (без врахування регістру)" << endl;
                cout << "5 - /A /L /C (усі опції)" << endl;
                int compOpt = 1;
                cin >> compOpt;
                cin.ignore();

                string flags = "";
                if (compOpt == 2) flags = " /A";
                else if (compOpt == 3) flags = " /L";
                else if (compOpt == 4) flags = " /C";
                else if (compOpt == 5) flags = " /A /L /C";

                string command = "COMP " + flags + " \"" + file1 + "\" \"" + file2 + "\"";
                system(command.c_str());
            }
            else if (userCommandUpper == "DIR")
            {
                system("DIR");
                cin.get();
            }
            else if (userCommandUpper == "ERA")
            {
                cout << "Введіть шлях до файлу для видалення: ";
                string fileToDelete;
                getline(cin, fileToDelete);
                string command = "DEL /p \"" + fileToDelete + "\"";
                system(command.c_str());
                cin.get();
            }
            else
            {
                system(userCommand.c_str());
                cin.get();
            }
        }
        else if  (choice == 6)
        {
            cout << "Вихід з програми." << endl;
            break;
        }
        else
        {
            cout << "Невірний вибір. Спробуйте ще раз." << endl;
        }
    }

    return 0;
}