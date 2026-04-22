/**
 * Version.h - System metadata and version information
 * Requirements: 25.1, 25.2, 25.3, 25.4, 25.6, 25.7
 */

#ifndef VERSION_H
#define VERSION_H

#include "ANSIColors.h"
#include <iostream>
#include <string>

namespace Version {

    // Compile-time constants (Requirement 25.6)
    constexpr const char* VERSION       = "1.0.0";
    constexpr const char* PROJECT_NAME  = "Shopping Management System";
    constexpr const char* AUTHOR        = "Shopping Management System Team";

    // Build timestamp using compiler-provided macros (Requirement 25.2)
    constexpr const char* BUILD_TIMESTAMP = __DATE__ " " __TIME__;

    /**
     * Displays an ASCII art banner with project name, version, author,
     * and build timestamp using ANSI colors. (Requirement 25.2)
     */
    inline void displayBanner() {
        std::cout << ANSIColors::BG_BLACK;
        std::cout << ANSIColors::CYAN << ANSIColors::BOLD;
        std::cout << "============================================================\n";
        std::cout << "   ____  _                       _\n";
        std::cout << "  / ___|| |__   ___  _ __  _ __ (_)_ __   __ _\n";
        std::cout << "  \\___ \\| '_ \\ / _ \\| '_ \\| '_ \\| | '_ \\ / _` |\n";
        std::cout << "   ___) | | | | (_) | |_) | |_) | | | | | (_| |\n";
        std::cout << "  |____/|_| |_|\\___/| .__/| .__/|_|_| |_|\\__, |\n";
        std::cout << "                    |_|   |_|             |___/\n";
        std::cout << ANSIColors::RESET;
        std::cout << ANSIColors::BG_BLACK;
        std::cout << ANSIColors::YELLOW << ANSIColors::BOLD;
        std::cout << "============================================================\n";
        std::cout << "  " << PROJECT_NAME << "\n";
        std::cout << ANSIColors::RESET << ANSIColors::BG_BLACK;
        std::cout << ANSIColors::GREEN;
        std::cout << "  Version   : " << VERSION << "\n";
        std::cout << "  Author    : " << AUTHOR << "\n";
        std::cout << "  Built     : " << BUILD_TIMESTAMP << "\n";
        std::cout << ANSIColors::CYAN;
        std::cout << "============================================================\n";
        std::cout << ANSIColors::RESET << "\n";
    }

    /**
     * Displays a compact version info block (used in About menu option).
     */
    inline void displayAbout() {
        std::cout << ANSIColors::YELLOW << ANSIColors::BOLD;
        std::cout << "============================================================\n";
        std::cout << "  About " << PROJECT_NAME << "\n";
        std::cout << "============================================================\n";
        std::cout << ANSIColors::RESET;
        std::cout << ANSIColors::GREEN;
        std::cout << "  Version   : " << VERSION << "\n";
        std::cout << "  Author    : " << AUTHOR << "\n";
        std::cout << "  Built     : " << BUILD_TIMESTAMP << "\n";
        std::cout << ANSIColors::CYAN;
        std::cout << "============================================================\n";
        std::cout << ANSIColors::RESET;
    }

} // namespace Version

#endif // VERSION_H
