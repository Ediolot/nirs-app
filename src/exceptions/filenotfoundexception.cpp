#include "filenotfoundexception.h"

FileNotFoundException::FileNotFoundException()
    : FileNotFoundException(QString(""))
{}

FileNotFoundException::FileNotFoundException(const QString &path)
    : FileNotFoundException(path.toStdString())
{}

FileNotFoundException::FileNotFoundException(const std::string &path)
    : message("File " + path + " not found")
{}

const char *FileNotFoundException::what() const throw()
{
    return message.c_str();
}
