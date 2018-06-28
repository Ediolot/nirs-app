#ifndef CANTWRITEFILEEXCEPTION_H
#define CANTWRITEFILEEXCEPTION_H

#include <QString>
#include <exception>
#include <string>

class CantWriteFileException : public std::exception
{
private:
    std::string message;

public:
    CantWriteFileException();
    explicit CantWriteFileException(const std::string &path);
    explicit CantWriteFileException(const QString &path);
    const char* what() const throw();
};

#endif // CANTWRITEFILEEXCEPTION_H
