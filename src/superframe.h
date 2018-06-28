#ifndef SUPERFRAME_H
#define SUPERFRAME_H

#include <cstdint>
#include <utility>

class Superframe
{
public:
    using TimestampData = uint64_t; /*! Type of value for the timestamp.                  */
    enum Type {
        ROW_MAJOR, COL_MAJOR
    };
    enum Tstamp {
        ENABLED, DISABLED
    };
    enum Pos {
        CURRENT = -1
    };

protected:
    int bpp;
    uint64_t timestamp; /*! Timestamp in nanoseconds (Since 01/01/1970). */

public:
    Superframe();
    Superframe(int bpp, uint64_t timestamp);
    explicit Superframe(const Superframe& other);
    explicit Superframe(Superframe&& other);

    TimestampData getTimestamp() const;
    void setTimestamp(const TimestampData& timestamp);
    int getBPP();
};

#endif // SUPERFRAME_H
