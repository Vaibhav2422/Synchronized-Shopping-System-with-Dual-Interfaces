#ifndef ANSICOLORS_H
#define ANSICOLORS_H

namespace ANSIColors {
    // Background colors
    constexpr const char* BG_BLACK = "\033[40m";
    constexpr const char* BG_RESET = "\033[49m";
    
    // Text colors
    constexpr const char* CYAN = "\033[96m";
    constexpr const char* YELLOW = "\033[93m";
    constexpr const char* GREEN = "\033[92m";
    constexpr const char* RED = "\033[91m";
    constexpr const char* WHITE = "\033[97m";
    constexpr const char* RESET = "\033[0m";
    
    // Text styles
    constexpr const char* BOLD = "\033[1m";
    constexpr const char* UNDERLINE = "\033[4m";
    
    // Utility functions
    void clearScreen();
    void setCursorPosition(int row, int col);
    void hideCursor();
    void showCursor();
}

#endif // ANSICOLORS_H
