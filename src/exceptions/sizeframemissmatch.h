#ifndef SIZEFRAMEMISSMATCH_H
#define SIZEFRAMEMISSMATCH_H

#include <QString>
#include <exception>
#include <string>

class SizeFrameMissmatch : public std::exception
{
private:
    std::string message;

public:
    SizeFrameMissmatch();
    explicit SizeFrameMissmatch(int widthA, int heightA, int widthB, int heightB);
    const char* what() const throw();
};

#endif // SIZEFRAMEMISSMATCH_H
