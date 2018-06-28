#ifndef BADINDEXFORFRAMEEXCEPTION_H
#define BADINDEXFORFRAMEEXCEPTION_H

#include <QString>
#include <exception>
#include <string>

class BadIndexForFrameException : public std::exception
{
private:
    std::string message;
    enum {
      NO_MAX = -1
    };

public:
    BadIndexForFrameException();
    explicit BadIndexForFrameException(int index, int max = NO_MAX);
    const char* what() const throw();
};

#endif // BADINDEXFORFRAMEEXCEPTION_H
