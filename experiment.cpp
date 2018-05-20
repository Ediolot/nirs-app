#include "experiment.h"

Experiment::Experiment(const std::string &path)
{
    load(path);
}

void Experiment::load(const std::string &path)
{
    assert(sizeof(float ) == 4); // 32 bit Float  // TODO que ambos sean por configuración
    assert(sizeof(double) == 8); // 64 bit Double

    std::ifstream file(path, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        throw FileNotFoundException(path);
    }

    // Width, Height, Bpp
    file.read(reinterpret_cast<char*>(&height), sizeof(uint32_t));
    file.read(reinterpret_cast<char*>(&width),  sizeof(uint32_t));
    file.read(reinterpret_cast<char*>(&bpp),    sizeof(uint32_t));
    bpp = 16; // 16 es el mínimo para guardar 12bits

    // Frames
    dark = readNextFrame<float, double>(file, width, height);
    gain = readNextFrame<float, double>(file, width, height);

    while (true) {
        Frame<double> frame = readNextFrame<int16_t, double>(file, width, height, true); // TODO HAS_TIMESTAMP = true
        if (file.eof()) break;
        frames.push_back(frame);
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


