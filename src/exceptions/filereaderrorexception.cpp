#include "filereaderrorexception.h"

FileReadErrorException::FileReadErrorException()
    : message("Error while reading file")
{}

FileReadErrorException::FileReadErrorException(const std::string &path)
    : message("Error while reading file " + path )
{}

FileReadErrorException::FileReadErrorException(const QString &path)
    : message(("Error while reading file" + path).toStdString())
{}

const char *FileReadErrorException::what() const throw()
{
    return message.c_str();
}
