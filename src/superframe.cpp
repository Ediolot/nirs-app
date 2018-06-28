#include "superframe.h"

Superframe::Superframe()
    : Superframe(0 ,0)
{}

Superframe::Superframe(int bpp, uint64_t timestamp)
    : bpp(bpp)
    , timestamp(timestamp)
{}

Superframe::Superframe(const Superframe &other)
    : Superframe(other.bpp, other.timestamp)
{}

Superframe::Superframe(Superframe &&other)
    : bpp(other.bpp)
    , timestamp(std::move(other.timestamp))
{}

Superframe::TimestampData Superframe::getTimestamp() const
{
    return timestamp;
}

void Superframe::setTimestamp(const Superframe::TimestampData &timestamp)
{
    this->timestamp = timestamp;
}

int Superframe::getBPP()
{
    return bpp;
}
