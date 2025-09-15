#include <iostream>
#include <cstdlib>
#include <string> 

using namespace std;

int main() {
    int choice;
    bool exitProgram = false;
    while (!exitProgram) {
        cout << "Menu:\n";
        cout << "1. Run lab1\n";
        cout << "2. Run lab2\n";
        cout << "3. Run lab3\n";
        cout << "4. Run lab4\n";
        cout << "5. Run lab5\n";
        cout << "6. Run lab6\n";
        cout << "7. Run lab7\n";
        cout << "8. Run lab8\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
while (true) {
    if(choice == 9) {
        cout << "Your program is ended\n";
        cout << "To exit press any key\n";
        cin.ignore();
        cin.get();
        exit(0);
    }

    if (choice == 1) {
        system("cls");
            system("\"number1-12-my\\lab12-koval.exe\"");
            break;
        } else if (choice == 2) {
            system("cls");
            system("\"number2-12-my\\lab12.exe\"");
            break;
        } else if (choice == 3) {
            system("cls");
            system("\"number3-12-my]\\lab12-advanced.exe\"");
            break;
        } else if (choice == 4) {
            system("cls");
            system("\"number4-12-my\\lab12.exe\"");
            break;
        } else if (choice == 5) {
            system("cls");
            system("\"number5-12-my\\lab12-advanced-2.exe\"");
            break;
        } else if (choice == 6) {
            system("cls");
            system("\"number6-12-my\\lab12.exe\"");
            break;
        } else if (choice == 7) {
            system("cls");  
            system("\"number7-12-my\\lab12.exe\"");
            break;
        } else if (choice == 8) {
            system("cls");
            system("\"number8-12-my\\lab12.exe\"");
            break;
        } 
    }
   
    while (true) {
        cout << "Do you want try again?\n";
        cout << "Yes/No ";
        string checker;
        cin >> checker;
        if (checker == "Yes" || checker == "yes") {
            system("cls");
            break;
        } else if (checker == "No" || checker == "no") {
            exitProgram = true;
            break;
        } else {
            system("cls");
            cout << "Your input is incorrect.\n";
            cout << "Try again.\n";
        }
    }
}
cout << "To exit press any key\n";
cin.get();
cin.ignore();
    return 0;
}