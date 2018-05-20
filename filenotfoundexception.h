#ifndef FILENOTFOUNDEXCEPTION_H
#define FILENOTFOUNDEXCEPTION_H

#include <string>
#include <exception>

class FileNotFoundException : public std::exception // TODO Correct inheritance ? or std::runtime ?
{
private:
    std::string message;

public:
    FileNotFoundException();
    explicit FileNotFoundException(const std::string &path);
    const char* what() const throw();
};

#endif // FILENOTFOUNDEXCEPTION_H
