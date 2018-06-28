#include "sizeframemissmatch.h"

SizeFrameMissmatch::SizeFrameMissmatch()
    : SizeFrameMissmatch(0, 0, 0, 0)
{}

SizeFrameMissmatch::SizeFrameMissmatch(int widthA, int heightA, int widthB, int heightB)
    : message(QString("Frames size missmatch, " +
                      QString::number(widthA) + " x " + QString::number(heightA) + ", " +
                      QString::number(widthB) + " x " + QString::number(heightB)
              ).toStdString())
{}

const char *SizeFrameMissmatch::what() const throw()
{
    return message.c_str();
}
