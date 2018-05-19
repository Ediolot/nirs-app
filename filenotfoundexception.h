#ifndef FILENOTFOUNDEXCEPTION_H
#define FILENOTFOUNDEXCEPTION_H


class FileNotFoundException : public std::exception
{
public:
    FileNotFoundException();
};

#endif // FILENOTFOUNDEXCEPTION_H