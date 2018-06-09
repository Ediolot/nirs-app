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
        throw FileNotFoundException(path.toStdString());
    }

    // Width, Height, Bpp
    file.read(reinterpret_cast<char*>(&height), sizeof(uint32_t)); // if an error occurs, such as when attempting to read from a device opened in
    file.read(reinterpret_cast<char*>(&width),  sizeof(uint32_t)); // WriteOnly mode, this function returns -1
    file.read(reinterpret_cast<char*>(&bpp),    sizeof(uint32_t)); // 0 is returned when no more data is available for reading
    bpp = 16; // 16 es el mínimo para guardar 12bits

    // Frames
    dark = readNextFrame<float, double>(file, width, height);
    gain = readNextFrame<float, double>(file, width, height);

    while (!file.atEnd()) {
        frames.push_back(readNextFrame<int16_t, double>(file, width, height, true)); // TODO HAS_TIMESTAMP = true
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


