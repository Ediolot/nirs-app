#include "filereaderrorexception.h"

FileReadErrorException::FileReadErrorException()
    : FileReadErrorException(QString(""))
{}

FileReadErrorException::FileReadErrorException(const QString &path)
    : FileReadErrorException(path.toStdString())
{}

FileReadErrorException::FileReadErrorException(const std::string &path)
    : message("Error while reading file " + path)
{}

const char *FileReadErrorException::what() const throw()
{
    return message.c_str();
}
