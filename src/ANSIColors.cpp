#include "ANSIColors.h"
#include <iostream>

namespace ANSIColors {

    void clearScreen() {
        // ANSI escape: erase entire display and move cursor to home position
        std::cout << "\033[2J\033[H";
        std::cout.flush();
    }

    void setCursorPosition(int r, int c) {
        // ANSI escape: move cursor to row r, column c (1-indexed)
        std::cout << "\033[" << r << ";" << c << "H";
        std::cout.flush();
    }

    void hideCursor() {
        // ANSI escape: hide the terminal cursor
        std::cout << "\033[?25l";
        std::cout.flush();
    }

    void showCursor() {
        // ANSI escape: restore the terminal cursor
        std::cout << "\033[?25h";
        std::cout.flush();
    }
}