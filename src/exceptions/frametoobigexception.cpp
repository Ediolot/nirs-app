#include "frametoobigexception.h"

FrameTooBigException::FrameTooBigException()
    : FrameTooBigException(0, 0)
{}

FrameTooBigException::FrameTooBigException(int width, int height)
    : message(QString(
                  "Read dimensions are too big,  " + QString::number(width) + " x " + QString::number(height)
              ).toStdString())
{}

const char *FrameTooBigException::what() const throw()
{
    return message.c_str();
}
