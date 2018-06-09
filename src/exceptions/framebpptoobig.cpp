#include "framebpptoobig.h"

FrameBPPTooBig::FrameBPPTooBig()
    : message("Bits per pixel value is too high for the frame")
{}

FrameBPPTooBig::FrameBPPTooBig(int bpp)
    : message("Bits per pixel value is too high for the frame, " + QString::number(bpp).toStdString())
{}

const char *FrameBPPTooBig::what() const throw()
{
    return message.c_str();
}
