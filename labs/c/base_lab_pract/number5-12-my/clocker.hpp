#ifndef CLOCKER_HPP
#define CLOCKER_HPP

#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include <conio.h>

using namespace std;



void print_big_number(int number, const vector<vector<string>>& digits) {
    string num_str = to_string(number);
    for (int row = 0; row < 11; ++row) {
        for (char c : num_str) {
            int d = c - '0';
            cout << digits[d][row] << "  ";
        }
        cout << endl;
    }
}

void clocker(int seconds) {
    vector<vector<string>> digits = {
        {
            "   *******   ",
            "  **     **  ",
            " **       ** ",
            "**         **",
            "**         **",
            "**         **",
            "**         **",
            "**         **",
            " **       ** ",
            "  **     **  ",
            "   *******   "
        },
        {
            "     ***     ",
            "    ****     ",
            "   ** **     ",
            "      **     ",
            "      **     ",
            "      **     ",
            "      **     ",
            "      **     ",
            "      **     ",
            "      **     ",
            "   ********* "
        },
        {
            "   *******   ",
            "  **     **  ",
            " **       ** ",
            "          ** ",
            "         **  ",
            "       **    ",
            "     **      ",
            "   **        ",
            " **         *",
            "*************",
            "*************"
        },
        {
            "   *******   ",
            "  **     **  ",
            " **       ** ",
            "          ** ",
            "       ****  ",
            "          ** ",
            "          ** ",
            "          ** ",
            " **       ** ",
            "  **     **  ",
            "   *******   "
        },
        {
            "       **    ",
            "      ***    ",
            "     ****    ",
            "    ** **    ",
            "   **  **    ",
            "  **   **    ",
            " *********** ",
            "       **    ",
            "       **    ",
            "       **    ",
            "   ********* "
        },
        {
            " *********** ",
            " **          ",
            " **          ",
            " **          ",
            " **********  ",
            "          ** ",
            "          ** ",
            "          ** ",
            " **      **  ",
            "  **    **   ",
            "   ******    "
        },
        {
            "   *******   ",
            "  **     **  ",
            " **         ",
            " **         ",
            " **********  ",
            " **      **  ",
            " **      **  ",
            " **      **  ",
            " **      **  ",
            "  **    **   ",
            "   ******    "
        },
        {
            " *********** ",
            "         **  ",
            "        **   ",
            "       **    ",
            "      **     ",
            "     **      ",
            "    **       ",
            "   **        ",
            "  **         ",
            " **          ",
            "**           "
        },
        {
            "   *******   ",
            "  **     **  ",
            " **      **  ",
            " **      **  ",
            "  **    **   ",
            "   ******    ",
            "  **    **   ",
            " **      **  ",
            " **      **  ",
            "  **     **  ",
            "   *******   "
        },
        {
            "   ******    ",
            "  **    **   ",
            " **      **  ",
            " **      **  ",
            "  **    **   ",
            "   ********* ",
            "         **  ",
            "         **  ",
            "        **   ",
            "  **   **    ",
            "   ****      "
        }
    };
    #ifdef _WIN64
    #endif
    for (int t = seconds; t > 0; --t) {
        system("cls");
        print_big_number(t, digits);
        cout << " " << endl;
        int waited = 0;
        while (waited < 10) { // Check every 0.1s for key press
            #ifdef _WIN64
            if (_kbhit()) {
                _getch();
                system("cls");
                cout << "Timer stopped early!" << endl;
                return;
            }
            #endif
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            waited++;
        }
    }
    system("cls");
    cout << "Timer finished!" << endl;
}


#endif