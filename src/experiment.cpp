#include "experiment.h"

Experiment::Experiment(const QString &path)
{
    load(path);
}

void Experiment::load(const QString &path)
{
    assert(sizeof(float ) == 4); // 32 bit Float  // TODO que ambos sean por configuración
    assert(sizeof(double) == 8); // 64 bit Double

    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) {
        throw FileNotFoundException(path);
    }

    // Width, Height, Bpp. If any fails, throw exception
    if (file.read(reinterpret_cast<char*>(&height), sizeof(uint32_t)) < 0) throw FileReadErrorException(path);
    if (file.read(reinterpret_cast<char*>(&width),  sizeof(uint32_t)) < 0) throw FileReadErrorException(path);
    if (file.read(reinterpret_cast<char*>(&bpp),    sizeof(uint32_t)) < 0) throw FileReadErrorException(path);

    // Frames
    dark = Frame<float>(file, width, height, FrameConstants::NO_TIMESTAMP).cast<double>();
    gain = Frame<float>(file, width, height, FrameConstants::NO_TIMESTAMP).cast<double>();

    while (!file.atEnd()) {
             if (bpp <=  8) frames.push_back(Frame< int8_t>(file, width, height, FrameConstants::HAS_TIMESTAMP).cast<double>());
        else if (bpp <= 16) frames.push_back(Frame<int16_t>(file, width, height, FrameConstants::HAS_TIMESTAMP).cast<double>());
        else if (bpp <= 32) frames.push_back(Frame<int32_t>(file, width, height, FrameConstants::HAS_TIMESTAMP).cast<double>());
        else if (bpp <= 64) frames.push_back(Frame<int64_t>(file, width, height, FrameConstants::HAS_TIMESTAMP).cast<double>());
        else throw FrameBPPTooBig(bpp);
    }

    qDebug() << frames.size();
    qDebug() << frames[0].getTimestamp();
    qDebug() << frames[1].getTimestamp();
    qDebug() << frames[2].getTimestamp();
    qDebug() << frames[3].getTimestamp();
    qDebug() << frames[0].getData()(0,0) << frames[0].getData()(0,1) << frames[0].getData()(1,0);
    qDebug() << frames[1].getData()(0,0) << frames[1].getData()(0,1) << frames[1].getData()(1,0);
    qDebug() << frames[2].getData()(0,0) << frames[2].getData()(0,1) << frames[2].getData()(1,0);

    file.close();
}


