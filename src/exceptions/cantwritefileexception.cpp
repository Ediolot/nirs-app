#include "cantwritefileexception.h"

CantWriteFileException::CantWriteFileException()
    : CantWriteFileException(QString(""))
{}

CantWriteFileException::CantWriteFileException(const QString &path)
    : CantWriteFileException(path.toStdString())
{}

CantWriteFileException::CantWriteFileException(const std::string &path)
    : message("Could not write file at " + path)
{}

const char *CantWriteFileException::what() const throw()
{
    return message.c_str();
}
