# Deployment Guide

## Prerequisites

### Required
- C++17 compatible compiler (see versions below)
- `make` build tool
- Terminal with ANSI color support (see Terminal Requirements)

### Optional
- RapidCheck (included as submodule in `rapidcheck/`) — for property-based tests
- Valgrind — for memory leak detection (Linux only)

### Minimum Compiler Versions

| Compiler | Minimum Version | Notes |
|----------|----------------|-------|
| GCC (g++) | 7.0+ | Recommended: GCC 9+ |
| Clang | 5.0+ | Full C++17 support |
| MSVC | Visual Studio 2017 (19.14+) | Use with CMake or nmake |
| MinGW-w64 | GCC 7.0+ | Windows native build |

---

## Platform Instructions

### Linux

#### Install Dependencies

**Ubuntu / Debian:**
```bash
sudo apt update
sudo apt install g++ make
```

**Fedora / RHEL:**
```bash
sudo dnf install gcc-c++ make
```

**Arch Linux:**
```bash
sudo pacman -S gcc make
```

#### Build and Run

```bash
# Clone or extract the project
cd shopping-management-system

# Build the application
make app

# Run the application
./bin/shopping_system

# Or use the Makefile shortcut
make run
```

#### Run Tests

```bash
# All unit tests
make test_all

# Property-based tests (requires RapidCheck build)
make test_pbt
```

#### Memory Check (optional)

```bash
sudo apt install valgrind
valgrind --leak-check=full ./bin/shopping_system
```

---

### Windows (MinGW-w64)

#### Install MinGW-w64

**Option A — MSYS2 (recommended):**
1. Download MSYS2 from https://www.msys2.org/
2. Open MSYS2 MinGW 64-bit terminal
3. Run:
   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make
   ```
4. Add `C:\msys64\mingw64\bin` to your system PATH

**Option B — WinLibs standalone:**
1. Download from https://winlibs.com/
2. Extract and add the `bin/` folder to your system PATH

#### Build and Run

Open Command Prompt or PowerShell in the project directory:

```cmd
# Build the application
mingw32-make app

# Run the application
bin\shopping_system.exe

# Or use the Makefile shortcut
mingw32-make run
```

#### Run Tests

```cmd
mingw32-make test_all
```

#### Notes for Windows
- Use `mingw32-make` instead of `make`
- Binary output is `bin\shopping_system.exe`
- Ensure your terminal supports ANSI colors (see Terminal Requirements)
- If `mingw32-make` is not found, try `make` if it is aliased in your MinGW installation

---

### Windows (MSVC)

MSVC is not directly supported by the provided Makefile. Use the platform-specific Makefile:

```cmd
nmake /f Makefile.windows
```

Or build manually:

```cmd
cl /std:c++17 /EHsc /Iinclude /W3 src\*.cpp /Fe:bin\shopping_system.exe
```

Requires Visual Studio 2017 or later with the "Desktop development with C++" workload installed.

---

### WSL (Windows Subsystem for Linux)

WSL provides a Linux environment on Windows and is the recommended approach for Windows users who want full ANSI color support.

#### Setup WSL

1. Open PowerShell as Administrator and run:
   ```powershell
   wsl --install
   ```
2. Restart your computer
3. Open the installed Linux distribution (Ubuntu by default)

#### Install Dependencies in WSL

```bash
sudo apt update
sudo apt install g++ make
```

#### Build and Run

```bash
cd /mnt/c/path/to/shopping-management-system

make app
./bin/shopping_system
```

#### Terminal Recommendation for WSL
Use **Windows Terminal** with the WSL profile for the best ANSI color rendering experience.

---

### macOS

#### Install Dependencies

**Using Homebrew (recommended):**
```bash
brew install gcc make
```

**Using Xcode Command Line Tools:**
```bash
xcode-select --install
```

Note: The default `clang` on macOS supports C++17 from Xcode 9.3+. However, GCC via Homebrew is recommended for consistency.

#### Build and Run

```bash
# If using Homebrew GCC (e.g., gcc-13):
make app CXX=g++-13

# If using system clang (Xcode 9.3+):
make app CXX=clang++

# Run
./bin/shopping_system
```

#### Run Tests

```bash
make test_all CXX=g++-13
```

---

## Terminal Requirements

### ANSI Color Support

The application uses ANSI escape codes for:
- Cyan text — menu options
- Yellow text — headings and monetary values
- Green text — success messages
- Red text — error messages
- Black background — dark theme

### Recommended Terminals

| Platform | Recommended Terminal | Notes |
|----------|---------------------|-------|
| Windows | Windows Terminal | Full ANSI support, available from Microsoft Store |
| Windows | MSYS2 terminal | Good ANSI support for MinGW builds |
| macOS | iTerm2 | Full ANSI support with true color |
| macOS | Terminal.app | Basic ANSI support (macOS 10.12+) |
| Linux | GNOME Terminal | Full ANSI support |
| Linux | Konsole (KDE) | Full ANSI support |
| Linux | xterm | Basic ANSI support |

### Enabling ANSI Colors on Windows

**Windows Terminal:** ANSI colors are enabled by default.

**Command Prompt (cmd.exe):** ANSI support requires Windows 10 version 1511 or later. Enable via registry or use:
```cmd
reg add HKCU\Console /v VirtualTerminalLevel /t REG_DWORD /d 1
```

**PowerShell:** ANSI colors work natively in PowerShell 5.1+ on Windows 10+.

**Legacy terminals (Windows 7/8):** ANSI codes will appear as raw escape sequences. Use MSYS2 or WSL for proper display.

### Fallback for Terminals Without ANSI Support

If your terminal does not support ANSI codes, the application will still function correctly — menu text and output will appear without color formatting. All functionality remains intact; only the visual styling is affected.

---

## Troubleshooting

### Compilation Errors

**Error: `'filesystem' is not a member of 'std'`**
- Cause: Compiler does not fully support C++17
- Fix: Upgrade to GCC 8+ or add `-lstdc++fs` to LDFLAGS in the Makefile

**Error: `fatal error: nlohmann/json.hpp: No such file or directory`**
- Cause: nlohmann/json header not found
- Fix: The project uses a bundled or system-installed header. Ensure `include/` contains `json.hpp` or install via:
  ```bash
  sudo apt install nlohmann-json3-dev   # Ubuntu/Debian
  brew install nlohmann-json            # macOS
  ```

**Error: `'make' is not recognized`** (Windows)
- Cause: MinGW `make` not in PATH
- Fix: Use `mingw32-make` instead, or add MinGW `bin/` to system PATH

**Error: `undefined reference to 'WinMain'`** (Windows)
- Cause: Linker looking for Windows GUI entry point
- Fix: Ensure `-mconsole` flag is present or use `Makefile.windows`

**Warning: `#warning This file requires compiler and library support...`**
- Cause: C++17 features not fully enabled
- Fix: Ensure `-std=c++17` flag is set (already in Makefile)

### Runtime Errors

**Error: `Cannot open data/config.json`**
- Cause: Missing config file
- Fix: The system uses defaults automatically. To customize, create `data/config.json` based on the sample in the README.

**Error: `Cannot create data/receipts/`**
- Cause: Missing directory
- Fix: Create manually:
  ```bash
  mkdir -p data/receipts   # Linux/macOS
  mkdir data\receipts      # Windows
  ```
  Or run `make directories` to create all required directories.

**Error: `Permission denied` on log file**
- Cause: `data/system.log` is read-only or owned by another user
- Fix:
  ```bash
  chmod 644 data/system.log   # Linux/macOS
  ```

**Application exits immediately / no output**
- Cause: Binary not found or wrong path
- Fix: Ensure you ran `make app` successfully and run from the project root directory

### ANSI Color Display Issues

**Escape codes appear as `^[[0;36m` or similar**
- Cause: Terminal does not support ANSI escape codes
- Fix: Switch to a supported terminal (see Terminal Requirements above)

**Colors appear but are wrong / garbled**
- Cause: Terminal color scheme conflict
- Fix: Reset terminal colors before running: `echo -e "\033[0m"` (Linux/macOS)

**Black background not showing on macOS Terminal.app**
- Cause: Terminal.app uses its own background color setting
- Fix: Go to Terminal → Preferences → Profiles and set background to black, or use iTerm2

### Missing Directory Issues

If `obj/`, `bin/`, or `data/` directories are missing, run:

```bash
make directories    # Linux/macOS
mingw32-make directories   # Windows
```

This creates all required directories automatically.

---

## Quick Reference

| Task | Linux/macOS | Windows (MinGW) |
|------|-------------|-----------------|
| Build app | `make app` | `mingw32-make app` |
| Run app | `./bin/shopping_system` | `bin\shopping_system.exe` |
| Run all tests | `make test_all` | `mingw32-make test_all` |
| Run PBT tests | `make test_pbt` | `mingw32-make test_pbt` |
| Clean build | `make clean` | `mingw32-make clean` |
| Full rebuild | `make rebuild` | `mingw32-make rebuild` |
| Create dirs | `make directories` | `mingw32-make directories` |
