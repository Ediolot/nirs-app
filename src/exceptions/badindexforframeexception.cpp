#include "badindexforframeexception.h"

BadIndexForFrameException::BadIndexForFrameException()
    : BadIndexForFrameException(0)
{}

BadIndexForFrameException::BadIndexForFrameException(int index, int max)
    : message()
{
    QString msg = "Cant access frame " + QString::number(index);
    if (max != NO_MAX) {
        msg += ", maximum is" + QString::number(max);
    }
    message = msg.toStdString();
}

const char *BadIndexForFrameException::what() const throw()
{
    return message.c_str();
}
