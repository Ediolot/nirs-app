#ifndef FRAME_H
#define FRAME_H

#include <string>
#include <Eigen/Dense>
#include <iostream>
#include <vector>

// TODO check https://stackoverflow.com/questions/115703/storing-c-template-function-definitions-in-a-cpp-file

using TimestampData = uint64_t; // TODO poner dentro ?

template<class T>
class Frame
{
using DataMatrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;
using Mapper     = Eigen::Map<const DataMatrix>;

private:
    TimestampData timestamp; // ms
    DataMatrix data;

public:
    /*!
     * \brief Build an empty frame
     */
    Frame(): timestamp(0), data()
    {} // TODO

    /*!
     * \brief Frame
     * \param vdata
     * \param width
     * \param height
     * \param timestamp
     */
    Frame(const std::vector<T>& vdata, size_t width, size_t height, TimestampData timestamp = 0): // TODO move vector data, no copy (?)
        timestamp(timestamp),
        data(Mapper(vdata.data(), height, width))
    {}

    template<class U>
    Frame(const Frame<U>& other):
        timestamp(other.getTimestamp()),
        data(other.getData().template cast<T>())
    {}

    template<class U>
    Frame<T>& operator=(const Frame<U>& other)
    {
        timestamp = other.getTimestamp();
        data = other.getData().template cast<T>();
        return *this;
    }

    template<class U>
    Frame<U> cast() {
        return Frame<U>(*this);
    }

    /*!
     * Returns bits per pixel used by the frame.
     */
    uint32_t getBPP() const {
        return sizeof(T);
    }

    /*!
     * Returns matrix data.
     */
    const DataMatrix& getData() const {
        return data;
    }

    TimestampData getTimestamp() const {
        return timestamp;
    }

    uint32_t getWidth() const {
        return data.cols();
    }

    uint32_t getHeight() const {
        return data.rows();
    }
};

#endif // FRAME_H
