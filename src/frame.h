#ifndef FRAME_H
#define FRAME_H

#include <Eigen/Dense>
#include <QVector>
#include <cmath>
#include <utility>
#include <QFile>
#include <QImage>
#include <QByteArray>
#include <QBuffer>
#include "exceptions/filereaderrorexception.h"

namespace FrameConstants {
    const bool HAS_TIMESTAMP = true;
    const bool NO_TIMESTAMP = false;
    const int ROW_MAJOR = 0;
    const int COLUM_MAJOR = 1;
}

/*!
 * \brief The frame class is a template class that manipulates frames.
 *
 * The Frame class can reads the data from a vector and saves it internally
 * to an Eigen matrix. Frame may be used to do simple operations between them
 * which includes casting from one frame type to another.
 */
template<class T>
class Frame
{
public:
    using TimestampData = uint64_t;                                                          /*! Type of value for the timestamp.                  */
    using DataMatrix    = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>; /*! Type of value for the raw data Eigen matrix.      */
    using Mapper        = Eigen::Map<const DataMatrix>;                                      /*! Mapper type to build Eigen matrices from vectors. */

private:
    TimestampData timestamp; /*! Timestamp in nanoseconds (Since 01/01/1970). */ // TODO since 1997 ?
    DataMatrix data;         /*! Raw frame data. Saved in an Eigen matrix.    */

public:

    /*!
     * \brief Construct an empty frame with timestamp 0.
     */
    Frame();

    /*!
     * \brief Construct an empty frame. (All data is set to 0).
     * \param width Frame width (number of columns).
     * \param height Frame height (number of rows).
     * \param timestamp Timestamp in milliseconds.
     */
    Frame(size_t width, size_t height, TimestampData timestamp = 0);

    /*!
     * \brief Construct a frame from an array of data. Subsequent data in the input array form columns.
     * \param vdata Vector containing the frame raw data.
     * \param width Frame width (number of columns).
     * \param height Frame height (number of rows).
     * \param timestamp Timestamp in milliseconds.
     */
    Frame(const QVector<T>& vdata, size_t width, size_t height, TimestampData timestamp = 0);

    /*!
     * \brief Construct a frame from an array a file. Subsequent data in the file form columns.
     * \param file File containing the raw data. The file must be opened before calling this function.
     * \param width Frame width (number of columns).
     * \param height Frame height (number of rows).
     * \param hasTimestamp Indicates if the raw data contains information about the timestamp or not.
     * \param pos Position in which the frame is located within the file. If pos is -1, the frame is assumed to be in the current position.
     */
    Frame(QFile& file, uint32_t width, uint32_t height, bool hasTimestamp, int64_t pos = -1);

    /*!
     * \brief Construct a copy of another frame.
     */
    Frame(const Frame& other);

    /*!
     * \brief Construct a copy from another frame of another type. The values are transformed using
     * an static cast.
     */
    template<class U>
    Frame(const Frame<U>& other);

    /*!
     * \brief Construct a frame from an rvalue.
     */
    Frame(Frame&& other);

    /*!
     * \brief Copy the timestamp and frame data from another Frame.
     * \return *this
     */
    Frame& operator=(const Frame& other);

    /*!
     * \brief Copy the timestamp and frame data from another frame of another type. The values are
     * transformed using an static cast.
     * \return A reference to this object.
     */
    template<class U>
    Frame& operator=(const Frame<U>& other);

    /*!
     * \brief Set the timestamp and frame data from an rvalue.
     * \return A reference to this object.
     */
    Frame& operator=(Frame&& other);

    /*!
     * \brief Create a copy of this frame whose values are transformed to an specific
     * frame type using static cast.
     * \return The new frame.
     */
    template<class U>
    Frame<U> cast();

    /*!
     * \return
     */
    QVariantList toQVariantList(int major, bool norm = false) const;

    /*!
     * \return
     */
    void set(uint32_t row, uint32_t colum, T val);

    /*!
     * \brief Get value at position.
     * \param colum
     * \param row
     * \return Constant reference to value.
     */
    const T& at(uint32_t row, uint32_t colum) const;

    /*!
     * \brief Split the information of this frame into two new separate frames horizontally.
     * \param colum Colum index indicating the ending of the left frame.
     * \param left The new left frame will be saved here.
     * \param right the new right frame will be saved here.
     */
    void horizontalSplit(uint32_t colum, Frame& left, Frame& right) const;

    /*!
     * \brief Split the information of this frame into two new separate frames vertically.
     * \param row Row index indicating the ending of the top frame.
     * \param top The new left frame will be saved here.
     * \param bottom the new right frame will be saved here.
     */
    void verticalSplit(uint32_t row, Frame& top, Frame& bottom) const;

    /*!
     * \return Bits per pixel used by the frame.
     */
    uint32_t getBPP() const;

    /*!
     * \return Frame raw data in the form of an Eigen matrix.
     */
    const DataMatrix& getData() const;

    /*!
     * \return The timestamp since 01/01/1997 in milliseconds.
     */
    TimestampData getTimestamp() const;

    /*!
     * \return Frame width (number of columns).
     */
    uint32_t getWidth() const;

    /*!
     * \return Frame height (number of rows).
     */
    uint32_t getHeight() const;

    /*!
     * \return Minimum value in the frame data.
     */
    T getMin() const;

    /*!
     * \return Maximum value in the frame data.
     */
    T getMax() const;

    /*!
     * \brief Apply the natural logarithm to the frame element wise.
     * \return A reference to this object.
     */
    Frame& applyLog();

    /*!
     * \brief Adds this frame with another.
     * \return A reference to this object.
     */
    Frame& operator+=(const Frame& other);

    /*!
     * \brief Subtracts this frame with another.
     * \return A reference to this object.
     */
    Frame& operator-=(const Frame& other);

    /*!
     * \brief Adds this frame with an scalar value.
     * \return A reference to this object.
     */
    Frame& operator+=(const T& scalar);

    /*!
     * \brief Subtracts this frame with an scalar value.
     * \return A reference to this object.
     */
    Frame& operator-=(const T& scalar);

    /*!
     * \brief Multiply this frame with an scalar value.
     * \return A reference to this object.
     */
    Frame& operator*=(const T& scalar);

    /*!
     * \brief Divide this frame with an scalar value.
     * \return A reference to this object.
     */
    Frame& operator/=(const T& scalar);

    /*!
     * \brief Adds this frame with another into a new frame.
     * \return New result frame.
     */
    Frame operator+(const Frame& other) const;

    /*!
     * \brief Subtracts another frame to this frame into a new frame.
     * \return New result frame.
     */
    Frame operator-(const Frame& other) const;

    /*!
     * \brief Multiply this frame with another into a new frame, element wise.
     * \return New result frame.
     */
    Frame operator*(const Frame& other) const;

    /*!
     * \brief Divide this frame with another into a new frame, element wise.
     * \return New result frame.
     */
    Frame operator/(const Frame& other) const;
};

/*==================================== TEMPLATE FUNCTIONS ====================================*/
template<class T>
Frame<T>::Frame()
    : timestamp(0)
    , data()
{}

template<class T>
Frame<T>::Frame(const QVector<T>& vdata, size_t width, size_t height, TimestampData timestamp)
    : timestamp(timestamp)
    , data(Mapper(vdata.data(), height, width))
{}

template<class T>
Frame<T>::Frame(size_t width, size_t height, TimestampData timestamp)
    : timestamp(timestamp)
    , data(DataMatrix::Zero(height, width))
{}

template<class T>
Frame<T>::Frame(QFile& file, uint32_t width, uint32_t height, bool hasTimestamp, int64_t pos)
    : Frame()
{
    QVector<T> vdata(width * height);

    if (pos >= 0) {
        file.seek(pos);
    }
    if (hasTimestamp) {
        if (file.read(reinterpret_cast<char*>(&timestamp), sizeof(TimestampData)) < 0) {
            throw FileReadErrorException(file.fileName());
        }
    }
    if (file.read(reinterpret_cast<char*>(vdata.data()), vdata.size() * sizeof(T)) < 0) {
        throw FileReadErrorException(file.fileName());
    }

    data = Mapper(vdata.data(), height, width);
}

template<class T>
Frame<T>::Frame(const Frame& other)
    : timestamp(other.timestamp)
    , data(other.data)
{}

template<class T>
template<class U>
Frame<T>::Frame(const Frame<U>& other)
    : timestamp(other.getTimestamp())
    , data(other.getData().template cast<T>())
{}

template<class T>
Frame<T>::Frame(Frame&& other)
    : timestamp(std::move(other.timestamp))
    , data(std::move(other.data))
{}

template<class T>
Frame<T>& Frame<T>::operator=(const Frame& other)
{
    timestamp = other.timestamp;
    data = other.data; // Check copy constructor
    return *this;
}

template<class T>
template<class U>
Frame<T>& Frame<T>::operator=(const Frame<U>& other)
{
    timestamp = other.getTimestamp();
    data = other.getData().template cast<T>();
    return *this;
}

template<class T>
Frame<T>& Frame<T>::operator=(Frame&& other)
{
    timestamp = std::move(other.timestamp);
    data = std::move(other.data);
    return *this;
}

template<class T>
template<class U>
Frame<U> Frame<T>::cast() {
    return Frame<U>(*this);
}

template<class T>
QVariantList Frame<T>::toQVariantList(int major, bool norm = true) const { // TODO, this is only for double images
    QVariantList result;
    int row = 0;
    int col = 0;
    int sz  = data.rows() * data.cols();
    result.reserve(sz);

    const T max = data.maxCoeff();
    const T min = data.minCoeff();

    for (int i = 0; i < sz; ++i) {
        const T aux = data(row, col);
        if (norm) {
            result.push_back((aux - min) / (max - min));
        }
        else if (aux > 1.0) {
            result.push_back(1.0);
        }
        else if (aux < 0.0) {
            result.push_back(0.0);
        }
        else {
            result.push_back(aux);
        }
        if (major == FrameConstants::ROW_MAJOR) {
            if (++row >= data.rows()) {
                col++;
                row = 0;
            }
        }
        else if (major == FrameConstants::COLUM_MAJOR) {
            if (++col >= data.cols()) {
                row++;
                col = 0;
            }
        }
    }
    return result; // No debería ser necesario
}

template<class T>
void Frame<T>::set(uint32_t row, uint32_t colum, T val) {
    data(row, colum) = std::move(val);
}

template<class T>
const T& Frame<T>::at(uint32_t row, uint32_t colum) const {
    return data(row, colum);
}

template<class T>
void Frame<T>::horizontalSplit(uint32_t colum, Frame& left, Frame& right) const {
    left  = Frame<T>();
    right = Frame<T>();
    left.timestamp  = timestamp;
    right.timestamp = timestamp;
    left.data  = DataMatrix(data.block(0, 0, data.rows(), colum));
    right.data = DataMatrix(data.block(0, colum, data.rows(), data.cols() - colum));
}

template<class T>
void Frame<T>::verticalSplit(uint32_t row, Frame& top, Frame& bottom) const {
    top    = Frame<T>();
    bottom = Frame<T>();
    top.timestamp    = timestamp;
    bottom.timestamp = timestamp;
    top.data    = DataMatrix(data.block(0, 0, row, data.cols()));
    bottom.data = DataMatrix(data.block(row, 0, data.rows() - row, data.cols()));
}

template<class T>
uint32_t Frame<T>::getBPP() const {
    return sizeof(T) * 8; // In bits
}

template<class T>
const typename Frame<T>::DataMatrix& Frame<T>::getData() const {
    return data;
}

template<class T>
typename Frame<T>::TimestampData Frame<T>::getTimestamp() const {
    return timestamp;
}

template<class T>
uint32_t Frame<T>::getWidth() const {
    return static_cast<uint32_t>(data.cols());
}

template<class T>
uint32_t Frame<T>::getHeight() const {
    return static_cast<uint32_t>(data.rows());
}

template<class T>
Frame<T>& Frame<T>::applyLog() {
    data = data.array().log();
    return *this;
}

template<class T>
Frame<T>& Frame<T>::operator+=(const Frame& other) {
    data += other.data;
    return *this;
}

template<class T>
Frame<T>& Frame<T>::operator-=(const Frame& other) {
    data -= other.data;
    return *this;
}

template<class T>
Frame<T>& Frame<T>::operator+=(const T& scalar) {
    data = data.array() + scalar; // TODO This should be data -= scalar
    return *this;
}

template<class T>
Frame<T>& Frame<T>::operator-=(const T& scalar) {
    data = data.array() - scalar; // TODO This should be data -= scalar
    return *this;
}

template<class T>
Frame<T>& Frame<T>::operator*=(const T& scalar) {
    data *= scalar;
    return *this;
}

template<class T>
Frame<T>& Frame<T>::operator/=(const T& scalar) {
    data /= scalar;
    return *this;
}

template<class T>
Frame<T> Frame<T>::operator+(const Frame& other) const {
    Frame<T> result;
    result.timestamp = timestamp;
    result.data = data + other.data;
    return result;
}

template<class T>
Frame<T> Frame<T>::operator-(const Frame& other) const {
    Frame<T> result;
    result.timestamp = timestamp;
    result.data = data - other.data;
    return result;
}

template<class T>
Frame<T> Frame<T>::operator*(const Frame& other) const {
    Frame<T> result;
    result.timestamp = timestamp;
    result.data = data.cwiseProduct(other.data);
    return result;
}

template<class T>
Frame<T> Frame<T>::operator/(const Frame& other) const {
    Frame<T> result;
    result.timestamp = timestamp;
    result.data = data.cwiseQuotient(other.data);
    return result;
}

template<class T>
T Frame<T>::getMin() const {
    return data.minCoeff();
}

template<class T>
T Frame<T>::getMax() const {
    return data.maxCoeff();
}


#endif // FRAME_H
