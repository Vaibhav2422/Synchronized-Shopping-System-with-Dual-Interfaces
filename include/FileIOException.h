#ifndef FILE_IO_EXCEPTION_H
#define FILE_IO_EXCEPTION_H

#include "ShoppingException.h"

/**
 * Base exception for file operation errors.
 */
class FileIOException : public ShoppingException {
protected:
    std::string filename;
    
public:
    explicit FileIOException(const std::string& file, const std::string& operation)
        : ShoppingException("File " + operation + " error: " + file, "FILE_ERR")
        , filename(file) {}
    
    std::string getFilename() const { return filename; }
};

/**
 * Exception thrown when a file is not found.
 */
class FileNotFoundException : public FileIOException {
public:
    explicit FileNotFoundException(const std::string& file)
        : FileIOException(file, "not found") {}
};

/**
 * Exception thrown when a file write operation fails.
 */
class FileWriteException : public FileIOException {
public:
    explicit FileWriteException(const std::string& file)
        : FileIOException(file, "write") {}
};

/**
 * Exception thrown when a file read operation fails.
 */
class FileReadException : public FileIOException {
public:
    explicit FileReadException(const std::string& file)
        : FileIOException(file, "read") {}
};

#endif // FILE_IO_EXCEPTION_H
