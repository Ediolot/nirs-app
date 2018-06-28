#ifndef FRAMETOOBIGEXCEPTION_H
#define FRAMETOOBIGEXCEPTION_H

#include <QString>
#include <exception>
#include <string>

class FrameTooBigException : public std::exception
{
private:
    std::string message;

public:
    FrameTooBigException();
    explicit FrameTooBigException(int width, int height);
    const char* what() const throw();
};

#endif // FRAMETOOBIGEXCEPTION_H
