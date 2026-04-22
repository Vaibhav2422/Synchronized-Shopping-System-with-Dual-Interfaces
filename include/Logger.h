#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <chrono>

// MinGW GCC 6.x does not expose std::mutex without POSIX threads.
// Since this application is single-threaded, we provide a no-op mutex shim.
#if defined(__MINGW32__) && !defined(_GLIBCXX_HAS_GTHREADS)
namespace std {
    struct mutex {
        void lock() {}
        void unlock() {}
        bool try_lock() { return true; }
    };
    template<typename M>
    struct lock_guard {
        explicit lock_guard(M&) {}
    };
}
#else
#include <mutex>
#endif

/**
 * Logger singleton class for system-wide logging.
 * Implements the Singleton design pattern: only one Logger instance exists
 * for the lifetime of the application, accessed via getInstance().
 * Private constructor and deleted copy/move prevent additional instances.
 * Writes timestamped log entries to data/system.log.
 * Thread-safe via std::mutex.
 */
class Logger {
public:
    enum class LogLevel {
        INFO,
        WARNING,
        ERROR,
        DEBUG
    };

    // Singleton access
    static Logger& getInstance();

    // Disable copy and assignment
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // Core logging
    std::string getCurrentTimestamp() const;
    void writeLog(LogLevel level, const std::string& component, const std::string& message);

    // Static convenience methods
    static void logInfo(const std::string& component, const std::string& message);
    static void logWarning(const std::string& component, const std::string& message);
    static void logError(const std::string& component, const std::string& message);
    static void logDebug(const std::string& component, const std::string& message);
    static void logException(const std::string& component, const std::exception& ex);
    static void clearLog();

    ~Logger();

private:
    Logger();

    std::ofstream logFile;
    std::mutex logMutex;

    static std::string levelToString(LogLevel level);
};

#endif // LOGGER_H
