#ifndef FRAMEBPPTOOBIG_H
#define FRAMEBPPTOOBIG_H

#include <QString>
#include <exception>
#include <string>

class FrameBPPTooBig : public std::exception
{
private:
    std::string message;

public:
    FrameBPPTooBig();
    explicit FrameBPPTooBig(int bpp);
    const char* what() const throw();
};

#endif // FRAMEBPPTOOBIG_H
