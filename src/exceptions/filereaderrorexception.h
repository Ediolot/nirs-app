#ifndef FILEREADERROREXCEPTION_H
#define FILEREADERROREXCEPTION_H

#include <QString>
#include <exception>
#include <string>

class FileReadErrorException : public std::exception
{
private:
    std::string message;

public:
    FileReadErrorException();
    explicit FileReadErrorException(const std::string &path);
    explicit FileReadErrorException(const QString &path);
    const char* what() const throw();
};

#endif // FILENOTFOUNDEXCEPTION_H
