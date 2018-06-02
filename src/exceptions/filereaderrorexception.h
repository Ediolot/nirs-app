#ifndef FILENOTFOUNDEXCEPTION_H
#define FILENOTFOUNDEXCEPTION_H

#include <QString>
#include <string>
#include <exception>

class FileReadErrorException : public std::exception // TODO Correct inheritance ? or std::runtime ?
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
