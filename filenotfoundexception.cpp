#include "filenotfoundexception.h"

FileNotFoundException::FileNotFoundException():
    message("File not found")
{}

FileNotFoundException::FileNotFoundException(const std::string &path):
    message("File " + path + " not found")
{}

const char *FileNotFoundException::what() const throw()
{
    return message.c_str();
}
