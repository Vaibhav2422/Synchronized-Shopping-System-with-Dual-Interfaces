#include "Logger.h"
#include "Version.h"
#include <iomanip>
#include <sstream>

// Use platform-specific directory creation since <filesystem> requires GCC 8+
#ifdef _WIN32
#include <direct.h>
#define CREATE_DIR(p) _mkdir(p)
#else
#include <sys/stat.h>
#define CREATE_DIR(p) mkdir(p, 0755)
#endif

// SECURITY: Log file is restricted to data/ directory (Requirement 27.3)
static const char* LOG_FILE_PATH = "data/system.log";

// SECURITY: Set restrictive file permissions after creation (Requirement 27.5).
// On POSIX systems, chmod() restricts access to owner only (mode 0600).
// On Windows, NTFS ACLs control access; no action is taken here — administrators
// should configure appropriate ACLs on the data/ directory.
static void setFilePermissions(const char* path, int mode) {
#ifndef _WIN32
    chmod(path, static_cast<mode_t>(mode));
#else
    (void)path;
    (void)mode;
    // Windows: rely on NTFS ACLs configured by the administrator.
#endif
}

Logger::Logger() {
    // Ensure data/ directory exists; if not, skip file logging gracefully
    CREATE_DIR("data");

    // SECURITY: Log file path is hardcoded to data/system.log to prevent
    // directory traversal — no user input influences this path (Requirement 27.3)
    logFile.open(LOG_FILE_PATH, std::ios::app);
    // If open fails, we continue without file logging (no crash)

    // SECURITY: Set restrictive permissions (600) on the log file so only
    // the owner can read/write it (Requirement 27.5).
    setFilePermissions(LOG_FILE_PATH, 0600);
    // If open fails, we continue without file logging (no crash)

    // Write log file header with version info (Requirement 25.4)
    if (logFile.is_open()) {
        logFile << "============================================================\n";
        logFile << "  " << Version::PROJECT_NAME << " v" << Version::VERSION << "\n";
        logFile << "  Built: " << Version::BUILD_TIMESTAMP << "\n";
        logFile << "  Log started: " << getCurrentTimestamp() << "\n";
        logFile << "============================================================\n";
        logFile.flush();
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

std::string Logger::getCurrentTimestamp() const {
    using namespace std::chrono;

    auto now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    std::time_t t = system_clock::to_time_t(now);
    std::tm* tm_info = std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(tm_info, "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:    return "INFO   ";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR  ";
        case LogLevel::DEBUG:   return "DEBUG  ";
        default:                return "UNKNOWN";
    }
}

void Logger::writeLog(LogLevel level, const std::string& component, const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);

    std::string entry = "[" + getCurrentTimestamp() + "] "
                      + "[" + levelToString(level) + "] "
                      + "[" + component + "] "
                      + message;

    if (logFile.is_open()) {
        logFile << entry << "\n";
        logFile.flush();
    }
}

void Logger::logInfo(const std::string& component, const std::string& message) {
    getInstance().writeLog(LogLevel::INFO, component, message);
}

void Logger::logWarning(const std::string& component, const std::string& message) {
    getInstance().writeLog(LogLevel::WARNING, component, message);
}

void Logger::logError(const std::string& component, const std::string& message) {
    getInstance().writeLog(LogLevel::ERROR, component, message);
}

void Logger::logDebug(const std::string& component, const std::string& message) {
    getInstance().writeLog(LogLevel::DEBUG, component, message);
}

void Logger::logException(const std::string& component, const std::exception& ex) {
    getInstance().writeLog(LogLevel::ERROR, component,
        std::string("Exception: ") + ex.what());
}

void Logger::clearLog() {
    Logger& logger = getInstance();
    std::lock_guard<std::mutex> lock(logger.logMutex);

    if (logger.logFile.is_open()) {
        logger.logFile.close();
    }

    // Reopen in truncate mode to clear, then switch back to append
    logger.logFile.open(LOG_FILE_PATH, std::ios::trunc);
    logger.logFile.close();
    logger.logFile.open(LOG_FILE_PATH, std::ios::app);
}
